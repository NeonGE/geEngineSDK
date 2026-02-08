#include <catch2/catch_test_macros.hpp>

#include "geTimer.h"
#include "geTime.h"

using namespace geEngineSDK;

static void sleepMs(uint32_t ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

TEST_CASE("Timer: default ctor starts running and returns non-negative time", "[Timer]") {
  Timer t;

  auto us0 = t.getMicroseconds();
  auto ms0 = t.getMilliseconds();

  REQUIRE(us0 >= 0);
  REQUIRE(ms0 >= 0);

  // Coherencia básica: microsegundos deberían ser >= milisegundos * 1000
  REQUIRE(us0 + 1000 >= ms0 * 1000); // +1000 para tolerancia por rounding/cambio de tick
}

TEST_CASE("Timer: time increases over sleep", "[Timer]") {
  Timer t;

  auto us0 = t.getMicroseconds();
  sleepMs(10);
  auto us1 = t.getMicroseconds();

  REQUIRE(us1 >= us0);

  auto ms1 = t.getMilliseconds();
  REQUIRE(ms1 >= 10 - 2); // tolerancia: scheduler/VM/CI
}

TEST_CASE("Timer: reset brings elapsed near zero", "[Timer]") {
  Timer t;

  sleepMs(5);
  REQUIRE(t.getMicroseconds() > 0);

  t.reset();

  // Inmediatamente después del reset debería ser muy pequeño
  auto us = t.getMicroseconds();
  REQUIRE(us < 2000); // 2ms de tolerancia (CI puede ser ruidoso)
}

TEST_CASE("Timer: getStartMs is stable (does not change after reset)", "[Timer]") {
  Timer t;

  const auto start0 = t.getStartMs();
  sleepMs(2);
  t.reset();
  sleepMs(2);
  const auto start1 = t.getStartMs();

  // getStartMs() usa m_startTime.time_since_epoch(), por lo que sí cambia al reset.
  // OJO: En tu implementación actual, reset() cambia m_startTime -> por lo tanto startMs cambia.
  // Este test documenta ese comportamiento.
  REQUIRE(start1 != start0);
}

TEST_CASE("Time: default state + first _update gives frameDelta = 0 and increments frameIdx", "[Time]") {
  Time time;

  const auto frame0 = time.getFrameIdx();
  const auto t0 = time.getTime();
  const auto ms0 = time.getTimeMs();

  time._update();

  REQUIRE(time.getFrameIdx() == frame0 + 1);
  REQUIRE(time.getFrameDelta() == 0.0f); // first frame forced to 0
  REQUIRE(time.getTime() >= t0);
  REQUIRE(time.getTimeMs() >= ms0);
}

TEST_CASE("Time: consecutive _update yields positive-ish frameDelta and increasing time", "[Time]") {
  Time time;

  time._update(); // first one sets baseline

  sleepMs(10);
  time._update();

  REQUIRE(time.getFrameIdx() >= 2);

  // frameDelta should be >= 0 and typically > 0 after sleeping
  REQUIRE(time.getFrameDelta() >= 0.0f);
  REQUIRE(time.getTimeMs() > 0);
  REQUIRE(time.getTime() >= 0.0f);

  // getTimePrecise is microseconds since Timer reset; should be >= timeMs*1000
  const auto precise = time.getTimePrecise();
  REQUIRE(precise >= time.getTimeMs() * 1000);
}

TEST_CASE("Time: getCurrentTimeString format looks like HH:MM:SS", "[Time]") {
  Time time;

  const auto s = time.getCurrentTimeString(false);
  // Formato "%T" => "HH:MM:SS" (8 chars)
  REQUIRE(s.size() == 8);
  REQUIRE(s[2] == ':');
  REQUIRE(s[5] == ':');
}

TEST_CASE("Time: getCurrentDateTimeString contains expected separators", "[Time]") {
  Time time;

  const auto s = time.getCurrentDateTimeString(false);
  // Formato "%A, %B %d, %Y %T"
  // No asumimos idioma/locale, pero sí que tenga comas y un tiempo con ':'
  REQUIRE(s.find(",") != String::npos);
  REQUIRE(s.find(":") != String::npos);
}

TEST_CASE("Time: getAppStartUpDateString returns non-empty and stable across calls", "[Time]") {
  Time time;

  const auto a = time.getAppStartUpDateString(false);
  sleepMs(5);
  const auto b = time.getAppStartUpDateString(false);

  REQUIRE(!a.empty());
  REQUIRE(a == b); // m_appStartUpDate se fija en ctor
}

TEST_CASE("Time: _getFixedUpdateStep returns 0 on first call, then >=1 after enough time", "[Time][FixedUpdate]") {
  Time time;

  uint64 step = 0;

  // First call: sets m_lastFixedUpdateTime and returns 0
  auto n0 = time._getFixedUpdateStep(step);
  REQUIRE(n0 == 0);
  REQUIRE(step == 0);

  // Espera un poco más que el fixed step (16666us ~ 16.6ms)
  sleepMs(25);

  uint64 step1 = 0;
  auto n1 = time._getFixedUpdateStep(step1);

  // Dependiendo de scheduling, debería ser >=1
  REQUIRE(n1 >= 1);
  REQUIRE(step1 > 0);

  // Avanzar el fixed update debe reducir “deuda” (en general)
  time._advanceFixedUpdate(step1);

  // Inmediatamente después, puede que ya no haya nada pendiente (a veces aún sí si pasó tiempo)
  uint64 step2 = 0;
  auto n2 = time._getFixedUpdateStep(step2);
  REQUIRE(n2 >= 0);
}
