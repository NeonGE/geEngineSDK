function(ge_enable_strict_warnings target)

  if (MSVC)
    target_compile_options(${target} PRIVATE
      /W4            # warning level alto
      /permissive-   # modo estándar estricto
      /w14242        # 'identifier': conversion, possible loss of data
      /w14254
      /w14263
      /w14265
      /w14287
      /we4289        # nonstandard extension used
      /w14296
      /w14311
      /w14545
      /w14546
      /w14547
      /w14549
      /w14555
      /w14619
      /w14640
      /w14826
      /w14905
      /w14906
      /w14928
    )

  else() # GCC / Clang
    target_compile_options(${target} PRIVATE
      -Wall
      -Wextra
      -Wpedantic
      -Wshadow
      -Wconversion
      -Wsign-conversion
      -Wdouble-promotion
      -Wformat=2
      -Wundef
      -Wold-style-cast
      -Woverloaded-virtual
      -Wnon-virtual-dtor
      -Wnull-dereference
      -Wuseless-cast
      -Wimplicit-fallthrough
    )
  endif()

endfunction()