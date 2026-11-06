// ARMOR-NODE-ALARM - tests/sim_bench.cpp: the driver of the simulation bench. The voltage of a wired loop goes through the real classifier and debouncer.
// Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
//
// It is built from the core files of the firmware, on a computer, and fed by ARMOR-SIMULATOR (python -m armor_simulator.node_bench) with the buses of the equipment, spoiled
// on purpose. Nothing here has run on a board.
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "../core/alarm_zone.hpp"

// The line protocol shared by the drivers of every node (see ARMOR-SIMULATOR docs/NODE_BENCH.md): one case per line on the standard input,
//   bus <TAB> id <TAB> arg <TAB> payload        (payload: hex of bytes or text; "-" when there is none)
// and one answer per case on the standard output: id <TAB> answer.
namespace simio {
inline std::vector<std::string> split(const std::string& text, char sep) {
  std::vector<std::string> out;
  std::string item;
  for (char c : text) { if (c == sep) { out.push_back(item); item.clear(); } else item += c; }
  out.push_back(item);
  return out;
}
inline std::vector<std::uint8_t> unhex(const std::string& hex) {
  std::vector<std::uint8_t> out;
  if (hex == "-") return out;
  for (std::size_t i = 0; i + 1 < hex.size(); i += 2) out.push_back(static_cast<std::uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16)));
  return out;
}
inline std::string unhex_text(const std::string& hex) { const auto bytes = unhex(hex); return std::string(bytes.begin(), bytes.end()); }
inline std::string fixed(double value, int digits) { char buffer[64]; std::snprintf(buffer, sizeof buffer, "%.*f", digits, value); return buffer; }
struct Case { std::string bus, id, arg, payload; };
inline bool next_case(Case& out) {
  std::string line;
  while (std::getline(std::cin, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    const auto f = split(line, '\t');
    if (f.size() != 4) continue;
    out = Case{f[0], f[1], f[2], f[3]};
    return true;
  }
  return false;
}
}  // namespace simio

static const char* const BUS = "alarm-loop";

// arg: the three limits in millivolts; payload: "time_ms:millivolts" samples separated by commas. The answer is the state after each sample.
static std::string answer(const simio::Case& c) {
  namespace az = armor::alarm;
  const auto limits_text = simio::split(c.arg, ',');
  if (limits_text.size() != 3) return "bad case";
  az::LoopLimits limits;
  limits.short_below_mv = std::stod(limits_text[0]);
  limits.normal_below_mv = std::stod(limits_text[1]);
  limits.triggered_below_mv = std::stod(limits_text[2]);
  if (!az::limits_are_valid(limits)) return "invalid limits";
  az::Debouncer debouncer(200);
  std::string text;
  bool first = true;
  for (const std::string& sample : simio::split(c.payload, ',')) {
    const auto f = simio::split(sample, ':');
    if (f.size() != 2) continue;
    const az::ZoneState state = debouncer.feed(az::classify_loop(std::stod(f[1]), limits), static_cast<std::uint64_t>(std::stoull(f[0])));
    text += std::string(first ? "" : ",") + az::state_text(state);
    first = false;
  }
  return text;
}

int main() {
  simio::Case c;
  while (simio::next_case(c)) std::cout << c.id << '\t' << (c.bus == BUS ? answer(c) : std::string("unsupported")) << '\n';
  return 0;
}
