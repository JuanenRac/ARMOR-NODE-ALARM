// ARMOR-NODE-ALARM - the backup battery of the panel, read by an analog pin through a resistor divider: the state of charge from the voltage, with a margin so it does not flicker.
// Tried on a computer with simulated readings; no battery was connected. It reports and never decides anything about the siren or the arming. Off unless a pin is set.
// Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#pragma once
#include <cstdint>

namespace armor::alarm {

struct BackupBatteryLimits {
  double divider = 5.7;        // the battery voltage divided by the voltage at the pin (a 4.7 k over 1 k divider gives 5.7)
  double empty_mv = 11000.0;   // the charge is 0 % at this voltage (a 12 V lead-acid battery)
  double full_mv = 12800.0;    // and 100 % at this one
  double low_mv = 11800.0;     // below this the battery is low
  double missing_mv = 3000.0;  // below this at the battery, nothing is connected
  double margin_mv = 150.0;    // a state is left only when the voltage is this far past its limit
};

enum class BatteryState { kUnknown, kMissing, kCritical, kLow, kGood };

struct BatteryReading {
  BatteryState state = BatteryState::kUnknown;
  double battery_mv = 0;
  int percent = 0;   // 0 to 100
};

class BackupBattery {
 public:
  explicit BackupBattery(const BackupBatteryLimits& limits = BackupBatteryLimits()) : limits_(limits) {}

  // The reading of the pin in millivolts.
  BatteryReading update(double pin_mv) {
    BatteryReading out;
    out.battery_mv = pin_mv * limits_.divider;
    const double span = limits_.full_mv - limits_.empty_mv;
    const double ratio = (out.battery_mv - limits_.empty_mv) / span;
    out.percent = ratio <= 0.0 ? 0 : ratio >= 1.0 ? 100 : static_cast<int>(ratio * 100.0 + 0.5);
    if (out.battery_mv < limits_.missing_mv) state_ = BatteryState::kMissing;
    else state_ = next(out.battery_mv);
    out.state = state_;
    return out;
  }

  BatteryState state() const { return state_; }

 private:
  // The state for a voltage, with the margin on the way up: a battery that was critical becomes low only above empty_mv + margin, and one that was low or critical becomes good only above low_mv + margin.
  BatteryState next(double mv) const {
    const bool was_critical = state_ == BatteryState::kCritical;
    const bool was_low = was_critical || state_ == BatteryState::kLow;
    if (mv < limits_.empty_mv || (was_critical && mv < limits_.empty_mv + limits_.margin_mv)) return BatteryState::kCritical;
    if (mv < limits_.low_mv || (was_low && mv < limits_.low_mv + limits_.margin_mv)) return BatteryState::kLow;
    return BatteryState::kGood;
  }

  BackupBatteryLimits limits_;
  BatteryState state_ = BatteryState::kUnknown;
};

}  // namespace armor::alarm
