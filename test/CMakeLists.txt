if (NOT BUILD_TESTS)
  return()
endif()

find_package(Boost COMPONENTS unit_test_framework QUIET REQUIRED)

set(test_vcf2csv_core_src
  main.cpp
)

add_executable(test_vcf2csv_core ${test_vcf2csv_core_src})
target_compile_options(test_vcf2csv_core PRIVATE ${compile_options})
set_target_properties(test_vcf2csv_core PROPERTIES ${properties})
target_compile_definitions(test_vcf2csv_core PRIVATE "BOOST_TEST_DYN_LINK")
target_include_directories(test_vcf2csv_core PRIVATE ../src ../src/3rd_party)

target_link_libraries(test_vcf2csv_core
  vcf2csv_core
  boost_unit_test_framework
)
