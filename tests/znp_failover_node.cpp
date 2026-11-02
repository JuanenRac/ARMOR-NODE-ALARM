// ARMOR-NODE-ALARM - the node's side of the failover, run against a coordinator that lives in another process (tools/znp_coordinator_sim.py, ZNP over TCP).
// The node starts with the server away; after a short wait it takes the radio over (RadioOwner), brings the coordinator up, learns the sensor and the siren, arms, and when the sensor
// opens the panel alarms and tells the siren to sound; then the server "comes back" and the node lets go. What happened is printed as one word per line for the check to read.
// Usage: znp_failover_node <host> <port> <sensor ieee> <siren ieee>. POSIX sockets: built where there are any. Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

#include "../core/alarm_config.hpp"
#include "../core/alarm_runner.hpp"
#include "../core/local_zigbee.hpp"
#include "../core/radio_owner.hpp"

using namespace armor;

static void say(const char* what, const std::string& more = "") { std::printf("%s%s%s\n", what, more.empty() ? "" : " ", more.c_str()); std::fflush(stdout); }

int main(int argc, char** argv) {
  if (argc != 5) { std::fprintf(stderr, "usage: %s host port sensor_ieee siren_ieee\n", argv[0]); return 2; }
  const std::string host = argv[1];
  const int port = std::atoi(argv[2]);
  std::uint64_t sensor_ieee = 0, siren_ieee = 0;
  if (!zigbee::parse_ieee(argv[3], sensor_ieee) || !zigbee::parse_ieee(argv[4], siren_ieee)) { std::fprintf(stderr, "bad address\n"); return 2; }

  config::Settings s = config::default_settings("a1b2c3");
  s.sta.enabled = true; s.sta.ssid = "casa"; s.sta.password = "una-clave-larga";
  // the settings refuse a loopback address; the program connects to the one it was given
  s.zigbee.network_host = "192.168.0.201"; s.zigbee.network_port = static_cast<std::uint16_t>(port);
  s.zigbee_local.after_s = 15;                         // the setting's own floor; the owner below is given a shorter wait for the test
  s.zigbee_local.sirens[0] = argv[4];
  s.zones[0].enabled = true; s.zones[0].id = "front-door"; s.zones[0].kind = alarm::ZoneKind::kInstant; s.zones[0].zigbee = argv[3]; s.zones[0].pin = -1;
  s.panel.pin = "1234"; s.panel.timing.exit_delay_s = 1; s.panel.timing.entry_delay_s = 5; s.panel.timing.siren_max_s = 60;
  const config::Problems problems = config::validate(s);
  if (!problems.empty()) { std::fprintf(stderr, "settings: %s %s\n", problems[0].path.c_str(), problems[0].code.c_str()); return 2; }

  using Clock = std::chrono::steady_clock;
  const auto t0 = Clock::now();
  const auto now_ms = [&]() { return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - t0).count()); };

  alarm::Runner node(s);
  zigbee::RadioOwner owner(1000, 0);                    // the server has been away for a second: the node takes the radio
  zigbee::LocalZigbee z({{"front-door", sensor_ieee}}, {{siren_ieee}});
  int fd = -1;
  bool began = false, armed_asked = false, siren_told = false, released = false;
  std::uint64_t alarm_at = 0;

  for (;;) {
    const std::uint64_t now = now_ms();
    if (now > 40'000) { say("TIMEOUT"); break; }
    const bool server_back = alarm_at != 0 && now - alarm_at > 1500;
    const bool mine = owner.update(server_back, now) == zigbee::RadioOwner::Owner::kNode;
    if (mine && !began) {
      fd = ::socket(AF_INET, SOCK_STREAM, 0);
      sockaddr_in addr{};
      addr.sin_family = AF_INET; addr.sin_port = htons(static_cast<std::uint16_t>(port));
      ::inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
      if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof addr) != 0) { say("NOCONNECT"); return 1; }
      ::fcntl(fd, F_SETFL, O_NONBLOCK);
      z.begin(now);
      began = true;
      say("TAKEOVER");
    }
    if (!mine && began && !released) {                  // the server is back: let go of the radio at once
      z.end();
      ::close(fd); fd = -1;
      released = true;
      say("RELEASED");
      break;
    }
    if (began) {
      std::uint8_t buffer[512];
      const ssize_t got = ::recv(fd, buffer, sizeof buffer, 0);
      if (got > 0) z.feed(buffer, static_cast<std::size_t>(got), now);
      z.tick(now);
      const std::vector<std::uint8_t> out = z.take_output();
      if (!out.empty()) ::send(fd, out.data(), out.size(), 0);
      node.radio_held(z.ready(), now);
      static bool ready_said = false;
      if (z.ready() && !ready_said) { ready_said = true; say("READY"); }
      static std::size_t known_said = 0;
      if (z.known_sensors() > known_said) { known_said = z.known_sensors(); say("KNOWN", std::to_string(known_said)); }
      for (const zigbee::ZoneUpdate& u : z.take_updates()) {
        node.feed_zigbee(u.zone_id, u.state, u.battery_low, now);
        say("UPDATE", u.zone_id + (u.state == alarm::ZoneState::kTriggered ? " triggered" : u.state == alarm::ZoneState::kTamper ? " tamper" : " normal"));
        if (u.state == alarm::ZoneState::kNormal && !armed_asked) {
          armed_asked = true;
          say("ARM", node.arm(alarm::Mode::kAway, false, now) == alarm::Refusal::kNone ? "ok" : "refused");
        }
      }
      node.tick(now);
      if (z.ready() && node.siren() != siren_told) {
        siren_told = node.siren();
        say("SIREN", std::string(siren_told ? "on " : "off ") + std::to_string(z.siren(siren_told, s.panel.timing.siren_max_s)));
        if (siren_told && alarm_at == 0) alarm_at = now;
      }
      if (node.panel().phase() == alarm::Phase::kAlarm) { static bool said = false; if (!said) { said = true; say("ALARM"); } }
    } else {
      node.tick(now);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  if (fd >= 0) ::close(fd);
  return released ? 0 : 1;
}
