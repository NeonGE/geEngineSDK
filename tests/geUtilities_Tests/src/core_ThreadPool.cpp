#include <catch_amalgamated.hpp>

#include "geThreadPool.h"

using namespace geEngineSDK;

TEST_CASE("ThreadPool: run executes and HThread::blockUntilComplete waits", "[ThreadPool]") {
  // Use a local pool (doesn't rely on Module::instance())
  TThreadPool<> pool(/*capacity*/ 2, /*max*/ 4, /*idleTimeout*/ 60);

  std::atomic<int> x{ 0 };

  auto h = pool.run("t0", [&]
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
      x.fetch_add(1, std::memory_order_relaxed);
    });

  h.blockUntilComplete();
  REQUIRE(x.load(std::memory_order_relaxed) == 1);
}

TEST_CASE("ThreadPool: getNumActive and getNumAllocated behave", "[ThreadPool]") {
  TThreadPool<> pool(/*capacity*/ 1, /*max*/ 4, /*idleTimeout*/ 60);

  std::atomic<bool> go{ false };

  auto h = pool.run("hold", [&]
    {
      while (!go.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    });

  // Give it a moment to start
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  REQUIRE(pool.getNumAllocated() >= 1);
  REQUIRE(pool.getNumActive() >= 1);

  go.store(true, std::memory_order_relaxed);
  h.blockUntilComplete();

  REQUIRE(pool.getNumActive() == 0);
}

TEST_CASE("ThreadPool: reuses threads (allocated doesn't grow unnecessarily)",
          "[ThreadPool]") {
  TThreadPool<> pool(/*capacity*/ 1, /*max*/ 8, /*idleTimeout*/ 60);

  {
    auto h = pool.run("a", [] {});
    h.blockUntilComplete();
  }
  auto allocated1 = pool.getNumAllocated();

  {
    auto h = pool.run("b", [] {});
    h.blockUntilComplete();
  }
  auto allocated2 = pool.getNumAllocated();

  REQUIRE(allocated2 == allocated1); // should reuse idle thread
}

TEST_CASE("ThreadPool: maxCapacity reached throws", "[ThreadPool]") {
  //capacity 0 so it doesn't keep spares, max 1 so second concurrent run forces
  //allocation failure
  TThreadPool<> pool(0, 1, 60);

  std::atomic<bool> hold{ true };

  auto h0 = pool.run("t0", [&] {
    while (hold.load(std::memory_order_relaxed)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    });

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // Second task can't find idle and can't allocate new (max=1)
  REQUIRE_THROWS(pool.run("t1", [] {}));

  hold.store(false, std::memory_order_relaxed);
  h0.blockUntilComplete();
}

TEST_CASE("ThreadPool: clearUnused trims down to capacity (diagnostic)",
          "[ThreadPool][Diag]") {
  using namespace std::chrono;

  // Capacity 0: should destroy all idle threads when clearUnused is called
  TThreadPool<> pool(/*capacity*/ 0, /*max*/ 8, /*idleTimeout*/ 0);

  std::atomic<bool> hold{ true };
  std::atomic<bool> started{ false };

  // Ocupa 1 thread para forzar que el segundo sea creado
  auto h0 = pool.run("hold", [&] {
    started.store(true, std::memory_order_release);
    while (hold.load(std::memory_order_acquire)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    });

  while (!started.load(std::memory_order_acquire)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  auto h1 = pool.run("fast", [] {});

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  REQUIRE(pool.getNumAllocated() >= 2);

  hold.store(false, std::memory_order_release);
  h0.blockUntilComplete();
  h1.blockUntilComplete();

  //Idletime is counted in seconds so we need to wait that long
  std::this_thread::sleep_for(std::chrono::seconds(1));
  pool.clearUnused();

  REQUIRE(pool.getNumAllocated() == 0);
}

TEST_CASE("ThreadPool: clearUnused drops idle threads over default capacity", "[ThreadPool]") {
  using namespace std::chrono;

  TThreadPool<> pool(/*capacity*/ 1, /*max*/ 8, /*idleTimeout*/ 1);

  std::atomic<bool> hold{ true };
  std::atomic<bool> started{ false };

  auto h0 = pool.run("t0", [&]
    {
      started.store(true, std::memory_order_release);
      while (hold.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    });

  while (!started.load(std::memory_order_acquire)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  auto h1 = pool.run("t1", [] {});

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  REQUIRE(pool.getNumAllocated() >= 2);

  hold.store(false, std::memory_order_release);
  h0.blockUntilComplete();
  h1.blockUntilComplete();

  // Para time(nullptr) necesitas cruzar el umbral en SEGUNDOS
  std::this_thread::sleep_for(std::chrono::seconds(2));

  pool.clearUnused();

  // Si no hay activos, debe quedar en capacity=1
  REQUIRE(pool.getNumAllocated() == 1);
}
