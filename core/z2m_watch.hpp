// ARMOR-NODE-ALARM - whether the server's Zigbee2MQTT is alive, from the state it publishes (zigbee2mqtt/bridge/state). Pure: tested on a computer.
// Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
//
// With a Zigbee coordinator on the network the node used to take it that the server had the radio whenever the node was connected to the broker. That is wrong in both directions: a broker that is up
// while Zigbee2MQTT is down left nobody with the radio. Zigbee2MQTT publishes a retained "online" / "offline" (the "offline" is its last will, so it also comes when it dies), and with this watch
// switched on (a topic in zigbee_local.bridge_state_topic) the node reads it. Off (the topic empty), nothing changes: the broker's link is the signal, as it always was.
//
// Unknown is not offline: until the first state arrives (and whenever the node has just reconnected to the broker, before the retained message is delivered) the node takes the server to have the radio,
// exactly as it did before, so the switch can only make the node take the radio over EARLIER, never later than before.
#pragma once
#include <string>
#include <string_view>

namespace armor::zigbee {

enum class BridgeState { kUnknown, kOnline, kOffline };

// "online" / "offline" as plain text, or a JSON object whose "state" is one of them (what the newer Zigbee2MQTT publishes). Anything else is unknown.
inline BridgeState parse_bridge_state(std::string_view payload) {
  const auto trim = [](std::string_view text) {
    while (!text.empty() && (text.front() == ' ' || text.front() == '\t' || text.front() == '\r' || text.front() == '\n')) text.remove_prefix(1);
    while (!text.empty() && (text.back() == ' ' || text.back() == '\t' || text.back() == '\r' || text.back() == '\n')) text.remove_suffix(1);
    return text;
  };
  std::string_view text = trim(payload);
  if (text == "online") return BridgeState::kOnline;
  if (text == "offline") return BridgeState::kOffline;
  if (text.size() < 2 || text.front() != '{' || text.back() != '}') return BridgeState::kUnknown;
  const std::string_view key = "\"state\"";
  const std::size_t at = text.find(key);
  if (at == std::string_view::npos || text.find(key, at + key.size()) != std::string_view::npos) return BridgeState::kUnknown;   // absent, or said twice
  std::string_view rest = trim(text.substr(at + key.size()));
  if (rest.empty() || rest.front() != ':') return BridgeState::kUnknown;
  rest = trim(rest.substr(1));
  if (rest.rfind("\"online\"", 0) == 0) return BridgeState::kOnline;
  if (rest.rfind("\"offline\"", 0) == 0) return BridgeState::kOffline;
  return BridgeState::kUnknown;
}

// A topic that can be subscribed to as it is: 1 to 128 characters of the usual set, levels separated by one slash, no wildcard.
inline bool valid_bridge_topic(std::string_view topic) {
  if (topic.empty() || topic.size() > 128 || topic.front() == '/' || topic.back() == '/') return false;
  char before = 0;
  for (const char c : topic) {
    const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '/' || c == '_' || c == '-' || c == '.';
    if (!ok || (c == '/' && before == '/')) return false;
    before = c;
  }
  return true;
}

class BridgeWatch {
 public:
  void set_state(BridgeState state) { state_ = state; }
  void forget() { state_ = BridgeState::kUnknown; }   // the node reconnected to the broker: what it knew is old
  BridgeState state() const { return state_; }

  // Does the server hold the radio? `enabled` is the setting, `broker_connected` the node's own link to the broker.
  bool server_has_radio(bool enabled, bool broker_connected) const {
    if (!broker_connected) return false;
    return !enabled || state_ != BridgeState::kOffline;
  }

 private:
  BridgeState state_ = BridgeState::kUnknown;
};

}  // namespace armor::zigbee
