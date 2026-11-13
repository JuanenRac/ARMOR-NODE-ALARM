// ARMOR-NODE-ALARM - host tests of the signature of the commands of the server: the vectors of ARMOR-COMMON (conformance/signatures/command.json, written here as they are), the
// refusals one by one, the memory of nonces, the freshness against a valid clock, and the runner that drops a command that does not verify. No hardware was used.
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

#include "../core/alarm_config.hpp"
#include "../core/alarm_runner.hpp"
#include "../core/command_sig.hpp"

static int failures = 0;
static int checks = 0;
#define CHECK(condition)                                                              \
  do {                                                                                \
    ++checks;                                                                         \
    if (!(condition)) {                                                               \
      ++failures;                                                                     \
      std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #condition);                \
    }                                                                                 \
  } while (0)

using namespace armor;

// The key of "alarm-1" and two signed commands of the vectors (an arm, and a disarm), and the arm signed with the key of another node.
static const char* kKey = "156a3ba893545ebfde6009ac5393395fcb033e467f7ee6fb1839548da19a66b3";
static const char* kOtherKey = "9d2bf6e333a39afd9ff219bdda4df1833697f37f9c0d7577be589d592f702652";   // the key of "electrical-1"
static const char* kArm =
    "{\"kind\":\"alarm_command\",\"node_id\":\"alarm-1\",\"timestamp_ms\":1000000,\"command_id\":\"c000000000000001\",\"action\":\"arm\",\"mode\":\"away\",\"force\":true,\"nonce\":\"0123456789abcdef\","
    "\"sig\":\"74f16f5431411bbecb71cc858aca6a59035ac0ce0f906b7a69882f692d2e493a\"}";
static const char* kDisarm =
    "{\"kind\":\"alarm_command\",\"node_id\":\"alarm-1\",\"timestamp_ms\":1000001,\"command_id\":\"c000000000000002\",\"action\":\"disarm\",\"nonce\":\"fedcba9876543210\","
    "\"sig\":\"63e7ee9a583f23f6c491fa4e8c216bf4fa8a589391076f8c15e6a0e89e9e0e30\"}";
static const char* kSignedByAnotherNode =
    "{\"kind\":\"alarm_command\",\"node_id\":\"alarm-1\",\"timestamp_ms\":1000000,\"command_id\":\"c000000000000001\",\"action\":\"arm\",\"mode\":\"away\",\"force\":true,\"nonce\":\"0123456789abcdef\","
    "\"sig\":\"19725b7b61df4559cac6f8224e7016dd4fdb27dfb26096a2d6cac6574353b094\"}";

static std::string replaced(std::string text, const std::string& from, const std::string& to) {
  const std::size_t at = text.find(from);
  if (at != std::string::npos) text.replace(at, from.size(), to);
  return text;
}

static void test_the_vectors_verify_and_the_pieces_are_found() {
  command_sig::NonceCache cache;
  CHECK(command_sig::verify(kArm, kKey, cache, 0) == command_sig::Verdict::kOk);
  std::string body;
  std::string_view sig;
  CHECK(command_sig::split(kDisarm, body, sig) && sig == "63e7ee9a583f23f6c491fa4e8c216bf4fa8a589391076f8c15e6a0e89e9e0e30" && body.back() == '}' && body.find("\"sig\"") == std::string::npos);
  CHECK(command_sig::signature_of(kKey, body) == std::string(sig));
  std::string nonce;
  CHECK(command_sig::nonce_of(body, nonce) && nonce == "fedcba9876543210");
  CHECK(command_sig::timestamp_of(body) == 1000001);
  CHECK(command_sig::timestamp_of("{\"kind\":\"x\"}") == -1);
  CHECK(command_sig::key_is_valid(kKey) && !command_sig::key_is_valid("abc") && !command_sig::key_is_valid(std::string(64, 'G')) && !command_sig::key_is_valid(std::string(64, 'A')));
}

static void test_every_refusal() {
  command_sig::NonceCache cache;
  const std::string good = kArm;
  CHECK(command_sig::verify(good, "", cache, 0) == command_sig::Verdict::kNoKey);
  CHECK(command_sig::verify(good, "short", cache, 0) == command_sig::Verdict::kNoKey);
  CHECK(command_sig::verify(replaced(good, "\"arm\"", "\"disarm\""), kKey, cache, 0) == command_sig::Verdict::kBadSignature);
  CHECK(command_sig::verify(replaced(good, "alarm-1", "alarm-2"), kKey, cache, 0) == command_sig::Verdict::kBadSignature);
  CHECK(command_sig::verify(replaced(good, "1000000", "1000001"), kKey, cache, 0) == command_sig::Verdict::kBadSignature);
  CHECK(command_sig::verify(replaced(good, "\"away\"", "\"stay\""), kKey, cache, 0) == command_sig::Verdict::kBadSignature);
  CHECK(command_sig::verify(replaced(good, "0123456789abcdef", "0123456789abcdee"), kKey, cache, 0) == command_sig::Verdict::kBadSignature);
  CHECK(command_sig::verify(kSignedByAnotherNode, kKey, cache, 0) == command_sig::Verdict::kBadSignature);
  CHECK(command_sig::verify(good, kOtherKey, cache, 0) == command_sig::Verdict::kBadSignature);
  CHECK(command_sig::verify(good + " ", kKey, cache, 0) == command_sig::Verdict::kNoSignature);
  CHECK(command_sig::verify(replaced(good, "74f16f54", "74F16F54"), kKey, cache, 0) == command_sig::Verdict::kNoSignature);   // capitals are not the format
  CHECK(command_sig::verify(replaced(good, "3a\"}", "3\"}"), kKey, cache, 0) == command_sig::Verdict::kNoSignature);   // a signature that is a digit short
  CHECK(command_sig::verify("{\"kind\":\"alarm_command\",\"node_id\":\"alarm-1\"}", kKey, cache, 0) == command_sig::Verdict::kNoSignature);
  CHECK(command_sig::verify("", kKey, cache, 0) == command_sig::Verdict::kNoSignature);
  CHECK(command_sig::verify("{\"sig\":\"" + std::string(64, 'a') + "\"}", kKey, cache, 0) == command_sig::Verdict::kNoSignature);   // the ending needs the comma before it
  // a signature that is good over a body whose nonce is missing: the server never makes one
  const std::string no_nonce_body = "{\"kind\":\"alarm_command\",\"node_id\":\"alarm-1\",\"timestamp_ms\":1000000,\"command_id\":\"c000000000000001\",\"action\":\"disarm\"}";
  const std::string no_nonce = no_nonce_body.substr(0, no_nonce_body.size() - 1) + ",\"sig\":\"" + command_sig::signature_of(kKey, no_nonce_body) + "\"}";
  CHECK(command_sig::verify(no_nonce, kKey, cache, 0) == command_sig::Verdict::kNoNonce);
  CHECK(command_sig::verify(good, kKey, cache, 0) == command_sig::Verdict::kOk);   // none of the refusals spent the nonce
}

static void test_a_nonce_is_used_once_and_the_memory_is_bounded() {
  command_sig::NonceCache cache;
  CHECK(command_sig::verify(kArm, kKey, cache, 0) == command_sig::Verdict::kOk);
  CHECK(command_sig::verify(kArm, kKey, cache, 0) == command_sig::Verdict::kReplay);
  CHECK(command_sig::verify(kDisarm, kKey, cache, 0) == command_sig::Verdict::kOk);
  CHECK(cache.seen("0123456789abcdef") && cache.seen("fedcba9876543210") && !cache.seen("0000000000000000"));
  // the ring keeps the last 32: after 32 others the first is forgotten (the freshness stops an old command then)
  for (unsigned i = 0; i < command_sig::kNonceMemory; ++i) cache.remember(std::string("00000000000000") + static_cast<char>('a' + (i / 16)) + static_cast<char>("0123456789abcdef"[i % 16]));
  CHECK(!cache.seen("0123456789abcdef") && !cache.seen("fedcba9876543210"));
  cache.clear();
  CHECK(!cache.seen("0123456789abcdef"));
}

static void test_the_freshness_is_checked_only_with_a_valid_clock() {
  // the timestamp of the disarm is 1000001 ms
  command_sig::NonceCache cache;
  CHECK(command_sig::verify(kDisarm, kKey, cache, 1000001 + 100000) == command_sig::Verdict::kOk);
  command_sig::NonceCache later;
  CHECK(command_sig::verify(kDisarm, kKey, later, 1000001 + 120001) == command_sig::Verdict::kStale);
  CHECK(!later.seen("fedcba9876543210"));   // a stale command does not spend its nonce
  command_sig::NonceCache early;
  CHECK(command_sig::verify(kDisarm, kKey, early, 1000001 - 120001 + 200000) == command_sig::Verdict::kOk);   // 200 s into the epoch: within two minutes
  command_sig::NonceCache none;
  CHECK(command_sig::verify(kDisarm, kKey, none, 0) == command_sig::Verdict::kOk);   // no clock: the nonces alone
  CHECK(std::string(command_sig::to_text(command_sig::Verdict::kReplay)) == "replay" && std::string(command_sig::to_text(command_sig::Verdict::kStale)) == "stale");
}

static config::Settings settings_with(const std::string& key) {
  config::Settings s = config::default_settings("a1b2c3");
  s.node_id = "alarm-1";
  s.server.commands = true;
  s.server.command_key = key;
  return s;
}

static void test_the_runner_takes_only_signed_commands_once_it_has_a_key() {
  // no key: a command of the contract is taken as it always was, signed or not (the fields nonce and sig are known to the parser)
  {
    alarm::Runner node(settings_with(""));
    alarm::Reply plain = node.handle("{\"kind\":\"alarm_command\",\"node_id\":\"alarm-1\",\"timestamp_ms\":1,\"command_id\":\"c000000000000009\",\"action\":\"disarm\"}", 0);
    CHECK(plain.understood && !plain.signature_refused);
    alarm::Reply signed_one = node.handle(kDisarm, 0);
    CHECK(signed_one.understood && !signed_one.signature_refused && signed_one.command.action == alarm::Action::kDisarm);
  }
  // with the key: an unsigned command and a forged one get no answer; the signed one is taken once, a replay is dropped
  {
    alarm::Runner node(settings_with(kKey));
    alarm::Reply plain = node.handle("{\"kind\":\"alarm_command\",\"node_id\":\"alarm-1\",\"timestamp_ms\":1,\"command_id\":\"c000000000000009\",\"action\":\"disarm\"}", 0);
    CHECK(!plain.understood && plain.signature_refused && plain.verdict == command_sig::Verdict::kNoSignature);
    alarm::Reply forged = node.handle(replaced(kDisarm, "1000001", "1000002"), 0);
    CHECK(!forged.understood && forged.signature_refused && forged.verdict == command_sig::Verdict::kBadSignature);
    alarm::Reply good = node.handle(kDisarm, 0);
    CHECK(good.understood && !good.signature_refused && good.command.command_id == "c000000000000002");
    alarm::Reply again = node.handle(kDisarm, 0);
    CHECK(!again.understood && again.signature_refused && again.verdict == command_sig::Verdict::kReplay);
    alarm::Reply stale = node.handle(kArm, 0, 1000000 + 500000);
    CHECK(!stale.understood && stale.signature_refused && stale.verdict == command_sig::Verdict::kStale);
  }
  // a node whose commands are off ignores everything, signed or not, and a command for another node is not taken even when it verifies with the key
  {
    config::Settings off = settings_with(kKey);
    off.server.commands = false;
    alarm::Runner node(off);
    CHECK(!node.handle(kDisarm, 0).understood);
    config::Settings other = settings_with(kKey);
    other.node_id = "alarm-2";
    alarm::Runner second(other);
    CHECK(!second.handle(kDisarm, 0).understood);
  }
}

// Whether the settings are refused because of the command key (the defaults of a node are refused for other reasons).
static bool key_refused(const config::Settings& s) {
  for (const config::Problem& problem : config::validate(s)) if (problem.path == "server.command_key") return true;
  return false;
}

static void test_the_key_is_a_secret_of_the_settings() {
  config::Settings s = settings_with(kKey);
  const std::string shown = config::to_json(s, false);
  CHECK(shown.find(kKey) == std::string::npos && shown.find("\"command_key_set\":true") != std::string::npos);
  CHECK(config::to_json(s, true).find(std::string("\"command_key\":\"") + kKey + "\"") != std::string::npos);
  CHECK(!key_refused(s));
  s.server.command_key = "abc";
  CHECK(key_refused(s));
  s.server.command_key = std::string(64, 'G');
  CHECK(key_refused(s));
  s.server.command_key = std::string(64, 'A');
  CHECK(key_refused(s));   // capitals are not the format
  s.server.command_key.clear();
  CHECK(!key_refused(s) && config::to_json(s, false).find("\"command_key_set\":false") != std::string::npos);
}

int main() {
  test_the_vectors_verify_and_the_pieces_are_found();
  test_every_refusal();
  test_a_nonce_is_used_once_and_the_memory_is_bounded();
  test_the_freshness_is_checked_only_with_a_valid_clock();
  test_the_runner_takes_only_signed_commands_once_it_has_a_key();
  test_the_key_is_a_secret_of_the_settings();
  std::printf("%d checks, %d failures\n", checks, failures);
  return failures == 0 ? 0 : 1;
}
