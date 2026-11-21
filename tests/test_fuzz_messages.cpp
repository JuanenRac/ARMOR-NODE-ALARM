// ARMOR-NODE-ALARM - host tests with a fixed seed: the readers of the messages an alarm node receives (the commands of the server, the feed of a Zigbee zone, the events kept in flash and the
// frames of the Zigbee coordinator) must never crash, must refuse anything that is not exactly the contract and must give back what was written.
// Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#include <cstdint>
#include <cstdio>
#include <optional>
#include <string>
#include <vector>

#include "../core/alarm_message.hpp"
#include "../core/zigbee_ias.hpp"
#include "../core/znp.hpp"

static int failures = 0;
static int checks = 0;
#define CHECK(condition)                                                              \
  do {                                                                                \
    ++checks;                                                                         \
    if (!(condition)) {                                                               \
      ++failures;                                                                     \
      std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #condition);                \
    }                                                                                 \
  } while (0)

// the same numbers on every run, so a failure can be repeated
static std::uint32_t rng_state = 0x7A3F9D15u;
static std::uint32_t next_random() {
  rng_state ^= rng_state << 13;
  rng_state ^= rng_state >> 17;
  rng_state ^= rng_state << 5;
  return rng_state;
}
static std::uint32_t below(std::uint32_t limit) { return limit == 0 ? 0 : next_random() % limit; }

static std::string noise_text(std::size_t length) {
  std::string out;
  for (std::size_t i = 0; i < length; ++i) out.push_back(static_cast<char>(next_random() & 0xff));
  return out;
}

// a valid text with a few characters changed, dropped or added
static std::string mutate(std::string text) {
  const std::uint32_t edits = 1 + below(4);
  for (std::uint32_t i = 0; i < edits && !text.empty(); ++i) {
    const std::size_t at = below(static_cast<std::uint32_t>(text.size()));
    switch (below(3)) {
      case 0: text[at] = static_cast<char>(next_random() & 0xff); break;
      case 1: text.erase(at, 1); break;
      default: text.insert(at, 1, static_cast<char>(next_random() & 0xff)); break;
    }
  }
  return text;
}

static std::string zone_id(std::size_t length) {
  static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789-_";
  std::string out;
  for (std::size_t i = 0; i < length; ++i) out.push_back(alphabet[below(sizeof(alphabet) - 1)]);
  if (!out.empty() && (out[0] == '-' || out[0] == '_')) out[0] = 'z';
  return out;
}

using namespace armor::alarm;

static const char kNode[] = "alarm-1";

static std::string command_text(const std::string& action, const std::string& tail) {
  return "{\"kind\":\"alarm_command\",\"node_id\":\"alarm-1\",\"timestamp_ms\":1000000,\"command_id\":\"c000000000000001\",\"action\":\"" + action + "\"" + tail + "}";
}

static const std::vector<std::string>& valid_commands() {
  static const std::vector<std::string> commands = {
      command_text("arm", ",\"mode\":\"away\""), command_text("arm", ",\"mode\":\"stay\",\"force\":true"), command_text("disarm", ""),
      command_text("silence", ""), command_text("arm", ",\"mode\":\"away\",\"force\":false")};
  return commands;
}

static void commands() {
  Command command;
  for (const std::string& text : valid_commands()) CHECK(parse_command(text, kNode, command) && valid_command_id(command.command_id));
  // another node, a field that is not in the contract, a wrong kind: no answer at all
  for (const std::string& text : valid_commands()) {
    CHECK(!parse_command(text, "alarm-2", command));
    std::string extra = text;
    extra.insert(extra.size() - 1, ",\"surprise\":1");
    CHECK(!parse_command(extra, kNode, command));
    std::string wrong_kind = text;
    wrong_kind.replace(wrong_kind.find("alarm_command"), 13, "alarm_comman_");
    CHECK(!parse_command(wrong_kind, kNode, command));
  }
  // a disarm or a silence that carries a mode, a force or a profile is not the contract's
  CHECK(!parse_command(command_text("disarm", ",\"mode\":\"away\""), kNode, command));
  CHECK(!parse_command(command_text("silence", ",\"force\":true"), kNode, command));
  CHECK(!parse_command(command_text("arm", ""), kNode, command));
  CHECK(!parse_command(command_text("arm", ",\"mode\":\"home\""), kNode, command));
  CHECK(!parse_command(command_text("arm", ",\"mode\":\"away\",\"force\":1"), kNode, command));
  CHECK(!parse_command(command_text("explode", ""), kNode, command));
  // the identifier of a command is sixteen lowercase hexadecimal digits
  for (int round = 0; round < 500; ++round) {
    std::string id;
    for (std::uint32_t i = below(24); i > 0; --i) id.push_back("0123456789abcdefABCDEFxyz"[below(25)]);
    bool expected = id.size() == 16;
    for (char c : id) if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) expected = false;
    CHECK(valid_command_id(id) == expected);
  }
  // damaged commands and noise: no crash, and whatever is accepted is a whole command
  for (int round = 0; round < 6000; ++round) {
    const std::string text = (round % 6 == 0) ? noise_text(below(200)) : mutate(valid_commands()[below(static_cast<std::uint32_t>(valid_commands().size()))]);
    Command read;
    if (parse_command(text, kNode, read)) {
      CHECK(valid_command_id(read.command_id));
      CHECK(read.action == Action::kArm || (!read.force && !read.has_profile));
    }
  }
}

static void zone_feed() {
  const std::string signature(64, 'a');
  const std::vector<std::string> feeds = {
      "{\"zone\":\"window\",\"state\":\"triggered\",\"battery_low\":true}", "{\"zone\":\"door-1\",\"state\":\"normal\"}", "{\"zone\":\"w_2\",\"state\":\"tamper\",\"battery_low\":false}",
      "{\"zone\":\"hall\",\"state\":\"normal\",\"seq\":42,\"sig\":\"" + signature + "\"}"};
  ZoneFeed feed;
  for (const std::string& text : feeds) CHECK(parse_zone_feed(text, feed));
  CHECK(parse_zone_feed(feeds[3], feed) && feed.signed_message && feed.seq == 42 && feed.sig == signature);
  // a sequence number with no signature (or the signature with no number) is not a message
  CHECK(!parse_zone_feed("{\"zone\":\"hall\",\"state\":\"normal\",\"seq\":42}", feed));
  CHECK(!parse_zone_feed("{\"zone\":\"hall\",\"state\":\"normal\",\"sig\":\"" + signature + "\"}", feed));
  CHECK(!parse_zone_feed("{\"zone\":\"hall\",\"state\":\"normal\",\"seq\":-1,\"sig\":\"" + signature + "\"}", feed));
  CHECK(!parse_zone_feed("{\"zone\":\"hall\",\"state\":\"normal\",\"seq\":1.5,\"sig\":\"" + signature + "\"}", feed));
  CHECK(!parse_zone_feed("{\"zone\":\"hall\",\"state\":\"normal\",\"seq\":1,\"sig\":\"" + std::string(63, 'a') + "\"}", feed));
  for (int round = 0; round < 6000; ++round) {
    const std::string text = (round % 6 == 0) ? noise_text(below(160)) : mutate(feeds[below(static_cast<std::uint32_t>(feeds.size()))]);
    ZoneFeed read;
    if (parse_zone_feed(text, read)) {
      CHECK(!read.zone_id.empty() && read.zone_id.size() <= 32 && read.zone_id.front() != '-' && read.zone_id.front() != '_');
      CHECK(!read.signed_message || read.sig.size() == 64);
    }
  }
  // any zone name made of the allowed characters is accepted and given back as it came; one longer than 32 is not
  for (int round = 0; round < 500; ++round) {
    const std::string id = zone_id(1 + below(40));
    ZoneFeed read;
    const bool accepted = parse_zone_feed("{\"zone\":\"" + id + "\",\"state\":\"normal\"}", read);
    CHECK(accepted == (id.size() <= 32));
    if (accepted) CHECK(read.zone_id == id);
  }
}

static void events_in_flash() {
  for (int round = 0; round < 500; ++round) {
    std::vector<Event> events;
    for (std::uint32_t i = below(16); i > 0; --i) {
      Event event;
      event.kind = static_cast<EventKind>(below(10));
      event.epoch = below(3) == 0 ? 0 : static_cast<std::int64_t>(1 + below(2000000000u));
      event.zone = zone_id(below(33));
      events.push_back(event);
    }
    std::vector<Event> kept;   // what is saved: the last ten that happened with real time
    for (const Event& event : events) if (event.epoch != 0) kept.push_back(event);
    if (kept.size() > kSavedEvents) kept.erase(kept.begin(), kept.end() - static_cast<std::ptrdiff_t>(kSavedEvents));
    const std::vector<Event> back = parse_events(events_text(events));
    CHECK(back.size() == kept.size());
    for (std::size_t i = 0; i < back.size() && i < kept.size(); ++i) CHECK(back[i].epoch == kept[i].epoch && back[i].kind == kept[i].kind && back[i].zone == kept[i].zone);
  }
  // damaged or invented text: no crash, and every event read has a real kind, a long enough number and a zone of at most 32 characters
  const std::string alphabet = "0123456789,;abcxyz_-";
  const std::vector<std::string> kinds = {"armed", "alarm", "disarmed", "bad_pin", "zone_bypassed", "nonsense"};
  for (int round = 0; round < 6000; ++round) {
    std::string text;
    if (round % 3 == 0) {
      for (std::uint32_t i = below(60); i > 0; --i) text.push_back(alphabet[below(static_cast<std::uint32_t>(alphabet.size()))]);
    } else {
      for (std::uint32_t i = below(5); i > 0; --i) text += std::to_string(below(2000000000u) + (below(4) == 0 ? 0u : 1u)) + "," + kinds[below(static_cast<std::uint32_t>(kinds.size()))] + "," + zone_id(below(40)) + ";";
      text = mutate(text);
    }
    for (const Event& event : parse_events(text)) CHECK(event.zone.size() <= 32 && static_cast<int>(event.kind) >= 0 && static_cast<int>(event.kind) <= 9);
  }
}

static std::vector<std::uint8_t> bytes_of(std::size_t length) {
  std::vector<std::uint8_t> out;
  for (std::size_t i = 0; i < length; ++i) out.push_back(static_cast<std::uint8_t>(below(256)));
  return out;
}

static void coordinator_frames() {
  using armor::znp::Frame;
  using armor::znp::Parser;
  // frames written and read again, one after the other, with noise (not holding a start byte) between them
  for (int round = 0; round < 200; ++round) {
    std::vector<Frame> sent;
    std::vector<std::uint8_t> stream;
    for (std::uint32_t i = 1 + below(6); i > 0; --i) {
      Frame frame;
      frame.type = static_cast<armor::znp::Type>(below(4));
      frame.subsystem = static_cast<std::uint8_t>(below(32));
      frame.command = static_cast<std::uint8_t>(below(256));
      frame.data = bytes_of(below(3) == 0 ? below(armor::znp::kMaxData + 1) : below(30));
      const std::vector<std::uint8_t> encoded = armor::znp::encode(frame);
      CHECK(!encoded.empty());
      for (std::uint32_t k = below(5); k > 0; --k) stream.push_back(static_cast<std::uint8_t>(below(254)));   // never 0xFE
      stream.insert(stream.end(), encoded.begin(), encoded.end());
      sent.push_back(frame);
    }
    Parser parser;
    for (std::uint8_t byte : stream) parser.feed(byte);
    for (const Frame& frame : sent) {
      Frame got;
      const bool popped = parser.pop(got);
      CHECK(popped);
      if (popped) CHECK(got.type == frame.type && got.subsystem == frame.subsystem && got.command == frame.command && got.data == frame.data);
    }
    Frame extra;
    CHECK(!parser.pop(extra) && parser.dropped() == 0);
  }
  CHECK(armor::znp::encode(Frame{armor::znp::Type::kSreq, 1, 1, std::vector<std::uint8_t>(armor::znp::kMaxData + 1, 0)}).empty());
  // a byte of a frame changed (after its length byte): the checksum drops it
  for (int round = 0; round < 500; ++round) {
    Frame frame;
    frame.type = armor::znp::Type::kAreq;
    frame.subsystem = armor::znp::sub::kAf;
    frame.command = 0x81;
    frame.data = bytes_of(1 + below(40));
    std::vector<std::uint8_t> encoded = armor::znp::encode(frame);
    const std::size_t at = 2 + below(static_cast<std::uint32_t>(encoded.size() - 2));
    encoded[at] = static_cast<std::uint8_t>(encoded[at] + 1 + below(255));
    Parser parser;
    for (std::uint8_t byte : encoded) parser.feed(byte);
    CHECK(parser.dropped() >= 1);
  }
  // a stream of anything (start bytes included): no crash, the queue never holds more than it may, and every frame that comes out has a length it can have
  Parser parser;
  for (int round = 0; round < 40; ++round) {
    for (std::uint8_t byte : bytes_of(2000)) parser.feed(byte);
    int taken = 0;
    Frame out;
    while (parser.pop(out)) { ++taken; CHECK(out.data.size() <= armor::znp::kMaxData); }
    CHECK(taken <= 64);
  }
}

static void zigbee_messages() {
  using armor::znp::Frame;
  // an incoming message of a door contact: the notification of a change of zone, read through the frame, the ZCL header and the cluster
  for (int round = 0; round < 500; ++round) {
    const std::uint16_t status = static_cast<std::uint16_t>(below(65536));
    const std::uint8_t zone = static_cast<std::uint8_t>(below(256));
    std::vector<std::uint8_t> zcl = {0x09, static_cast<std::uint8_t>(below(256)), 0x00, static_cast<std::uint8_t>(status & 0xFF), static_cast<std::uint8_t>(status >> 8), 0x00, zone, 0x00, 0x00};
    std::vector<std::uint8_t> data(17, 0);
    data[2] = 0x00; data[3] = 0x05;   // the IAS Zone cluster
    data[4] = static_cast<std::uint8_t>(below(256)); data[5] = static_cast<std::uint8_t>(below(256));
    data[16] = static_cast<std::uint8_t>(zcl.size());
    data.insert(data.end(), zcl.begin(), zcl.end());
    Frame frame;
    frame.type = armor::znp::Type::kAreq;
    frame.subsystem = armor::znp::sub::kAf;
    frame.command = 0x81;
    frame.data = data;
    const std::optional<armor::zigbee::Incoming> incoming = armor::zigbee::incoming_message(frame);
    CHECK(incoming.has_value());
    if (!incoming) continue;
    const std::optional<armor::zigbee::ZoneStatusChange> change = armor::zigbee::zone_status_change(*incoming);
    CHECK(change.has_value() && change->status == status && change->zone_id == zone);
    CHECK(!armor::zigbee::enroll_request(*incoming).has_value());
    // the same message cut short is no message
    Frame cut = frame;
    cut.data.resize(below(static_cast<std::uint32_t>(frame.data.size())));
    const std::optional<armor::zigbee::Incoming> shorter = armor::zigbee::incoming_message(cut);
    if (shorter) CHECK(cut.data.size() >= 17 + shorter->payload.size());
  }
  // anything at all: no crash, and what is read stays inside what arrived
  for (int round = 0; round < 6000; ++round) {
    Frame frame;
    frame.type = below(4) == 0 ? armor::znp::Type::kSreq : armor::znp::Type::kAreq;
    frame.subsystem = below(5) == 0 ? static_cast<std::uint8_t>(below(32)) : armor::znp::sub::kAf;
    frame.command = below(5) == 0 ? static_cast<std::uint8_t>(below(256)) : 0x81;
    frame.data = bytes_of(below(70));
    if (frame.data.size() >= 17 && below(2) == 0) { frame.data[3] = 0x05; frame.data[2] = 0x00; }
    const std::optional<armor::zigbee::Incoming> incoming = armor::zigbee::incoming_message(frame);
    if (incoming) {
      CHECK(incoming->payload.size() <= frame.data.size());
      (void)armor::zigbee::zone_status_change(*incoming);
      (void)armor::zigbee::enroll_request(*incoming);
    }
    const std::optional<armor::zigbee::Zcl> zcl = armor::zigbee::parse_zcl(frame.data);
    if (zcl) CHECK(zcl->payload.size() <= frame.data.size());
  }
}

int main() {
  commands();
  zone_feed();
  events_in_flash();
  coordinator_frames();
  zigbee_messages();
  std::printf("fuzz_messages: %d checks, %d failed\n", checks, failures);
  return failures == 0 ? 0 : 1;
}
