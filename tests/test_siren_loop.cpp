// ARMOR-NODE-ALARM - host tests of the supervision of the siren's wiring (core/siren_loop.hpp): the bands of the return line, the time a reading has to hold, what is not judged while the siren
// sounds, a return line that cannot be read, the settings, the state message ("siren_loop" and "siren_fault") and the refusal to arm that is off until the settings turn it on. The loop is
// simulated: a siren that is connected, cut, shorted or sounding, with noise on the line. No board has the return input yet.
// Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#include <cstdint>
#include <cstdio>
#include <string>

#include "../core/alarm_config.hpp"
#include "../core/alarm_message.hpp"
#include "../core/alarm_supervisor.hpp"
#include "../core/alarm_wiring.hpp"
#include "../core/siren_loop.hpp"

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

using namespace armor;
using alarm::SirenLoop;
using alarm::SirenLoopState;

// ---- the simulated loop --------------------------------------------------------------------------------------------------------------

// The voltage on the return pin: the divider of the installer gives about 1 V with the siren in place, the pin is pulled to 3.1 V when the wire is cut and to about 0 V when it is shorted;
// a siren that sounds draws current and the voltage sags. A little noise on top, repeatable.
struct SimLoop {
  enum class Wire { kConnected, kCut, kShorted } wire = Wire::kConnected;
  bool sounding = false;
  std::uint32_t seed = 7;
  double mv() {
    seed = seed * 1664525u + 1013904223u;
    const double noise = (static_cast<double>(seed >> 20) / 4096.0 - 0.5) * 40.0;   // +-20 mV
    switch (wire) {
      case Wire::kCut: return 3100.0 + noise;
      case Wire::kShorted: return 40.0 + noise;
      case Wire::kConnected: return (sounding ? 1350.0 : 1000.0) + noise;
    }
    return 0;
  }
};

// Runs a loop for `seconds`, one reading every 200 ms; returns the time of the first moment `fault()` was true, or -1.
static long run(SirenLoop& loop, SimLoop& sim, long from_ms, long seconds) {
  long first_fault = -1;
  for (long t = from_ms; t < from_ms + seconds * 1000; t += 200) {
    loop.feed(sim.mv(), sim.sounding, static_cast<std::uint64_t>(t));
    if (first_fault < 0 && loop.fault()) first_fault = t;
  }
  return first_fault;
}

static alarm::SirenLoopTiming timing(std::uint32_t hold, std::uint32_t clear, bool while_sounding = false) { alarm::SirenLoopTiming t; t.hold_ms = hold; t.clear_ms = clear; t.read_while_sounding = while_sounding; return t; }

static void test_the_bands() {
  const alarm::SirenLoopLimits limits;
  CHECK(alarm::siren_limits_are_valid(limits));
  CHECK(alarm::classify_siren_loop(0, limits) == SirenLoopState::kShort && alarm::classify_siren_loop(299, limits) == SirenLoopState::kShort);
  CHECK(alarm::classify_siren_loop(300, limits) == SirenLoopState::kOk && alarm::classify_siren_loop(1999, limits) == SirenLoopState::kOk);
  CHECK(alarm::classify_siren_loop(2000, limits) == SirenLoopState::kOpen && alarm::classify_siren_loop(3300, limits) == SirenLoopState::kOpen);
  CHECK(!alarm::siren_limits_are_valid({0, 2000}) && !alarm::siren_limits_are_valid({2000, 300}) && !alarm::siren_limits_are_valid({300, 3200}) && alarm::siren_limits_are_valid({300, 3000}));
  CHECK(std::string(alarm::siren_loop_text(SirenLoopState::kOk)) == "ok" && std::string(alarm::siren_loop_text(SirenLoopState::kOpen)) == "open" && std::string(alarm::siren_loop_text(SirenLoopState::kShort)) == "short" &&
        std::string(alarm::siren_loop_text(SirenLoopState::kUnreadable)) == "unreadable" && std::string(alarm::siren_loop_text(SirenLoopState::kUnknown)) == "unknown");
  CHECK(alarm::siren_loop_is_fault(SirenLoopState::kOpen) && alarm::siren_loop_is_fault(SirenLoopState::kShort) && alarm::siren_loop_is_fault(SirenLoopState::kUnreadable) &&
        !alarm::siren_loop_is_fault(SirenLoopState::kOk) && !alarm::siren_loop_is_fault(SirenLoopState::kUnknown));
}

static void test_a_cut_or_shorted_wire_is_reported_after_the_hold() {
  for (const SimLoop::Wire bad : {SimLoop::Wire::kCut, SimLoop::Wire::kShorted}) {
    SirenLoop loop(true, alarm::SirenLoopLimits{}, timing(3000, 2000));
    SimLoop sim;
    CHECK(loop.state() == SirenLoopState::kUnknown && !loop.fault());
    CHECK(run(loop, sim, 0, 10) < 0 && loop.state() == SirenLoopState::kOk);   // a good loop is never a fault, and is "ok" once it has held
    sim.wire = bad;
    const long at = run(loop, sim, 10000, 10);
    CHECK(at >= 13000 && at <= 13400);   // the hold (3 s) after the change at 10 s, to the reading
    CHECK(loop.state() == (bad == SimLoop::Wire::kCut ? SirenLoopState::kOpen : SirenLoopState::kShort) && loop.fault());
    const alarm::SirenLoopReport report = loop.report();
    CHECK(report.supervised && report.fault && report.state == loop.state());
    // mended: the fault stays for the clear time and then goes
    sim.wire = SimLoop::Wire::kConnected;
    CHECK(run(loop, sim, 20000, 1) >= 0 && loop.fault());   // one second later it is still there (2 s to clear)
    run(loop, sim, 21000, 5);
    CHECK(!loop.fault() && loop.state() == SirenLoopState::kOk);
  }
}

static void test_a_blip_is_not_a_fault() {
  SirenLoop loop(true, alarm::SirenLoopLimits{}, timing(3000, 3000));
  SimLoop sim;
  run(loop, sim, 0, 10);
  // a gust of wind on the wire: two seconds of open line, then back
  sim.wire = SimLoop::Wire::kCut;
  run(loop, sim, 10000, 2);
  sim.wire = SimLoop::Wire::kConnected;
  run(loop, sim, 12000, 10);
  CHECK(!loop.fault() && loop.state() == SirenLoopState::kOk && loop.changes() == 1);
  // alternating bad and good every second never holds long enough either way
  for (long t = 30000; t < 60000; t += 1000) { sim.wire = (t / 1000) % 2 ? SimLoop::Wire::kCut : SimLoop::Wire::kConnected; run(loop, sim, t, 1); }
  CHECK(!loop.fault() && loop.changes() == 1);
}

static void test_nothing_is_judged_while_the_siren_sounds() {
  SirenLoop loop(true, alarm::SirenLoopLimits{}, timing(3000, 3000));
  SimLoop sim;
  run(loop, sim, 0, 10);
  // the siren sounds: its current moves the voltage, and a wire cut at that moment is not judged until it stops
  sim.sounding = true;
  sim.wire = SimLoop::Wire::kCut;
  CHECK(run(loop, sim, 10000, 20) < 0 && !loop.fault() && loop.state() == SirenLoopState::kOk);
  // it stops, the cut is still there: the fault is reported after the hold, counted from the end of the sound
  sim.sounding = false;
  const long at = run(loop, sim, 30000, 10);
  CHECK(at >= 33000 && at <= 33400);
  // the settings can ask for it to be read while it sounds
  SirenLoop always(true, alarm::SirenLoopLimits{}, timing(3000, 3000, true));
  SimLoop sounding;
  sounding.sounding = true;
  run(always, sounding, 0, 10);
  CHECK(always.state() == SirenLoopState::kOk);   // a siren that sags to 1.35 V is still in the "ok" band
  sounding.wire = SimLoop::Wire::kCut;
  CHECK(run(always, sounding, 10000, 10) >= 13000 && always.fault());
}

static void test_an_unreadable_return_is_a_fault() {
  SirenLoop loop(true, alarm::SirenLoopLimits{}, timing(3000, 3000));
  SimLoop sim;
  run(loop, sim, 0, 10);
  CHECK(loop.state() == SirenLoopState::kOk);
  // the converter stops answering: four rounds are nothing, five make the loop unreadable once the hold has passed
  for (long t = 10000; t < 10800; t += 200) loop.feed_unreadable(false, static_cast<std::uint64_t>(t));
  CHECK(!loop.fault());
  for (long t = 10800; t < 16000; t += 200) loop.feed_unreadable(false, static_cast<std::uint64_t>(t));
  CHECK(loop.state() == SirenLoopState::kUnreadable && loop.fault());
  // and a reading that comes back ends it
  run(loop, sim, 16000, 6);
  CHECK(!loop.fault() && loop.state() == SirenLoopState::kOk);
  // one good reading in between resets the count
  SirenLoop other(true, alarm::SirenLoopLimits{}, timing(1000, 1000));
  for (long t = 0; t < 600000; t += 200) { if ((t / 200) % 5 == 4) other.feed(1000.0, false, static_cast<std::uint64_t>(t)); else other.feed_unreadable(false, static_cast<std::uint64_t>(t)); }
  CHECK(!other.fault());
}

static void test_a_loop_that_is_not_supervised_says_nothing() {
  SirenLoop off;
  SimLoop sim;
  sim.wire = SimLoop::Wire::kCut;
  for (long t = 0; t < 20000; t += 200) { off.feed(sim.mv(), false, static_cast<std::uint64_t>(t)); off.feed_unreadable(false, static_cast<std::uint64_t>(t)); }
  CHECK(!off.enabled() && !off.fault() && off.state() == SirenLoopState::kUnknown && !off.report().supervised && !off.report().fault);
}

// ---- the node: settings, wiring, message, arming ---------------------------------------------------------------------------------------

static config::Settings house() {
  config::Settings s = config::default_settings("a1b2c3");
  s.node_id = "alarm-1";
  s.sta.enabled = true; s.sta.ssid = "casa"; s.sta.password = "una-clave-larga";
  for (std::size_t i = 0; i < 2; ++i) {
    config::ZoneSetting& z = s.zones[i];
    z.enabled = true; z.id = i == 0 ? "door" : "hall"; z.name = z.id; z.input = "digital"; z.pin = 4 + static_cast<int>(i); z.hold_ms = 20;
    z.kind = i == 0 ? alarm::ZoneKind::kEntry : alarm::ZoneKind::kInstant;
  }
  s.panel.pin = "1234";
  s.panel.timing.exit_delay_s = 0; s.panel.timing.entry_delay_s = 10; s.panel.timing.siren_max_s = 60;
  s.relays[0] = {true, "siren", "Siren", board::kRelayPins[0], false};
  s.panel.siren_relay = "siren";
  return s;
}
static config::Settings supervised() {
  config::Settings s = house();
  s.siren_loop.enabled = true;
  s.siren_loop.pin = board::kAnalogPins[0];
  s.siren_loop.timing = timing(1000, 1000);
  return s;
}
static bool has(const config::Problems& problems, const std::string& path, const std::string& code) { for (const config::Problem& p : problems) if (p.path == path && p.code == code) return true; return false; }

static void test_the_settings() {
  config::Settings s = house();
  CHECK(!s.siren_loop.enabled && !s.siren_loop.block_arming && s.siren_loop.pin == -1 && config::validate(s).empty());   // off, and not blocking, by default
  s = supervised();
  CHECK(config::validate(s).empty());
  const std::string text = config::to_json(s, true);
  config::Settings back;
  config::Problems problems;
  CHECK(config::load(text, config::default_settings("a1b2c3"), back, problems) && problems.empty());
  CHECK(back.siren_loop.enabled && back.siren_loop.pin == board::kAnalogPins[0] && back.siren_loop.timing.hold_ms == 1000 && back.siren_loop.timing.clear_ms == 1000 && !back.siren_loop.timing.read_while_sounding && !back.siren_loop.block_arming);
  CHECK(back.siren_loop.limits.short_below_mv == 300.0 && back.siren_loop.limits.ok_below_mv == 2000.0 && config::to_json(back, true) == text);
  // a document that sets some of it leaves the rest; a wrong one is refused by name
  config::Settings partial;
  problems.clear();
  CHECK(config::load("{\"siren_loop\":{\"block_arming\":true,\"hold_ms\":5000}}", s, partial, problems) && partial.siren_loop.block_arming && partial.siren_loop.timing.hold_ms == 5000 && partial.siren_loop.enabled && partial.siren_loop.timing.clear_ms == 1000);
  problems.clear();
  CHECK(!config::load("{\"siren_loop\":{\"hold_ms\":50}}", s, partial, problems) && has(problems, "siren_loop.hold_ms", "range"));
  problems.clear();
  CHECK(!config::load("{\"siren_loop\":{\"enabled\":\"yes\"}}", s, partial, problems) && has(problems, "siren_loop.enabled", "invalid"));
  problems.clear();
  CHECK(!config::load("{\"siren_loop\":{\"ok_below_mv\":200}}", s, partial, problems) && has(problems, "siren_loop.short_below_mv", "range"));
  // it needs a siren to supervise and a pin of its own that the converter can read
  config::Settings no_siren = supervised();
  no_siren.panel.siren_relay.clear();
  CHECK(has(config::validate(no_siren), "siren_loop.enabled", "needs_siren"));
  config::Settings no_pin = supervised();
  no_pin.siren_loop.pin = -1;
  CHECK(has(config::validate(no_pin), "siren_loop.pin", "required"));
  config::Settings digital_only = supervised();
  digital_only.siren_loop.pin = 0;
  CHECK(!config::validate(digital_only).empty() && (has(config::validate(digital_only), "siren_loop.pin", "not_analog") || has(config::validate(digital_only), "siren_loop.pin", "reserved")));
  config::Settings taken = supervised();
  taken.zones[0].input = "loop"; taken.zones[0].pin = board::kAnalogPins[0];
  CHECK(has(config::validate(taken), "siren_loop.pin", "conflict") || has(config::validate(taken), "zones.0.pin", "conflict"));
  // switched off, nothing of it is checked (a stale pin does not stop a node from being saved)
  config::Settings stale = house();
  stale.siren_loop.pin = 0;
  CHECK(config::validate(stale).empty());
}

static void test_the_wiring() {
  const auto relay_place = [](const config::Settings& s, const std::string& name) { int place = 0; for (const config::RelaySetting& r : s.relays) { if (!r.enabled) continue; if (r.name == name) return place; ++place; } return -1; };
  config::Settings s = house();
  alarm::Wiring w = alarm::plan_wiring(s, [&](const std::string& n) { return relay_place(s, n); });
  CHECK(!w.siren_loop && !w.needs_converter);   // two digital zones need no converter
  s = supervised();
  w = alarm::plan_wiring(s, [&](const std::string& n) { return relay_place(s, n); });
  CHECK(w.siren_loop && w.needs_converter && w.siren_relay >= 0);
  s.panel.siren_relay.clear();
  w = alarm::plan_wiring(s, [&](const std::string& n) { return relay_place(s, n); });
  CHECK(!w.siren_loop);
}

// The message the node publishes, parsed back.
static bool field_is(const std::string& text, const std::string& member, const std::string& expected) { return text.find("\"" + member + "\":" + expected) != std::string::npos; }

static void test_the_message() {
  alarm::Supervisor off(house());
  off.boot(alarm::Stored{}, 0);
  const std::string quiet = off.runner().state_json(1000, 1000);
  CHECK(quiet.find("siren_loop") == std::string::npos && quiet.find("siren_fault") == std::string::npos);   // a node that does not supervise says nothing of it: the message is the one every server knows
  alarm::Supervisor sup(supervised());
  sup.boot(alarm::Stored{}, 0);
  sup.runner().set_epoch(1700000000);
  const auto closed = [&](std::uint64_t now) { sup.runner().feed_digital(0, false, now); sup.runner().feed_digital(1, false, now); };
  SimLoop sim;
  std::uint64_t now = 0;
  const auto spin = [&](long ms) { for (long end = static_cast<long>(now) + ms; static_cast<long>(now) < end; now += 200) { closed(now); sup.runner().feed_siren_loop(sim.mv(), now); sup.runner().tick(now); } };
  CHECK(field_is(sup.runner().state_json(1, now), "siren_loop", "\"unknown\"") && field_is(sup.runner().state_json(1, now), "siren_fault", "false"));   // not read yet
  spin(5000);
  std::string text = sup.runner().state_json(1, now);
  CHECK(field_is(text, "siren_loop", "\"ok\"") && field_is(text, "siren_fault", "false"));
  // the message is made again at once when the wiring changes
  const std::string before = sup.state_message(true, 1, now);
  CHECK(!before.empty() && sup.state_message(false, 2, now + 100).empty());
  sim.wire = SimLoop::Wire::kCut;
  spin(3000);
  text = sup.runner().state_json(1, now);
  CHECK(field_is(text, "siren_loop", "\"open\"") && field_is(text, "siren_fault", "true"));
  CHECK(!sup.state_message(false, 3, now).empty());
  json::Value parsed;
  CHECK(json::parse(text, parsed) && parsed.string_or("siren_loop", "") == "open" && parsed.bool_or("siren_fault", false));
  sim.wire = SimLoop::Wire::kShorted;
  spin(3000);
  CHECK(field_is(sup.runner().state_json(1, now), "siren_loop", "\"short\"") && !sup.state_message(false, 4, now).empty());
  sim.wire = SimLoop::Wire::kConnected;
  spin(3000);
  text = sup.runner().state_json(1, now);
  CHECK(field_is(text, "siren_loop", "\"ok\"") && field_is(text, "siren_fault", "false"));
  // an unreadable return line
  for (int i = 0; i < 40; ++i) { closed(now); sup.runner().feed_siren_loop_unreadable(now); now += 200; }
  CHECK(field_is(sup.runner().state_json(1, now), "siren_loop", "\"unreadable\"") && field_is(sup.runner().state_json(1, now), "siren_fault", "true"));
}

static void test_arming_is_refused_only_when_the_settings_say_so() {
  for (const bool block : {false, true}) {
    config::Settings s = supervised();
    s.siren_loop.block_arming = block;
    alarm::Supervisor sup(s);
    sup.boot(alarm::Stored{}, 0);
    SimLoop sim;
    sim.wire = SimLoop::Wire::kCut;
    std::uint64_t now = 0;
    for (; now < 4000; now += 200) { sup.runner().feed_digital(0, false, now); sup.runner().feed_digital(1, false, now); sup.runner().feed_siren_loop(sim.mv(), now); sup.runner().tick(now); }
    CHECK(sup.runner().siren_loop().fault());
    const alarm::Refusal result = sup.runner().arm(alarm::Mode::kAway, false, now);
    if (block) {
      CHECK(result == alarm::Refusal::kZonesOpen && sup.runner().panel().phase() == alarm::Phase::kDisarmed);
      CHECK(sup.runner().arm(alarm::Mode::kAway, true, now) == alarm::Refusal::kNone);   // the person who says "arm anyway" is obeyed
    } else {
      CHECK(result == alarm::Refusal::kNone && sup.runner().panel().phase() != alarm::Phase::kDisarmed);   // reported, never in the way
    }
  }
  // with the wire in place the blocking setting changes nothing
  config::Settings s = supervised();
  s.siren_loop.block_arming = true;
  alarm::Supervisor sup(s);
  sup.boot(alarm::Stored{}, 0);
  SimLoop sim;
  std::uint64_t now = 0;
  for (; now < 4000; now += 200) { sup.runner().feed_digital(0, false, now); sup.runner().feed_digital(1, false, now); sup.runner().feed_siren_loop(sim.mv(), now); sup.runner().tick(now); }
  CHECK(!sup.runner().siren_loop().fault() && sup.runner().arm(alarm::Mode::kAway, false, now) == alarm::Refusal::kNone);
}

static void test_a_ringing_alarm_does_not_look_like_a_cut_siren() {
  alarm::Supervisor sup(supervised());
  sup.boot(alarm::Stored{}, 0);
  SimLoop sim;
  std::uint64_t now = 0;
  const auto spin = [&](long ms, bool door_open) { for (long end = static_cast<long>(now) + ms; static_cast<long>(now) < end; now += 200) {
    sup.runner().feed_digital(0, door_open, now); sup.runner().feed_digital(1, false, now);
    sim.sounding = sup.runner().siren();
    sup.runner().feed_siren_loop(sim.mv(), now); sup.runner().tick(now); } };
  spin(4000, false);
  CHECK(sup.runner().arm(alarm::Mode::kAway, false, now) == alarm::Refusal::kNone);
  spin(2000, false);
  spin(20000, true);   // the door opens: entry delay, then the alarm, and the siren sounds
  CHECK(sup.runner().panel().phase() == alarm::Phase::kAlarm && sup.runner().siren());
  spin(10000, true);
  CHECK(!sup.runner().siren_loop().fault() && sup.runner().siren_loop().state() == SirenLoopState::kOk);   // the sag of a siren that sounds is not a fault
}

int main() {
  test_the_bands();
  test_a_cut_or_shorted_wire_is_reported_after_the_hold();
  test_a_blip_is_not_a_fault();
  test_nothing_is_judged_while_the_siren_sounds();
  test_an_unreadable_return_is_a_fault();
  test_a_loop_that_is_not_supervised_says_nothing();
  test_the_settings();
  test_the_wiring();
  test_the_message();
  test_arming_is_refused_only_when_the_settings_say_so();
  test_a_ringing_alarm_does_not_look_like_a_cut_siren();
  std::printf("%d checks, %d failures\n", checks, failures);
  return failures == 0 ? 0 : 1;
}
