function(ge_link_rttr target_name)
  if(TARGET RTTR::Core)
    target_link_libraries(${target_name} PRIVATE RTTR::Core)
  elseif(TARGET rttr_core_lib)
    target_link_libraries(${target_name} PRIVATE rttr_core_lib)
  elseif(TARGET rttr_core)
    target_link_libraries(${target_name} PRIVATE rttr_core)
  else()
    message(FATAL_ERROR
      "RTTR targets not found. Expected RTTR::Core or rttr_core_lib/rttr_core. "
      "Make sure Externals_RTTR.cmake is included before calling ge_link_rttr()."
    )
  endif()
endfunction()