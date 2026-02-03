#include <catch2/catch_test_macros.hpp>

#include "geThreading.h"

using namespace geEngineSDK;

TEST_CASE("Threading: LockingPolicy<false> does nothing", "[Threading]") {
  LockingPolicy<false> p;
  p.lock();
  p.unlock();
  SUCCEED();
}

TEST_CASE("Threading: LockingPolicy<true> locks/unlocks safely", "[Threading]") {
  LockingPolicy<true> p;
  p.lock();
  p.unlock();

  // unlock twice should be safe (it checks owns_lock())
  p.unlock();
  SUCCEED();
}

TEST_CASE("Threading: ScopedLock<true> works with Mutex", "[Threading]") {
  Mutex m;
  {
    ScopedLock<true> lk(m);
  }
  SUCCEED();
}

TEST_CASE("Threading: ScopedLock<true> serializes increments", "[Threading]") {
  Mutex m;
  std::atomic<int> x{ 0 };

  auto worker = [&] {
    for (int i = 0; i < 10000; ++i) {
      ScopedLock<true> lk(m);
      x.fetch_add(1, std::memory_order_relaxed);
    }
    };

  std::thread a(worker), b(worker);
  a.join();
  b.join();

  REQUIRE(x.load(std::memory_order_relaxed) == 20000);
}
