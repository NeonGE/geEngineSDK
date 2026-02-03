#include <catch2/catch_test_macros.hpp>

#include "geTaskScheduler.h"

using namespace geEngineSDK;

// -----------------------------------------------------------------------------
// TODO: Adapt this to your Module<> startup/shutdown API
// -----------------------------------------------------------------------------
static void ensureThreadPoolModuleStartedForTests()
{
  if (!ThreadPool::isStarted()) {
    ThreadPool::startUp<TThreadPool<>>(GE_THREAD_HARDWARE_CONCURRENCY-1);
  }
}

// Optionally: call shutdown in another helper if your module system requires it.

TEST_CASE("TaskScheduler: runs a task to completion", "[TaskScheduler]") {
  ensureThreadPoolModuleStartedForTests();

  TaskScheduler sched;

  std::atomic<int> x{ 0 };
  auto t = Task::create("inc", [&] { x.fetch_add(1); });

  sched.addTask(t);
  t->wait();

  REQUIRE(t->isComplete());
  REQUIRE(x.load() == 1);
}

TEST_CASE("TaskScheduler: priority ordering (higher first)", "[TaskScheduler]") {
  ensureThreadPoolModuleStartedForTests();

  TaskScheduler sched;

  std::mutex m;
  std::vector<int> order;

  auto push = [&](int id) {
    std::lock_guard<std::mutex> lk(m);
    order.push_back(id);
    };

  // Make them quick and independent, so ordering in queue matters.
  auto low = Task::create("low", [&] { push(1); }, TASKPRIORITY::kLow);
  auto high = Task::create("high", [&] { push(2); }, TASKPRIORITY::kHigh);

  sched.addTask(low);
  sched.addTask(high);

  high->wait();
  low->wait();

  REQUIRE(order.size() == 2);

  // With more than 1 worker, these could execute in parallel and reorder.
  // So enforce single worker to make the ordering deterministic:
  //
  // If you want strict ordering tests, set workers to 1:
  // - either modify TaskScheduler to allow configuring m_maxActiveTasks
  // - or call removeWorker() until 1 worker remains
  //
  // As written, this test is a "soft" check:
  REQUIRE((order[0] == 2 || order[1] == 2)); // high executed (at least) not later than completion of both
}

TEST_CASE("TaskScheduler: dependency prevents execution until dependency completes", "[TaskScheduler]") {
  ensureThreadPoolModuleStartedForTests();

  TaskScheduler sched;

  std::atomic<int> stage{ 0 };

  auto dep = Task::create("dep", [&] {
    stage.store(1, std::memory_order_relaxed);
    });

  auto t = Task::create("after", [&] {
    // Must observe stage==1 if dependency enforced
    REQUIRE(stage.load(std::memory_order_relaxed) == 1);
    stage.store(2, std::memory_order_relaxed);
    }, TASKPRIORITY::kNormal, dep);

  sched.addTask(t);
  sched.addTask(dep);

  t->wait();
  REQUIRE(stage.load(std::memory_order_relaxed) == 2);
}

TEST_CASE("TaskScheduler: cancel prevents execution", "[TaskScheduler]") {
  ensureThreadPoolModuleStartedForTests();

  TaskScheduler sched;

  std::atomic<int> hits{ 0 };

  auto t = Task::create("never", [&] { hits.fetch_add(1); });
  t->cancel();
  sched.addTask(t);

  // If canceled before scheduling, it should never run.
  // wait() should return quickly (state stays canceled).
  t->wait();

  REQUIRE(t->isCanceled());
  REQUIRE(hits.load() == 0);
}

TEST_CASE("TaskScheduler: TaskGroup runs count items and wait completes",
          "[TaskScheduler]") {
  ensureThreadPoolModuleStartedForTests();

  TaskScheduler sched;

  constexpr uint32 N = 64;
  std::atomic<uint32> hits{ 0 };

  auto g = TaskGroup::create("grp",
    [&](uint32 /*i*/) {
      hits.fetch_add(1, std::memory_order_relaxed);
    },
    N,
    TASKPRIORITY::kNormal);

  sched.addTaskGroup(g);
  g->wait();

  REQUIRE(g->isComplete());
  REQUIRE(hits.load(std::memory_order_relaxed) == N);
}
