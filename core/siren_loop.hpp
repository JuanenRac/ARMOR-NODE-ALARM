// ARMOR-NODE-ALARM - the supervision of the siren's wiring: a return line read by the converter says whether the siren is still there, and the panel reports "siren_fault" when it is not.
// Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
//
// A siren that was cut, unplugged or shorted is the quietest way to defeat an alarm: the relay clicks and nothing sounds. The siren's wire goes through a small resistor network (an end-of-line
// resistor and a divider chosen by the installer) so that the voltage on the return pin tells the three states of the loop apart, the way the zones' loops do (alarm_zone.hpp):
//   below `short_below_mv`          the wire is shorted        -> "short"
//   up to `ok_below_mv`             the siren is there         -> "ok"
//   above it                        the wire is cut, the siren is gone (nothing pulls the pin) -> "open"
// The return is read while the siren is NOT sounding (a siren that draws current moves the voltage), unless the settings say it may be read anyway. A reading has to hold for `hold_ms` before
// the fault is reported, and the opposite one for `clear_ms` before it is lifted, so a gust of wind on the wire or the switching of the relay does not raise it. A return that cannot be read at
// all (the converter does not answer) is "unreadable": the siren is not supervised, which is a fault too, never an "ok".
// Nothing here touches hardware: the task in main/alarm_manager.cpp gives it the millivolts and the clock, and the tests give it a loop that is simulated. The hardware itself (the return input,
// the resistors) is not on any board yet: the limits are settings, not constants.
#pragma once
#include <cstdint>
#include <string>

namespace armor::alarm {

enum class SirenLoopState { kUnknown, kOk, kOpen, kShort, kUnreadable };

inline const char* siren_loop_text(SirenLoopState state) {
  switch (state) {
    case SirenLoopState::kOk: return "ok";
    case SirenLoopState::kOpen: return "open";
    case SirenLoopState::kShort: return "short";
    case SirenLoopState::kUnreadable: return "unreadable";
    case SirenLoopState::kUnknown: return "unknown";
  }
  return "unknown";
}
inline bool siren_loop_is_fault(SirenLoopState state) { return state == SirenLoopState::kOpen || state == SirenLoopState::kShort || state == SirenLoopState::kUnreadable; }

struct SirenLoopLimits {
  double short_below_mv = 300;
  double ok_below_mv = 2000;
};
inline bool siren_limits_are_valid(const SirenLoopLimits& l) { return l.short_below_mv > 0 && l.short_below_mv < l.ok_below_mv && l.ok_below_mv <= 3000; }

// What the return line says at one instant.
inline SirenLoopState classify_siren_loop(double millivolts, const SirenLoopLimits& limits) {
  if (millivolts < limits.short_below_mv) return SirenLoopState::kShort;
  if (millivolts < limits.ok_below_mv) return SirenLoopState::kOk;
  return SirenLoopState::kOpen;
}

struct SirenLoopTiming {
  std::uint32_t hold_ms = 3000;                // a bad reading must last this long before the fault is reported
  std::uint32_t clear_ms = 3000;               // and a good one this long before it is lifted
  bool read_while_sounding = false;            // false: nothing is judged while the siren sounds
};

// What the state message says of the loop (core/alarm_message.hpp): nothing at all when the loop is not supervised.
struct SirenLoopReport {
  bool supervised = false;
  SirenLoopState state = SirenLoopState::kUnknown;
  bool fault = false;
};

class SirenLoop {
 public:
  SirenLoop() = default;
  SirenLoop(bool enabled, const SirenLoopLimits& limits, const SirenLoopTiming& timing) : enabled_(enabled), limits_(limits), timing_(timing) {}

  bool enabled() const { return enabled_; }
  SirenLoopState state() const { return state_; }
  bool fault() const { return enabled_ && siren_loop_is_fault(state_); }
  std::uint32_t changes() const { return changes_; }

  // A reading of the return line, in millivolts, at `now_ms`. `siren_sounding`: the siren is on right now.
  void feed(double millivolts, bool siren_sounding, std::uint64_t now_ms) {
    if (!enabled_) return;
    unreadable_rounds_ = 0;
    if (siren_sounding && !timing_.read_while_sounding) { candidate_ = state_; candidate_since_ms_ = now_ms; return; }   // nothing is judged while it sounds
    consider(classify_siren_loop(millivolts, limits_), now_ms);
  }

  // The converter gave no reading this round. After kUnreadableRounds in a row the loop is unreadable (and with it the siren unsupervised).
  static constexpr std::uint8_t kUnreadableRounds = 5;
  void feed_unreadable(bool siren_sounding, std::uint64_t now_ms) {
    if (!enabled_) return;
    if (unreadable_rounds_ < kUnreadableRounds) ++unreadable_rounds_;
    if (unreadable_rounds_ < kUnreadableRounds) return;
    if (siren_sounding && !timing_.read_while_sounding) return;
    consider(SirenLoopState::kUnreadable, now_ms);
  }

  SirenLoopReport report() const { return SirenLoopReport{enabled_, enabled_ ? state_ : SirenLoopState::kUnknown, fault()}; }

 private:
  void consider(SirenLoopState reading, std::uint64_t now_ms) {
    if (reading != candidate_) { candidate_ = reading; candidate_since_ms_ = now_ms; }
    if (candidate_ == state_) return;
    const bool to_fault = siren_loop_is_fault(candidate_);
    const std::uint64_t needed = to_fault ? timing_.hold_ms : timing_.clear_ms;
    if (now_ms - candidate_since_ms_ >= needed) { state_ = candidate_; ++changes_; }
  }

  bool enabled_ = false;
  SirenLoopLimits limits_;
  SirenLoopTiming timing_;
  SirenLoopState state_ = SirenLoopState::kUnknown;
  SirenLoopState candidate_ = SirenLoopState::kUnknown;
  std::uint64_t candidate_since_ms_ = 0;
  std::uint8_t unreadable_rounds_ = 0;
  std::uint32_t changes_ = 0;
};

}  // namespace armor::alarm
