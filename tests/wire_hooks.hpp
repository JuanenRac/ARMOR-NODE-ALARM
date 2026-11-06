// ARMOR-NODE-ALARM - the parsers this node has written by hand for the messages it receives, offered to tests/test_wire.cpp so that they are run against the oracle of ARMOR-COMMON.
// Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#pragma once
#include "../core/alarm_message.hpp"
#include "../core/alarm_policy.hpp"
#include "../core/arm_profile.hpp"

static std::vector<Hook> node_hooks() {
  return {
      {"alarm_command", "alarm::parse_command",
       [](const std::string& text) {
         armor::json::Value document;
         if (!armor::json::parse(text, document) || !document.is_object()) return false;
         armor::alarm::Command command;   // the node only reads a command that is for itself: the case's own name stands for this node's
         return armor::alarm::parse_command(text, document.string_or("node_id", ""), command);
       }},
      // `battery_low` may be left out of a zone message on purpose (a server that does not say it); everything else is the contract's
      {"alarm_zone_feed", "alarm::parse_zone_feed", [](const std::string& text) { armor::alarm::ZoneFeed feed; return armor::alarm::parse_zone_feed(text, feed); }, "battery_low"},
      {"alarm_policy", "alarm::parse_policy", [](const std::string& text) { armor::alarm::Policy policy; return armor::alarm::parse_policy(text, policy); }},
      {"arm_profile", "alarm::parse_profile_text", [](const std::string& text) { armor::alarm::ArmProfile profile; return armor::alarm::parse_profile_text(text, profile); }},
  };
}
