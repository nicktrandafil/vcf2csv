# Compile options
set(compile_options)
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
   set(compile_options
    $<$<CONFIG:Release>:
      -O2
      -DNDEBUG
    >
    $<$<CONFIG:Debug>:
      -O0
      -ggdb3
      -Wall
    >
  )
endif()


# Properties
set(properties CXX_STANDARD 14)


# Debug config shortcut
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(debug)
endif()
