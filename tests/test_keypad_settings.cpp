// ARMOR-NODE-ALARM - host tests of the settings of the keypad and of the backup battery (off by default, read, checked and written back) and of their connection to the alarm loop,
// with simulated keys and readings. No keypad and no battery were connected. Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#include <cstdio>
#include <string>

#include "../core/alarm_config.hpp"
#include "../core/alarm_runner.hpp"

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

static config::Settings with_zone(bool keypad, bool battery) {
  config::Settings s = config::default_settings("a1b2c3");
  s.node_id = "alarm-1";
  s.zones[0].enabled = true;
  s.zones[0].id = "z1";
  s.zones[0].kind = ZoneKind::kInstant;
  s.zones[0].zigbee = "00158d0001a2b3c0";
  s.zones[0].pin = -1;
  s.panel.pin = "1234";
  s.keypad.enabled = keypad;
  s.backup_battery.enabled = battery;
  return s;
}

static bool type(Runner& node, const std::string& keys, std::uint64_t& now, Refusal& last) {
  bool attempted = false, any = false;
  for (const char c : keys) {
    KeyPress key;
    if (c == '#') key.key = Key::kEnter; else if (c == '*') key.key = Key::kClear; else { key.key = Key::kDigit; key.digit = c; }
    now += 500;
    const Refusal result = node.keypad_press(key, now, attempted);
    if (attempted) { last = result; any = true; }
  }
  return any;
}

static void test_settings() {
  config::Settings defaults = config::default_settings("a1b2c3");
  CHECK(!defaults.keypad.enabled && defaults.keypad.idle_s == 10 && defaults.keypad.max_digits == 8);
  CHECK(!defaults.backup_battery.enabled && defaults.backup_battery.limits.divider == 5.7 && defaults.backup_battery.limits.low_mv == 11800.0);
  const std::string plain = config::to_json(defaults, false);
  CHECK(plain.find(R"("keypad":{"enabled":false,"idle_s":10,"max_digits":8})") != std::string::npos && plain.find(R"("backup_battery":{"enabled":false)") != std::string::npos);

  // read, checked and written back, with the same values (the settings need a network to be valid at all)
  defaults.sta.enabled = true; defaults.sta.ssid = "casa"; defaults.sta.password = "una-clave-larga";
  config::Settings out;
  config::Problems problems;
  const char* text = R"({"keypad":{"enabled":true,"idle_s":20,"max_digits":6},"backup_battery":{"enabled":true,"divider":6.1,"empty_mv":10800,"full_mv":12600,"low_mv":11500,"missing_mv":2500,"margin_mv":100}})";
  CHECK(config::load("{}", defaults, out, problems) && !out.keypad.enabled && !out.backup_battery.enabled);   // the defaults pass the checks
  CHECK(config::load(text, defaults, out, problems));
  CHECK(out.keypad.enabled && out.keypad.idle_s == 20 && out.keypad.max_digits == 6);
  CHECK(out.backup_battery.enabled && out.backup_battery.limits.divider == 6.1 && out.backup_battery.limits.empty_mv == 10800.0 && out.backup_battery.limits.full_mv == 12600.0 &&
        out.backup_battery.limits.low_mv == 11500.0 && out.backup_battery.limits.missing_mv == 2500.0 && out.backup_battery.limits.margin_mv == 100.0);
  config::Settings again;
  problems.clear();
  CHECK(config::load(config::to_json(out, true), defaults, again, problems));
  CHECK(again.keypad.enabled && again.keypad.idle_s == 20 && again.keypad.max_digits == 6 && again.backup_battery.enabled && again.backup_battery.limits.divider == 6.1 &&
        again.backup_battery.limits.low_mv == 11500.0 && again.backup_battery.limits.margin_mv == 100.0);
  CHECK(config::to_json(again, true) == config::to_json(out, true));   // the round trip is stable

  // values out of range, or limits that contradict each other, are refused
  for (const char* bad : {R"({"keypad":{"idle_s":2}})", R"({"keypad":{"idle_s":61}})", R"({"keypad":{"max_digits":3}})", R"({"keypad":{"max_digits":9}})", R"({"keypad":{"enabled":"yes"}})",
                          R"({"backup_battery":{"divider":0.5}})", R"({"backup_battery":{"margin_mv":5000}})",
                          R"({"backup_battery":{"low_mv":13000}})",                      // low above full
                          R"({"backup_battery":{"empty_mv":11900}})",                    // empty above low
                          R"({"backup_battery":{"missing_mv":11500}})"}) {               // missing above empty
    problems.clear();
    config::Settings refused;
    CHECK(!config::load(bad, defaults, refused, problems));
  }
}

static void test_keypad_in_the_loop() {
  // a node whose switch is off ignores every key
  Runner off(with_zone(false, false));
  std::uint64_t now = 1000;
  Refusal last = Refusal::kNone;
  CHECK(off.arm(Mode::kAway, true, now) == Refusal::kNone);
  CHECK(!type(off, "1234#", now, last));

  // on: the right PIN disarms like the page of the panel
  Runner node(with_zone(true, false));
  now = 1000;
  CHECK(node.arm(Mode::kAway, true, now) == Refusal::kNone);
  CHECK(type(node, "1234#", now, last) && last == Refusal::kNone);
  CHECK(node.arm(Mode::kAway, true, now + 100) == Refusal::kNone);   // it was disarmed, so it arms again

  // a wrong PIN is refused and counts toward the lockout, a short pause throws the digits away, `*` clears
  CHECK(type(node, "9999#", now, last) && last == Refusal::kBadPin);
  CHECK(!type(node, "12*", now, last));
  now += 20000;   // longer than the idle time: the digits typed before are gone
  CHECK(!type(node, "12", now, last));
  now += 20000;
  CHECK(type(node, "34#", now, last) && last == Refusal::kBadPin);   // "34", not "1234"
  CHECK(node.keypad_battery_json().find(R"("typed":0)") != std::string::npos);
  for (int i = 0; i < 6; ++i) type(node, "0000#", now, last);
  CHECK(last == Refusal::kLockedOut || node.locked_out(now));
  CHECK(type(node, "1234#", now, last) && last == Refusal::kLockedOut);   // even the right PIN waits for the lockout
}

static void test_battery_in_the_loop() {
  Runner off(with_zone(false, false));
  off.battery_feed(2000.0);
  CHECK(off.keypad_battery_json().find(R"("backup_battery":{"enabled":false,"state":"unknown","percent":0,"battery_mv":0})") != std::string::npos);

  Runner node(with_zone(false, true));
  CHECK(node.keypad_battery_json().find(R"("backup_battery":{"enabled":true,"state":"unknown")") != std::string::npos);   // no reading yet
  node.battery_feed(2200.0);   // 12 540 mV with the divider 5.7
  std::string json = node.keypad_battery_json();
  CHECK(json.find(R"("state":"good")") != std::string::npos && json.find(R"("percent":86)") != std::string::npos && json.find(R"("battery_mv":12540)") != std::string::npos);
  node.battery_feed(2050.0);   // 11 685 mV: low
  CHECK(node.keypad_battery_json().find(R"("state":"low")") != std::string::npos);
  node.battery_feed(1800.0);   // 10 260 mV: critical
  CHECK(node.keypad_battery_json().find(R"("state":"critical")") != std::string::npos);
  node.battery_feed(1950.0);   // 11 115 mV: still critical (the margin) and not yet low
  CHECK(node.keypad_battery_json().find(R"("state":"critical")") != std::string::npos);
  node.battery_feed(100.0);    // 570 mV: nothing connected
  CHECK(node.keypad_battery_json().find(R"("state":"missing")") != std::string::npos);
}

int main() {
  test_settings();
  test_keypad_in_the_loop();
  test_battery_in_the_loop();
  std::printf("%d checks, %d failures\n", checks, failures);
  return failures == 0 ? 0 : 1;
}
