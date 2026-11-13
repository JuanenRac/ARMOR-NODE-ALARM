// ARMOR-NODE-ALARM - a keypad at the door, without the hardware: a Wiegand reader (26 bits for a card, 8 bits for a key) or a keypad on an RS485 line sends keys, and the digits typed
// make a PIN. Everything is decoded and tried on a computer with simulated frames; no keypad was connected. Nothing here arms or disarms: the PIN goes to the panel, which has the
// lockout and the rest of the rules. Off unless a keypad is set up. Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

namespace armor::alarm {

// ---- Wiegand ----------------------------------------------------------------------------------------------------------

struct Wiegand26 {
  std::uint8_t facility = 0;
  std::uint16_t card = 0;
};

// 26 bits, most significant first in the low bits of `bits`: an even parity bit over the first twelve data bits, 8 bits of facility, 16 bits of card, an odd parity bit over the last twelve.
inline bool wiegand26_decode(std::uint32_t bits, Wiegand26& out) {
  if ((bits >> 26) != 0) return false;
  const auto ones = [](std::uint32_t value) { int n = 0; for (; value != 0; value >>= 1) n += static_cast<int>(value & 1u); return n; };
  const std::uint32_t first_half = (bits >> 13) & 0xFFFu;   // data bits 1 to 12
  const std::uint32_t last_half = (bits >> 1) & 0xFFFu;     // data bits 13 to 24
  const bool even_ok = ((ones(first_half) + static_cast<int>((bits >> 25) & 1u)) % 2) == 0;
  const bool odd_ok = ((ones(last_half) + static_cast<int>(bits & 1u)) % 2) == 1;
  if (!even_ok || !odd_ok) return false;
  out.facility = static_cast<std::uint8_t>((bits >> 17) & 0xFFu);
  out.card = static_cast<std::uint16_t>((bits >> 1) & 0xFFFFu);
  return true;
}

// The key of an 8-bit Wiegand keypad: the high nibble is the complement of the low one. 0 to 9 are digits, 10 is `*` (clear) and 11 is `#` (enter). Anything else is refused.
enum class Key { kNone, kDigit, kClear, kEnter };
struct KeyPress { Key key = Key::kNone; char digit = '0'; };

inline bool wiegand_key_decode(std::uint8_t byte, KeyPress& out) {
  const std::uint8_t low = byte & 0x0Fu;
  if (((~byte >> 4) & 0x0Fu) != low) return false;
  if (low <= 9) { out.key = Key::kDigit; out.digit = static_cast<char>('0' + low); return true; }
  if (low == 10) { out.key = Key::kClear; return true; }
  if (low == 11) { out.key = Key::kEnter; return true; }
  return false;
}

// ---- RS485 ------------------------------------------------------------------------------------------------------------

// A frame: 0x7E, the address of the keypad, the number of bytes of the payload (1 to 8), the payload, the XOR of address, length and payload, 0x7F. The first byte of the payload is the key
// as a character ('0' to '9', '*', '#'); the rest is ignored. Returns false for a frame that is not complete, not for this address or not right.
inline bool rs485_frame_decode(const std::uint8_t* bytes, std::size_t count, std::uint8_t address, KeyPress& out) {
  if (count < 6 || bytes[0] != 0x7E || bytes[count - 1] != 0x7F) return false;
  const std::size_t length = bytes[2];
  if (length < 1 || length > 8 || count != length + 5) return false;
  if (bytes[1] != address) return false;
  std::uint8_t check = static_cast<std::uint8_t>(bytes[1] ^ bytes[2]);
  for (std::size_t i = 0; i < length; ++i) check = static_cast<std::uint8_t>(check ^ bytes[3 + i]);
  if (check != bytes[3 + length]) return false;
  const char c = static_cast<char>(bytes[3]);
  if (c >= '0' && c <= '9') { out.key = Key::kDigit; out.digit = c; return true; }
  if (c == '*') { out.key = Key::kClear; return true; }
  if (c == '#') { out.key = Key::kEnter; return true; }
  return false;
}

// ---- the PIN being typed ----------------------------------------------------------------------------------------------

// The digits typed since the last pause make a PIN: `#` hands it over, `*` throws it away, a pause of `idle_ms` between two keys throws it away too, and more than `max_digits` digits start over
// (a keypad must not keep an endless string).
class PinEntry {
 public:
  explicit PinEntry(std::uint32_t idle_ms = 10000, std::size_t max_digits = 8) : idle_ms_(idle_ms), max_digits_(max_digits) {}

  // Returns true when `entered()` has a PIN to try.
  bool press(const KeyPress& key, std::uint64_t now_ms) {
    if (!digits_.empty() && now_ms - last_ms_ > idle_ms_) digits_.clear();
    last_ms_ = now_ms;
    ready_ = false;
    if (key.key == Key::kDigit) {
      if (digits_.size() >= max_digits_) digits_.clear();
      digits_.push_back(key.digit);
      return false;
    }
    if (key.key == Key::kClear) { digits_.clear(); return false; }
    if (key.key == Key::kEnter) {
      entered_ = digits_;
      digits_.clear();
      ready_ = !entered_.empty();
      return ready_;
    }
    return false;
  }

  const std::string& entered() const { return entered_; }
  std::size_t typed() const { return digits_.size(); }

 private:
  std::uint32_t idle_ms_;
  std::size_t max_digits_;
  std::string digits_, entered_;
  std::uint64_t last_ms_ = 0;
  bool ready_ = false;
};

}  // namespace armor::alarm
