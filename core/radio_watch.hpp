// ARMOR-NODE-ALARM - suspicion of a jammed radio: when most of the Zigbee sensors go quiet at the same time, a flat battery is not the likely cause, an inhibited radio is.
// Off unless it is turned on (the setting is off by default). The verdict needs the silence to last for a while (`hold_s`) so a short gap does not raise it, and it says nothing when
// there are too few sensors for "most of them" to mean anything. Pure logic with the clock handed in. Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace armor::alarm {

struct RadioWatchLimits {
  std::uint32_t silence_s = 600;      // a sensor not heard for this long counts as quiet
  std::uint32_t min_sensors = 3;      // fewer sensors than this and the verdict is "too few"
  std::uint32_t share_percent = 75;   // this share of them quiet at once is suspicious
  std::uint32_t hold_s = 120;         // and the suspicion must last this long
};

enum class RadioVerdict { kOff, kTooFew, kHealthy, kWatching, kSuspected };

class RadioWatch {
 public:
  RadioWatch(bool enabled, const RadioWatchLimits& limits) : enabled_(enabled), limits_(limits) {}

  // `heard_ms` holds, for each Zigbee sensor the node waits for, when it was last heard (0 = not since the node started listening, counted from `listening_since_ms`).
  RadioVerdict update(const std::vector<std::uint64_t>& heard_ms, std::uint64_t listening_since_ms, std::uint64_t now_ms) {
    if (!enabled_) return RadioVerdict::kOff;
    if (heard_ms.size() < limits_.min_sensors) { since_ms_ = 0; return RadioVerdict::kTooFew; }
    const std::uint64_t silence_ms = static_cast<std::uint64_t>(limits_.silence_s) * 1000ULL;
    std::size_t quiet = 0;
    for (const std::uint64_t heard : heard_ms) {
      const std::uint64_t last = heard > listening_since_ms ? heard : listening_since_ms;
      if (now_ms > last && now_ms - last > silence_ms) ++quiet;
    }
    if (quiet * 100 < heard_ms.size() * static_cast<std::size_t>(limits_.share_percent)) { since_ms_ = 0; return RadioVerdict::kHealthy; }
    if (since_ms_ == 0) since_ms_ = now_ms;
    return now_ms - since_ms_ >= static_cast<std::uint64_t>(limits_.hold_s) * 1000ULL ? RadioVerdict::kSuspected : RadioVerdict::kWatching;
  }

 private:
  bool enabled_ = false;
  RadioWatchLimits limits_;
  std::uint64_t since_ms_ = 0;
};

}  // namespace armor::alarm
