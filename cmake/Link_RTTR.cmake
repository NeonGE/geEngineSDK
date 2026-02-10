function(ge_link_rttr target_name)
  if(TARGET RTTR::Core)
    target_link_libraries(${target_name} SYSTEM PRIVATE RTTR::Core)
  elseif(TARGET rttr_core_lib)
    target_link_libraries(${target_name} SYSTEM PRIVATE rttr_core_lib)
  elseif(TARGET rttr_core)
    target_link_libraries(${target_name} SYSTEM PRIVATE rttr_core)
  else()
    message(FATAL_ERROR
      "RTTR targets not found. Expected RTTR::Core or rttr_core_lib/rttr_core. "
      "Make sure Externals_RTTR.cmake is included before calling ge_link_rttr()."
    )
  endif()
endfunction()

function(ge_rttr_resolve_target out_var)
  set(_t "")
  if(TARGET RTTR::Core)
    # RTTR::Core puede ser ALIAS
    get_target_property(_aliased RTTR::Core ALIASED_TARGET)
    if(_aliased)
      set(_t "${_aliased}")
    else()
      set(_t "RTTR::Core")
    endif()
  elseif(TARGET rttr_core_lib)
    set(_t "rttr_core_lib")
  elseif(TARGET rttr_core)
    set(_t "rttr_core")
  endif()

  if(_t STREQUAL "")
    message(FATAL_ERROR "RTTR target not found (RTTR::Core/rttr_core_lib/rttr_core).")
  endif()

  set(${out_var} "${_t}" PARENT_SCOPE)
endfunction()

function(ge_link_rttr target_name)
  ge_rttr_resolve_target(_rttr_target)
  target_link_libraries(${target_name} PRIVATE ${_rttr_target})
endfunction()

function(ge_copy_rttr_runtime target_name)
  if(NOT WIN32)
    return()
  endif()

  ge_rttr_resolve_target(_rttr_target)

  # Solo copia si RTTR es una DLL
  get_target_property(_type "${_rttr_target}" TYPE)
  if(_type STREQUAL "SHARED_LIBRARY")
    add_custom_command(TARGET ${target_name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              "$<TARGET_FILE:${_rttr_target}>"
              "$<TARGET_FILE_DIR:${target_name}>"
      COMMENT "Copying RTTR DLL next to ${target_name}"
    )
  endif()
endfunction()