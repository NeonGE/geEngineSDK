#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <gePrerequisitesUtilities.h>
#include <geVector3.h>

#if USING(GE_REFLECTION)
#include <geRTTRMeta.h>
using namespace rttr;

using namespace geEngineSDK;

TEST_CASE("RTTR Dependency Testing", "[CORE][RTTR]")
{
  std::cout << "RTTR Dependency Testing:\n";

  std::cout << "Class: Vector3\n";
  std::cout << "  Properties:\n";
  type t = type::get<Vector3>();
  for (auto& prop : t.get_properties()) {
    std::cout << "    Name: " << prop.get_name() << std::endl;
    if (auto tooltip = prop.get_metadata(MetaData_Type::TOOLTIP)) {
      if (tooltip.is_valid()) {
        std::cout << "      Tooltip: " << tooltip.get_value<String>() << std::endl;
      }
    }
  }

  std::cout << "  Methods:\n";
  for (auto& meth : t.get_methods()) {
    std::cout << "    Name: " << meth.get_name() << std::endl;
    if (auto tooltip = meth.get_metadata(MetaData_Type::TOOLTIP)) {
      if (tooltip.is_valid()) {
        std::cout << "      Tooltip: " << tooltip.get_value<String>() << std::endl;
      }
    }
  }
}

#endif // USING(GE_REFLECTION)