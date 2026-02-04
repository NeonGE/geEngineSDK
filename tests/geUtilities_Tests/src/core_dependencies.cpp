#include <catch2/catch_test_macros.hpp>
#include <rttr/registration>
#include <iostream>

using namespace rttr;

enum class MetaData_Type
{
  SCRIPTABLE,
  GUI
};

struct MyStruct
{
  MyStruct()
  {};
  
  void func(double)
  {};
  
  int data;
};

RTTR_REGISTRATION
{
  registration::class_<MyStruct>("MyStruct")
    .constructor<>()
    .property("data", &MyStruct::data)
      (
        metadata("Description", "This is the data")
      )
    .method("func", &MyStruct::func)
      (
        metadata(MetaData_Type::SCRIPTABLE, true)
      );
}

TEST_CASE("RTTR Dependency Testing", "[CORE][RTTR]")
{
  type t = type::get<MyStruct>();
  for (auto& prop : t.get_properties())
    std::cout << "name: " << prop.get_name() << std::endl;

  for (auto& meth : t.get_methods())
    std::cout << "name: " << meth.get_name() << std::endl;
}
