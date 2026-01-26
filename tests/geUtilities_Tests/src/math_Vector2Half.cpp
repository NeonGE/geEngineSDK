#include <catch_amalgamated.hpp>

#include "geVector2Half.h"

using namespace geEngineSDK;

namespace {
  inline void requireNear(float a, float b, float eps = 1e-3f) {
    REQUIRE(std::fabs(a - b) <= eps);
  }
}

TEST_CASE("Vector2Half: conversion roundtrip", "[Math][Vector2Half]") {
  Vector2 v(1.25f, -2.5f);

  //ctor from Vector2
  Vector2Half h(v);

  Vector2 back = (Vector2)h;
  requireNear(back.x, v.x);
  requireNear(back.y, v.y);
}
