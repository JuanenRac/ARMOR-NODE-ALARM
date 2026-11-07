// ARMOR-NODE-ALARM - tests/mqtt_shim.cpp: the real panel and message code of the node as a program for a computer, for the MQTT shim of ARMOR-SIMULATOR.
// Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
//
//   mqtt_shim <node-id>      standard input, one step per line, fields separated by a TAB:
//                              timestamp_ms <TAB> action <TAB> zone:kind:millivolts | zone:kind:millivolts ...
//                              action: - (nothing), arm_away, arm_stay, disarm (with the PIN of the shim), restart (a power cut: the panel is rebuilt from what the node saves)
//                            standard output, one message per line:  topic <TAB> payload
//
// It is built from the core files of the firmware: the voltage of every wired loop goes through the real classifier, the real panel (delays, zones, siren, restore after a
// restart) reacts and the real writer of the alarm message tells it. It does not talk to any broker: the simulator publishes what it prints, so the server receives exactly
// the bytes the node would have sent. The debouncing of the ADC samples and the radio are not in it. Nothing here has run on a board.
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../core/alarm_message.hpp"
#include "../core/alarm_panel.hpp"
#include "../core/alarm_zone.hpp"
#include "../core/mqtt_topic.hpp"

namespace {
std::vector<std::string> split(const std::string& text, char sep) {
  std::vector<std::string> out;
  std::string item;
  for (char c : text) { if (c == sep) { out.push_back(item); item.clear(); } else item += c; }
  out.push_back(item);
  return out;
}
const char* const kPin = "1234";
}  // namespace

int main(int argc, char** argv) {
  namespace al = armor::alarm;
  const std::string node = argc > 1 ? argv[1] : "";
  const std::string health_topic = armor::mqtt::will_topic(node);
  if (!health_topic.empty()) std::cout << health_topic << '\t' << armor::mqtt::birth_payload(node) << '\n';
  const std::string topic = al::topic(node, "state");
  al::Timing timing;
  timing.exit_delay_s = 1;
  timing.entry_delay_s = 1;
  timing.siren_max_s = 30;
  std::vector<al::Zone> zones;
  std::unique_ptr<al::Panel> panel;
  std::vector<al::ZoneReport> reports;
  const al::LoopLimits limits;
  std::string line;
  while (std::getline(std::cin, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    const std::vector<std::string> f = split(line, '\t');
    if (f.size() < 3 || topic.empty()) continue;
    const std::uint64_t now_ms = std::stoull(f[0]);
    std::vector<std::pair<std::string, double>> volts;
    for (const std::string& item : split(f[2], '|')) {
      const std::vector<std::string> p = split(item, ':');
      if (p.size() != 3) continue;
      al::Zone zone;
      zone.id = p[0];
      if (!al::valid_zone_id(zone.id) || !al::parse_kind(p[1], zone.kind)) continue;
      volts.emplace_back(zone.id, std::stod(p[2]));
      bool known = false;
      for (const al::Zone& z : zones) if (z.id == zone.id) known = true;
      if (!known) zones.push_back(zone);
    }
    if (!panel) panel = std::make_unique<al::Panel>(timing, zones, kPin);
    if (f[1] == "restart") {   // a power cut: what the node keeps is taken back by a panel that starts from nothing
      const al::Panel::Saved kept = panel->saved();
      panel = std::make_unique<al::Panel>(timing, zones, kPin);
      panel->restore(kept, now_ms);
    }
    for (const auto& v : volts) panel->set_zone(v.first, al::classify_loop(v.second, limits), now_ms);
    if (f[1] == "arm_away") panel->arm(al::Mode::kAway, false, now_ms);
    else if (f[1] == "arm_stay") panel->arm(al::Mode::kStay, false, now_ms);
    else if (f[1] == "disarm") panel->disarm(kPin, now_ms);
    panel->tick(now_ms);
    reports.clear();
    for (const al::Zone& z : zones) {
      al::ZoneReport r;
      r.id = z.id; r.name = z.name; r.kind = z.kind; r.state = panel->zone_state(z.id); r.bypassed = panel->bypassed(z.id);
      reports.push_back(r);
    }
    std::cout << topic << '\t' << al::state_json(node, now_ms, now_ms, *panel, panel->locked_out(now_ms), false, reports) << '\n';
  }
  return 0;
}
