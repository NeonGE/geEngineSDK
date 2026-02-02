#include <rttr/registration>
#include <iostream>

using namespace rttr;

struct MyStruct
{
  MyStruct()
  {};
  
  void func(double)
  {};
  
  int data;
};

enum class MetaData_Type
{
  SCRIPTABLE,
  GUI
};

RTTR_REGISTRATION
{
  registration::class_<MyStruct>("MyStruct")
    .constructor<>()
    .property("data", &MyStruct::data)
      (
        metadata(MetaData_Type::SCRIPTABLE, false),
        metadata("Description", "This is the data")
      )
    .method("func", &MyStruct::func);
}

void foo()
{
  type t = type::get<MyStruct>();
  for (auto& prop : t.get_properties())
    std::cout << "name: " << prop.get_name() << std::endl;

  for (auto& meth : t.get_methods())
    std::cout << "name: " << meth.get_name() << std::endl;
}
