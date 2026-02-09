#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

#include "geSmartEnum.h"

using namespace geEngineSDK;

GE_SMART_ENUM(TestEnum,
  A = 5,
  B,
  C = 10,
  D
)

GE_SMART_ENUM_CLASS(TestEnumClass,
  Zero = 0,
  One,
  Ten = 10,
  Eleven
)

TEST_CASE("SmartEnum: makeEnumNameMap handles explicit values + auto increment + trims spaces", "[SmartEnum]") {
  auto map = makeEnumNameMap("A = 5, B, C=10,   D");

  REQUIRE(map.at(5) == "A");
  REQUIRE(map.at(6) == "B");
  REQUIRE(map.at(10) == "C");
  REQUIRE(map.at(11) == "D");

  REQUIRE(map.size() == 4);
}

TEST_CASE("SmartEnum: makeEnumValuesMap handles explicit values + auto increment + trims spaces", "[SmartEnum]") {
  auto map = makeEnumValuesMap("A = 5, B, C=10,   D");

  REQUIRE(map.at("A") == 5);
  REQUIRE(map.at("B") == 6);
  REQUIRE(map.at("C") == 10);
  REQUIRE(map.at("D") == 11);

  REQUIRE(map.size() == 4);
}

TEST_CASE("SmartEnum: makeEnumList preserves explicit values + auto increment", "[SmartEnum]") {
  auto list = makeEnumList<int32>("A = 5, B, C=10, D");

  REQUIRE(list.size() == 4);
  REQUIRE(list[0] == 5);
  REQUIRE(list[1] == 6);
  REQUIRE(list[2] == 10);
  REQUIRE(list[3] == 11);
}

TEST_CASE("GE_SMART_ENUM: generated maps/lists contain expected values", "[SmartEnum]") {
  // LIST
  REQUIRE(TestEnum_LIST.size() == 4);
  REQUIRE(static_cast<int32>(TestEnum_LIST[0]) == 5);
  REQUIRE(static_cast<int32>(TestEnum_LIST[1]) == 6);
  REQUIRE(static_cast<int32>(TestEnum_LIST[2]) == 10);
  REQUIRE(static_cast<int32>(TestEnum_LIST[3]) == 11);

  // int->name
  REQUIRE(TestEnum_ENUM_NAMES.at(5) == "A");
  REQUIRE(TestEnum_ENUM_NAMES.at(6) == "B");
  REQUIRE(TestEnum_ENUM_NAMES.at(10) == "C");
  REQUIRE(TestEnum_ENUM_NAMES.at(11) == "D");

  // name->int
  REQUIRE(TestEnum_ENUM_VALUES.at("A") == 5);
  REQUIRE(TestEnum_ENUM_VALUES.at("B") == 6);
  REQUIRE(TestEnum_ENUM_VALUES.at("C") == 10);
  REQUIRE(TestEnum_ENUM_VALUES.at("D") == 11);
}

TEST_CASE("GE_SMART_ENUM: Type_toString returns exact token names (trimmed)", "[SmartEnum]") {
  REQUIRE(TestEnum_toString(TestEnum::A) == "A");
  REQUIRE(TestEnum_toString(TestEnum::B) == "B");
  REQUIRE(TestEnum_toString(TestEnum::C) == "C");
  REQUIRE(TestEnum_toString(TestEnum::D) == "D");
}

TEST_CASE("GE_SMART_ENUM: Type_toEnum parses names into values", "[SmartEnum]") {
  REQUIRE(static_cast<int32>(TestEnum_toEnum("A")) == 5);
  REQUIRE(static_cast<int32>(TestEnum_toEnum("B")) == 6);
  REQUIRE(static_cast<int32>(TestEnum_toEnum("C")) == 10);
  REQUIRE(static_cast<int32>(TestEnum_toEnum("D")) == 11);
}

TEST_CASE("GE_SMART_ENUM: invalid name/value throws (std::out_of_range via map::at)", "[SmartEnum]") {
  REQUIRE_THROWS_AS(TestEnum_toEnum("Nope"), std::out_of_range);
  REQUIRE_THROWS_AS(TestEnum_ENUM_NAMES.at(999), std::out_of_range);
}

TEST_CASE("GE_SMART_ENUM_CLASS: generated helpers work for enum class", "[SmartEnum]") {
  REQUIRE(TestEnumClass_toString(TestEnumClass::Zero) == "Zero");
  REQUIRE(TestEnumClass_toString(TestEnumClass::One) == "One");
  REQUIRE(TestEnumClass_toString(TestEnumClass::Ten) == "Ten");
  REQUIRE(TestEnumClass_toString(TestEnumClass::Eleven) == "Eleven");

  REQUIRE(static_cast<int32>(TestEnumClass_toEnum("Zero")) == 0);
  REQUIRE(static_cast<int32>(TestEnumClass_toEnum("One")) == 1);
  REQUIRE(static_cast<int32>(TestEnumClass_toEnum("Ten")) == 10);
  REQUIRE(static_cast<int32>(TestEnumClass_toEnum("Eleven")) == 11);

  REQUIRE_THROWS_AS(TestEnumClass_toEnum("Nope"), std::out_of_range);
}
