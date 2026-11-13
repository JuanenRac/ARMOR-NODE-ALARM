// ARMOR-NODE-ALARM - host tests of the keypad (core/keypad.hpp: Wiegand, RS485 and the PIN being typed) and of the backup battery (core/backup_battery.hpp), with simulated frames and readings.
// No keypad and no battery were connected. Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "../core/backup_battery.hpp"
#include "../core/keypad.hpp"

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

using namespace armor::alarm;

static int ones(std::uint32_t value) { int n = 0; for (; value != 0; value >>= 1) n += static_cast<int>(value & 1u); return n; }

// A good 26-bit frame for a facility and a card: the even bit makes the first twelve data bits even, the odd bit makes the last twelve odd.
static std::uint32_t build26(std::uint8_t facility, std::uint16_t card) {
  const std::uint32_t data = (static_cast<std::uint32_t>(facility) << 16) | card;
  const std::uint32_t even = static_cast<std::uint32_t>(ones(data >> 12) % 2);
  const std::uint32_t odd = ones(data & 0xFFFu) % 2 == 0 ? 1u : 0u;
  return (even << 25) | (data << 1) | odd;
}

static void test_wiegand() {
  Wiegand26 card;
  CHECK(wiegand26_decode(build26(0x12, 0x3456), card) && card.facility == 0x12 && card.card == 0x3456);
  CHECK(wiegand26_decode(build26(0, 0), card) && card.facility == 0 && card.card == 0);
  CHECK(wiegand26_decode(build26(255, 65535), card) && card.facility == 255 && card.card == 65535);
  CHECK(!wiegand26_decode(build26(0x12, 0x3456) | (1u << 26), card));    // more than 26 bits
  CHECK(!wiegand26_decode(build26(0x12, 0x3456) ^ (1u << 25), card));    // the even bit is wrong
  CHECK(!wiegand26_decode(build26(0x12, 0x3456) ^ 1u, card));            // the odd bit is wrong
  CHECK(!wiegand26_decode(build26(0x12, 0x3456) ^ (1u << 20), card));    // a data bit flipped

  KeyPress key;
  for (std::uint8_t digit = 0; digit <= 9; ++digit) {
    const std::uint8_t byte = static_cast<std::uint8_t>(((~digit & 0x0Fu) << 4) | digit);
    CHECK(wiegand_key_decode(byte, key) && key.key == Key::kDigit && key.digit == static_cast<char>('0' + digit));
  }
  CHECK(wiegand_key_decode(0x5A, key) && key.key == Key::kClear);        // 10: `*`
  CHECK(wiegand_key_decode(0x4B, key) && key.key == Key::kEnter);        // 11: `#`
  CHECK(!wiegand_key_decode(0x3C, key));                                  // 12 is no key
  CHECK(!wiegand_key_decode(0x11, key));                                  // the high nibble is not the complement
}

static std::vector<std::uint8_t> frame(std::uint8_t address, const std::vector<std::uint8_t>& payload) {
  std::vector<std::uint8_t> out{0x7E, address, static_cast<std::uint8_t>(payload.size())};
  std::uint8_t check = static_cast<std::uint8_t>(address ^ payload.size());
  for (const std::uint8_t b : payload) { out.push_back(b); check = static_cast<std::uint8_t>(check ^ b); }
  out.push_back(check);
  out.push_back(0x7F);
  return out;
}

static bool decode(const std::vector<std::uint8_t>& bytes, std::uint8_t address, KeyPress& key) { return rs485_frame_decode(bytes.data(), bytes.size(), address, key); }

static void test_rs485() {
  KeyPress key;
  CHECK(decode(frame(5, {'7'}), 5, key) && key.key == Key::kDigit && key.digit == '7');
  CHECK(decode(frame(5, {'*', 'x', 'y'}), 5, key) && key.key == Key::kClear);
  CHECK(decode(frame(5, {'#'}), 5, key) && key.key == Key::kEnter);
  CHECK(!decode(frame(5, {'x'}), 5, key));                                 // not a key
  CHECK(!decode(frame(5, {'7'}), 6, key));                                 // another keypad
  std::vector<std::uint8_t> bad = frame(5, {'7'});
  bad[bad.size() - 2] ^= 1;                                                // wrong checksum
  CHECK(!decode(bad, 5, key));
  bad = frame(5, {'7'});
  bad[0] = 0x7D;                                                           // wrong start
  CHECK(!decode(bad, 5, key));
  bad = frame(5, {'7'});
  bad[bad.size() - 1] = 0x00;                                              // wrong end
  CHECK(!decode(bad, 5, key));
  CHECK(!rs485_frame_decode(nullptr, 3, 5, key));                          // too short (and the pointer is never read)
  CHECK(!decode({0x7E, 5, 0, 0, 0, 0x7F}, 5, key));                        // a payload of zero bytes
  CHECK(!decode(frame(5, {'1', '2', '3', '4', '5', '6', '7', '8', '9'}), 5, key));   // nine bytes: too long
  bad = frame(5, {'7'});
  bad.insert(bad.begin() + 4, 0x00);                                       // longer than it says
  CHECK(!decode(bad, 5, key));
}

static KeyPress digit(char c) { KeyPress k; k.key = Key::kDigit; k.digit = c; return k; }
static KeyPress other(Key which) { KeyPress k; k.key = which; return k; }

static void test_pin_entry() {
  PinEntry entry(10000, 4);
  CHECK(!entry.press(digit('1'), 1000) && !entry.press(digit('2'), 2000) && !entry.press(digit('3'), 3000) && !entry.press(digit('4'), 4000));
  CHECK(entry.typed() == 4);
  CHECK(entry.press(other(Key::kEnter), 5000) && entry.entered() == "1234" && entry.typed() == 0);
  CHECK(!entry.press(other(Key::kEnter), 6000));                           // nothing typed: nothing to try
  CHECK(!entry.press(digit('9'), 7000) && !entry.press(other(Key::kClear), 8000) && entry.typed() == 0);
  CHECK(!entry.press(digit('5'), 9000) && !entry.press(digit('6'), 30000));  // a long pause started over: only the six is kept
  CHECK(entry.typed() == 1 && entry.press(other(Key::kEnter), 31000) && entry.entered() == "6");
  CHECK(!entry.press(digit('1'), 40000) && !entry.press(digit('2'), 41000) && !entry.press(digit('3'), 42000) && !entry.press(digit('4'), 43000));
  CHECK(!entry.press(digit('5'), 44000) && entry.typed() == 1);            // the fifth digit started over
  CHECK(!entry.press(other(Key::kNone), 45000) && entry.typed() == 1);     // a key that is nothing changes nothing
  PinEntry defaults;
  CHECK(!defaults.press(digit('0'), 1) && defaults.press(other(Key::kEnter), 2) && defaults.entered() == "0");
}

static void test_battery() {
  BackupBattery battery;
  BatteryReading r = battery.update(2000.0);                  // 11 400 mV at the battery
  CHECK(r.state == BatteryState::kLow && r.percent == 22 && r.battery_mv > 11399.0 && r.battery_mv < 11401.0);
  r = battery.update(2250.0);                                 // 12 825 mV: full, but a low battery needs the margin: 11 800 + 150 is passed
  CHECK(r.state == BatteryState::kGood && r.percent == 100);
  r = battery.update(2100.0);                                 // 11 970 mV: still good (it was good, no margin on the way down)
  CHECK(r.state == BatteryState::kGood);
  r = battery.update(2000.0);                                 // 11 400: low
  CHECK(r.state == BatteryState::kLow);
  r = battery.update(2100.0);                                 // 11 970: between 11 800 and 11 950+... above the margin (11 950): good again
  CHECK(r.state == BatteryState::kGood);
  r = battery.update(2057.0);                                 // 11 724: low
  CHECK(r.state == BatteryState::kLow);
  r = battery.update(2085.0);                                 // 11 884: above low_mv but inside the margin: still low
  CHECK(r.state == BatteryState::kLow);
  r = battery.update(1900.0);                                 // 10 830: critical
  CHECK(r.state == BatteryState::kCritical && r.percent == 0);
  r = battery.update(1950.0);                                 // 11 115: above empty but inside the margin: still critical
  CHECK(r.state == BatteryState::kCritical);
  r = battery.update(1990.0);                                 // 11 343: low
  CHECK(r.state == BatteryState::kLow);
  r = battery.update(100.0);                                  // 570 mV: nothing connected
  CHECK(r.state == BatteryState::kMissing && battery.state() == BatteryState::kMissing);
  r = battery.update(2000.0);
  CHECK(r.state == BatteryState::kLow);                       // back from missing: judged by the voltage
  BackupBattery fresh;
  CHECK(fresh.state() == BatteryState::kUnknown && fresh.update(2100.0).state == BatteryState::kGood);
  BackupBatteryLimits limits;
  limits.divider = 1.0; limits.empty_mv = 3000.0; limits.full_mv = 4200.0; limits.low_mv = 3500.0; limits.missing_mv = 1000.0; limits.margin_mv = 50.0;
  BackupBattery cell(limits);                                 // a single lithium cell read straight
  CHECK(cell.update(3600.0).percent == 50 && cell.state() == BatteryState::kGood);
}

int main() {
  test_wiegand();
  test_rs485();
  test_pin_entry();
  test_battery();
  std::printf("%d checks, %d failures\n", checks, failures);
  return failures == 0 ? 0 : 1;
}
