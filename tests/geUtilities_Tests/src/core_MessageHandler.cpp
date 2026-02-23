#include <catch2/catch_test_macros.hpp>

#include "geMessageHandler.h"

using namespace geEngineSDK;

static String
uniqueMsg(const char* base) {
  static std::atomic<uint32_t> counter{ 0 };
  const uint32_t c = ++counter;
  return String(base) + "_" + toString(c);
}

static void
ensureMessageHandlerModuleStartedForTests() {
  if (!MessageHandler::isStarted()) {
    MessageHandler::startUp();
  }
}

TEST_CASE("MessageId: same name yields same identifier",
          "[MessageHandler][MessageId]") {
  const String name = uniqueMsg("TestMessage_Same");

  MessageId a(name);
  MessageId b(name);

  REQUIRE(a == b);
}

TEST_CASE("MessageId: different names yield different identifiers",
          "[MessageHandler][MessageId]") {
  MessageId a(uniqueMsg("TestMessage_A"));
  MessageId b(uniqueMsg("TestMessage_B"));

  REQUIRE(a != b);
}

TEST_CASE("MessageHandler: send triggers all listeners exactly once",
          "[MessageHandler]") {
  ensureMessageHandlerModuleStartedForTests();
  MessageId msg(uniqueMsg("TestMessage_SendAll"));

  std::atomic<int> hitsA{ 0 };
  std::atomic<int> hitsB{ 0 };
  std::atomic<int> hitsC{ 0 };

  std::function<void()> cbA = [&] { hitsA.fetch_add(1, std::memory_order_relaxed); };
  std::function<void()> cbB = [&] { hitsB.fetch_add(1, std::memory_order_relaxed); };
  std::function<void()> cbC = [&] { hitsC.fetch_add(1, std::memory_order_relaxed); };

  auto hA = MessageHandler::instance().listen(msg, cbA);
  auto hB = MessageHandler::instance().listen(msg, cbB);
  auto hC = MessageHandler::instance().listen(msg, cbC);

  MessageHandler::instance().send(msg);

  REQUIRE(hitsA.load(std::memory_order_relaxed) == 1);
  REQUIRE(hitsB.load(std::memory_order_relaxed) == 1);
  REQUIRE(hitsC.load(std::memory_order_relaxed) == 1);

  // cleanup
  hA.disconnect();
  hB.disconnect();
  hC.disconnect();
}

TEST_CASE("MessageHandler: listeners are per-message (no cross talk)",
          "[MessageHandler]") {
  ensureMessageHandlerModuleStartedForTests();
  MessageId msgA(uniqueMsg("TestMessage_A"));
  MessageId msgB(uniqueMsg("TestMessage_B"));

  std::atomic<int> hitsA{ 0 };
  std::atomic<int> hitsB{ 0 };

  std::function<void()> cbA = [&] { hitsA.fetch_add(1, std::memory_order_relaxed); };
  std::function<void()> cbB = [&] { hitsB.fetch_add(1, std::memory_order_relaxed); };

  auto hA = MessageHandler::instance().listen(msgA, cbA);
  auto hB = MessageHandler::instance().listen(msgB, cbB);

  MessageHandler::instance().send(msgA);

  REQUIRE(hitsA.load(std::memory_order_relaxed) == 1);
  REQUIRE(hitsB.load(std::memory_order_relaxed) == 0);

  MessageHandler::instance().send(msgB);

  REQUIRE(hitsA.load(std::memory_order_relaxed) == 1);
  REQUIRE(hitsB.load(std::memory_order_relaxed) == 1);

  hA.disconnect();
  hB.disconnect();
}

TEST_CASE("MessageHandler: disconnect prevents further callbacks",
          "[MessageHandler]") {
  ensureMessageHandlerModuleStartedForTests();
  MessageId msg(uniqueMsg("TestMessage_Disconnect"));

  std::atomic<int> hits{ 0 };
  std::function<void()> cb = [&] { hits.fetch_add(1, std::memory_order_relaxed); };

  auto h = MessageHandler::instance().listen(msg, cb);

  MessageHandler::instance().send(msg);
  REQUIRE(hits.load(std::memory_order_relaxed) == 1);

  h.disconnect();

  MessageHandler::instance().send(msg);
  REQUIRE(hits.load(std::memory_order_relaxed) == 1); // no change
}

TEST_CASE("MessageHandler: disconnect one of multiple listeners keeps others active",
          "[MessageHandler]") {
  ensureMessageHandlerModuleStartedForTests();
  MessageId msg(uniqueMsg("TestMessage_PartialDisconnect"));

  std::atomic<int> hitsA{ 0 };
  std::atomic<int> hitsB{ 0 };

  std::function<void()> cbA = [&] { hitsA.fetch_add(1, std::memory_order_relaxed); };
  std::function<void()> cbB = [&] { hitsB.fetch_add(1, std::memory_order_relaxed); };

  auto hA = MessageHandler::instance().listen(msg, cbA);
  auto hB = MessageHandler::instance().listen(msg, cbB);

  MessageHandler::instance().send(msg);
  REQUIRE(hitsA.load(std::memory_order_relaxed) == 1);
  REQUIRE(hitsB.load(std::memory_order_relaxed) == 1);

  hA.disconnect();

  MessageHandler::instance().send(msg);
  REQUIRE(hitsA.load(std::memory_order_relaxed) == 1); // unchanged
  REQUIRE(hitsB.load(std::memory_order_relaxed) == 2); // still active

  hB.disconnect();
}

TEST_CASE("sendMessage free function forwards to MessageHandler singleton",
          "[MessageHandler]") {
  ensureMessageHandlerModuleStartedForTests();
  MessageId msg(uniqueMsg("TestMessage_FreeFunction"));

  std::atomic<int> hits{ 0 };
  std::function<void()> cb = [&] { hits.fetch_add(1, std::memory_order_relaxed); };

  auto h = MessageHandler::instance().listen(msg, cb);

  sendMessage(msg);

  REQUIRE(hits.load(std::memory_order_relaxed) == 1);

  h.disconnect();
}

TEST_CASE("HMessage: double disconnect should not crash (documents current behavior)",
          "[MessageHandler]") {
  ensureMessageHandlerModuleStartedForTests();
  MessageId msg(uniqueMsg("TestMessage_DoubleDisconnect"));

  std::atomic<int> hits{ 0 };
  std::function<void()> cb = [&] { hits.fetch_add(1, std::memory_order_relaxed); };

  HMessage h = MessageHandler::instance().listen(msg, cb);

  MessageHandler::instance().send(msg);
  REQUIRE(hits.load(std::memory_order_relaxed) == 1);

  // First disconnect: removes listener
  h.disconnect();

  MessageHandler::instance().send(msg);
  REQUIRE(hits.load(std::memory_order_relaxed) == 1);

  //Second disconnect: should not crash.
  h.disconnect();

  MessageHandler::instance().send(msg);
  REQUIRE(hits.load(std::memory_order_relaxed) == 1);
}
