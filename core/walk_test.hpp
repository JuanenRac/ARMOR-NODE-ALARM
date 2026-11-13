// ARMOR-NODE-ALARM - the walk test: for a few minutes the installer walks through the house and every sensor that sees the person is written down, with no alarm and no siren.
// Off unless the node is told it is allowed (the setting is off by default), and it ends by itself: a walk test that someone forgets must not leave the house undefended.
// Pure logic with the clock handed in, so it is tested on a computer. Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

namespace armor::alarm {

class WalkTest {
 public:
  static constexpr std::uint32_t kMinSeconds = 60, kMaxSeconds = 1800, kDefaultSeconds = 300;
  static constexpr std::size_t kMaxZones = 8;

  explicit WalkTest(bool allowed) : allowed_(allowed) {}

  // Starts a walk of `seconds` (kept between a minute and half an hour). False when it is not allowed or one is already running.
  bool start(std::uint64_t now_ms, std::uint32_t seconds = kDefaultSeconds) {
    if (!allowed_ || running(now_ms)) return false;
    const std::uint32_t kept = seconds < kMinSeconds ? kMinSeconds : seconds > kMaxSeconds ? kMaxSeconds : seconds;
    until_ms_ = now_ms + static_cast<std::uint64_t>(kept) * 1000ULL;
    active_ = true;
    seen_ = 0;
    return true;
  }

  void stop() { active_ = false; }

  // True while the walk is on; the clock ends it.
  bool running(std::uint64_t now_ms) const { return active_ && now_ms < until_ms_; }

  // A zone (1 to 8) saw something. Returns true when it was written down for the first time in this walk.
  bool seen(std::size_t zone, std::uint64_t now_ms) {
    if (!running(now_ms) || zone < 1 || zone > kMaxZones) return false;
    const std::uint32_t bit = 1u << (zone - 1);
    if ((seen_ & bit) != 0) return false;
    seen_ |= bit;
    return true;
  }

  std::uint32_t seen_mask() const { return seen_; }

  // The state for the panel: {"running":true,"seen":[1,3],"missing":[2]}; `enabled_mask` has a bit for each zone that exists.
  std::string report(std::uint64_t now_ms, std::uint32_t enabled_mask) const {
    std::string seen_list, missing_list;
    for (std::size_t zone = 1; zone <= kMaxZones; ++zone) {
      const std::uint32_t bit = 1u << (zone - 1);
      if ((enabled_mask & bit) == 0) continue;
      std::string& list = (seen_ & bit) != 0 ? seen_list : missing_list;
      if (!list.empty()) list += ',';
      list += std::to_string(zone);
    }
    return std::string("{\"running\":") + (running(now_ms) ? "true" : "false") + ",\"seen\":[" + seen_list + "],\"missing\":[" + missing_list + "]}";
  }

 private:
  bool allowed_ = false;
  bool active_ = false;
  std::uint64_t until_ms_ = 0;
  std::uint32_t seen_ = 0;
};

}  // namespace armor::alarm
