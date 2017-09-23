cmake_minimum_required(VERSION 3.1)


# Custom scripts
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


project("vcf2csv" VERSION 0.0.1)


#
# Options
#

option(BUILD_TESTS "Собирать тесты" OFF)


# To reflect CMAKE_BUILD_TYPE changes into the GUI
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Тип сборки" FORCE)
endif()


include(compile_options)


#
# Dependencies
#

find_package(Boost COMPONENTS system filesystem QUIET REQUIRED)

if(BUILD_TESTS)
  find_package(Boost COMPONENTS unit_test_framework QUIET REQUIRED)
endif()


# Enables ctest in the root
enable_testing()


#
# Project moduels
#

add_subdirectory(src)
add_subdirectory(test)
