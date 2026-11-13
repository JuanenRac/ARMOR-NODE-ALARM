// ARMOR-NODE-ALARM - host tests of the walk test (core/walk_test.hpp) and of the suspicion of a jammed radio (core/radio_watch.hpp), with a simulated coordinator that goes quiet.
// No hardware was used. Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#include <cstdio>
#include <string>
#include <vector>

#include "../core/alarm_config.hpp"
#include "../core/alarm_runner.hpp"
#include "../core/radio_watch.hpp"
#include "../core/walk_test.hpp"

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
using namespace armor::alarm;

static void test_walk_test() {
  WalkTest off(false);
  CHECK(!off.start(1000) && !off.running(1000) && !off.seen(1, 1000));   // not allowed: nothing happens
  WalkTest walk(true);
  CHECK(!walk.running(0) && !walk.seen(1, 0));                           // not started
  CHECK(walk.start(1000, 5));                                            // too short: kept at a minute
  CHECK(walk.running(1000 + 59000) && !walk.running(1000 + 60000));
  CHECK(!walk.start(2000));                                              // already running
  walk.stop();
  CHECK(!walk.running(2000));
  CHECK(walk.start(10000, 99999));                                       // too long: kept at half an hour
  CHECK(walk.running(10000 + 1799000) && !walk.running(10000 + 1800000));
  CHECK(!walk.seen(0, 20000) && !walk.seen(9, 20000));                   // no such zone
  CHECK(walk.seen(1, 20000) && walk.seen(3, 21000) && !walk.seen(1, 22000));
  CHECK(walk.seen_mask() == 0x5u);
  CHECK(walk.report(30000, 0x07u) == "{\"running\":true,\"seen\":[1,3],\"missing\":[2]}");
  CHECK(walk.report(30000, 0x00u) == "{\"running\":true,\"seen\":[],\"missing\":[]}");
  CHECK(walk.report(10000 + 1800000, 0x05u) == "{\"running\":false,\"seen\":[1,3],\"missing\":[]}");
  CHECK(!walk.seen(2, 10000 + 1800000));                                 // after the end nothing is written down
  CHECK(walk.start(10000 + 1800000, 120) && walk.seen_mask() == 0);      // a new walk starts clean
  CHECK(!walk.start(0));                                                 // (still running)
  WalkTest defaults(true);
  CHECK(defaults.start(0) && defaults.running(299000) && !defaults.running(300000));
}

// A simulated Zigbee coordinator with sensors that report every minute; the first `deaf` of them are not heard after `jam_from_ms`.
static std::vector<std::uint64_t> heard_at(std::uint64_t now_ms, std::uint64_t jam_from_ms, std::size_t deaf, std::size_t sensors = 5) {
  std::vector<std::uint64_t> out(sensors, 0);
  for (std::size_t i = 0; i < sensors; ++i) {
    const std::uint64_t reporting_until = i < deaf ? jam_from_ms : now_ms;
    out[i] = reporting_until - (reporting_until % 60000ULL);
  }
  return out;
}

static void test_radio_watch() {
  const RadioWatchLimits limits{};   // ten minutes of silence, three sensors at least, 75 %, held for two minutes
  RadioWatch off(false, limits);
  CHECK(off.update(heard_at(5000000, 0, 5), 0, 5000000) == RadioVerdict::kOff);
  RadioWatch watch(true, limits);
  CHECK(watch.update(heard_at(100000, 0, 0, 2), 0, 100000) == RadioVerdict::kTooFew);
  CHECK(watch.update(heard_at(1000000, 0, 0), 0, 1000000) == RadioVerdict::kHealthy);
  // one flat battery among five: healthy
  CHECK(watch.update(heard_at(2000000, 600000, 1), 0, 2000000) == RadioVerdict::kHealthy);
  // all five go deaf at 600 s: after ten minutes of silence it starts to watch, after the hold it is suspected
  CHECK(watch.update(heard_at(1200000, 600000, 5), 0, 1200000) == RadioVerdict::kHealthy);   // exactly ten minutes: not over yet
  CHECK(watch.update(heard_at(1260000, 600000, 5), 0, 1260000) == RadioVerdict::kWatching);
  CHECK(watch.update(heard_at(1320000, 600000, 5), 0, 1320000) == RadioVerdict::kWatching);
  CHECK(watch.update(heard_at(1380000, 600000, 5), 0, 1380000) == RadioVerdict::kSuspected);
  CHECK(watch.update(heard_at(1400000, 1400000, 0), 0, 1400000) == RadioVerdict::kHealthy);   // they are heard again
  // four of five (80 %) is enough, three of five (60 %) is not
  RadioWatch share(true, limits);
  CHECK(share.update(heard_at(1260000, 600000, 4), 0, 1260000) == RadioVerdict::kWatching);
  RadioWatch three(true, limits);
  CHECK(three.update(heard_at(1260000, 600000, 3), 0, 1260000) == RadioVerdict::kHealthy);
  // a node that has only just started listening does not count the time before it listened
  RadioWatch fresh(true, limits);
  CHECK(fresh.update(std::vector<std::uint64_t>(5, 0), 5000000, 5100000) == RadioVerdict::kHealthy);
  CHECK(fresh.update(std::vector<std::uint64_t>(5, 0), 5000000, 5700000) == RadioVerdict::kWatching);
  CHECK(fresh.update(std::vector<std::uint64_t>(5, 0), 5000000, 6000000) == RadioVerdict::kSuspected);
  CHECK(fresh.update(std::vector<std::uint64_t>(5, 7000000), 5000000, 7000000) == RadioVerdict::kHealthy);   // a heard time after the start is used as it is
}

static config::Settings three_sensors(bool walk, bool radio) {
  config::Settings s = config::default_settings("a1b2c3");
  s.node_id = "alarm-1";
  for (std::size_t i = 0; i < 3; ++i) {
    s.zones[i].enabled = true;
    s.zones[i].id = "z" + std::to_string(i + 1);
    s.zones[i].kind = ZoneKind::kInstant;
    s.zones[i].zigbee = "00158d0001a2b3c" + std::to_string(i);
    s.zones[i].pin = -1;
  }
  s.panel.pin = "1234";
  s.walk_test.enabled = walk;
  s.radio_watch.enabled = radio;
  return s;
}

static void test_settings_and_runner() {
  // off by default, and the values are read, checked and written back
  config::Settings defaults = config::default_settings("a1b2c3");
  CHECK(!defaults.walk_test.enabled && defaults.walk_test.seconds == 300 && !defaults.radio_watch.enabled && defaults.radio_watch.limits.share_percent == 75);
  defaults.sta.enabled = true; defaults.sta.ssid = "casa"; defaults.sta.password = "una-clave-larga";
  config::Settings out;
  config::Problems problems;
  CHECK(config::load(R"({"walk_test":{"enabled":true,"seconds":120},"radio_watch":{"enabled":true,"silence_s":300,"min_sensors":4,"share_percent":60,"hold_s":30}})", defaults, out, problems));
  CHECK(out.walk_test.enabled && out.walk_test.seconds == 120 && out.radio_watch.enabled && out.radio_watch.limits.silence_s == 300 && out.radio_watch.limits.min_sensors == 4 &&
        out.radio_watch.limits.share_percent == 60 && out.radio_watch.limits.hold_s == 30);
  const std::string text = config::to_json(out, true);
  CHECK(text.find(R"("walk_test":{"enabled":true,"seconds":120})") != std::string::npos && text.find(R"("share_percent":60)") != std::string::npos);
  problems.clear();
  CHECK(!config::load(R"({"walk_test":{"seconds":5}})", defaults, out, problems));
  problems.clear();
  CHECK(!config::load(R"({"radio_watch":{"share_percent":20}})", defaults, out, problems));

  // a node whose switch is off never starts a walk
  Runner off(three_sensors(false, false));
  CHECK(!off.start_walk_test(1000) && !off.walk_test_running(1000));
  CHECK(off.radio_verdict(5000000) == RadioVerdict::kOff);

  // a walk: only with the panel disarmed, the zones that see the person are written down, arming ends it
  Runner node(three_sensors(true, true));
  CHECK(node.start_walk_test(1000) && node.walk_test_running(1000));
  node.feed_zigbee("z1", ZoneState::kTriggered, false, 2000);
  node.feed_zigbee("z2", ZoneState::kNormal, false, 3000);
  node.feed_zigbee("z3", ZoneState::kTriggered, false, 4000);
  CHECK(node.walk_test_report(5000) == "{\"running\":true,\"seen\":[1,3],\"missing\":[2]}");
  CHECK(node.arm(Mode::kAway, true, 6000) == Refusal::kNone);
  CHECK(!node.walk_test_running(6001));
  CHECK(!node.start_walk_test(7000));   // armed: no walk
  node.stop_walk_test();

  // the radio: the sensors were heard, then none for a long time
  for (std::size_t i = 1; i <= 3; ++i) node.feed_zigbee("z" + std::to_string(i), ZoneState::kNormal, false, 10000);
  CHECK(node.radio_verdict(20000) == RadioVerdict::kHealthy);
  CHECK(node.radio_verdict(10000 + 601000) == RadioVerdict::kWatching);
  CHECK(node.radio_verdict(10000 + 601000 + 120000) == RadioVerdict::kSuspected);
}

int main() {
  test_walk_test();
  test_radio_watch();
  test_settings_and_runner();
  std::printf("%d checks, %d failures\n", checks, failures);
  return failures == 0 ? 0 : 1;
}
