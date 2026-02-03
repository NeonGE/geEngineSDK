#include <catch2/catch_test_macros.hpp>

#include "geEvent.h"

using namespace geEngineSDK;

TEST_CASE("Event: empty() is true by default", "[Event]") {
  Event<void()> e;
  REQUIRE(e.empty());
}

TEST_CASE("Event: connect + trigger calls callback", "[Event]") {
  Event<void(int)> e;

  int sum = 0;
  auto h = e.connect([&](int v) { sum += v; });

  REQUIRE(!e.empty());

  e(3);
  e(4);

  REQUIRE(sum == 7);
  (void)h;
}

TEST_CASE("Event: disconnect via handle prevents future calls", "[Event]") {
  Event<void(int)> e;

  int sum = 0;
  auto h = e.connect([&](int v) { sum += v; });

  e(10);
  REQUIRE(sum == 10);

  h.disconnect();
  REQUIRE(sum == 10);

  e(10);
  REQUIRE(sum == 10);
}

TEST_CASE("Event: handle bool conversion works", "[Event]") {
  Event<void()> e;

  HEvent h0;
  REQUIRE(!h0);

  auto h1 = e.connect([] {});
  REQUIRE(h1);

  h1.disconnect();
  REQUIRE(!h1);
}

TEST_CASE("Event: clear() removes all callbacks", "[Event]") {
  Event<void(int)> e;

  int a = 0, b = 0;
  auto ha = e.connect([&](int v) { a += v; });
  auto hb = e.connect([&](int v) { b += v * 10; });

  e(1);
  REQUIRE(a == 1);
  REQUIRE(b == 10);

  e.clear();
  REQUIRE(e.empty());

  e(1);
  REQUIRE(a == 1);
  REQUIRE(b == 10);

  //Handles can be left alive; disconnect should be safe
  ha.disconnect();
  hb.disconnect();
}

TEST_CASE("Event: handle copy increments ref and both can disconnect safely", "[Event]") {
  Event<void()> e;

  int hits = 0;
  auto h1 = e.connect([&] { ++hits; });

  HEvent h2 = h1; // copy
  REQUIRE(h1);
  REQUIRE(h2);

  e();
  REQUIRE(hits == 1);

  h1.disconnect();
  REQUIRE(!h1);

  // h2 should now be detached as well because the connection is deactivated
  // (If your semantics are "independent handle refs", this may differ; but with your code,
  // disconnect deactivates the shared connection.)
  e();
  REQUIRE(hits == 1);

  h2.disconnect(); // should be safe no-op (or safe detach)
  REQUIRE(!h2);
}

TEST_CASE("Event: handle move transfers ownership", "[Event]") {
  Event<void()> e;

  int hits = 0;
  HEvent h1 = e.connect([&] { ++hits; });
  REQUIRE(h1);

  HEvent h2 = std::move(h1);
  REQUIRE(!h1);
  REQUIRE(h2);

  e();
  REQUIRE(hits == 1);

  h2.disconnect();
  REQUIRE(!h2);

  e();
  REQUIRE(hits == 1);
}

TEST_CASE("Event: callback can disconnect itself during trigger", "[Event]") {
  Event<void()> e;

  int hitsA = 0;
  int hitsB = 0;

  HEvent ha;
  ha = e.connect([&] {
    ++hitsA;
    ha.disconnect(); // disconnect inside callback
    });

  auto hb = e.connect([&] { ++hitsB; });

  e();
  REQUIRE(hitsA == 1);
  REQUIRE(hitsB == 1);

  e(); // A should be gone now, B remains
  REQUIRE(hitsA == 1);
  REQUIRE(hitsB == 2);

  hb.disconnect();
}

TEST_CASE("Event: connect during trigger is deferred and not called in same dispatch",
          "[Event]") {
  Event<void()> e;

  int hitsA = 0;
  int hitsB = 0;

  HEvent hb;

  auto ha = e.connect([&]
    {
      ++hitsA;
      if (!hb) {
        hb = e.connect([&] { ++hitsB; });
      }
    });

  e();
  REQUIRE(hitsA == 1);
  REQUIRE(hitsB == 0); // MUST be 0: new connection is deferred

  e();
  REQUIRE(hitsA == 2);
  REQUIRE(hitsB == 1); // now it runs

  ha.disconnect();
  hb.disconnect();
}

TEST_CASE("Event: basic thread safety smoke (many threads trigger)", "[Event][Threading]") {
  Event<void()> e;

  std::atomic<int> hits{ 0 };
  auto h = e.connect([&] { hits.fetch_add(1, std::memory_order_relaxed); });

  constexpr int kThreads = 8;
  constexpr int kIters = 2000;

  std::vector<std::thread> ts;
  ts.reserve(kThreads);

  for (int t = 0; t < kThreads; ++t) {
    ts.emplace_back([&] {
      for (int i = 0; i < kIters; ++i) {
        e();
      }
      });
  }

  for (auto& th : ts) th.join();

  REQUIRE(hits.load(std::memory_order_relaxed) == kThreads * kIters);

  h.disconnect();
}
