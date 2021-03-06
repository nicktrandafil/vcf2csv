if (NOT BUILD_TESTS)
  return()
endif()


set(test_vcf2csv_core_src
  main.cpp
  test.cpp
  vcf.cpp
  vcf_iterator.cpp
  quoted_printable.cpp
)


add_executable(test_vcf2csv_core ${test_vcf2csv_core_src})
target_compile_options(test_vcf2csv_core PRIVATE ${compile_options})
set_target_properties(test_vcf2csv_core PROPERTIES ${properties})

if(NOT MINGW)
  target_compile_definitions(test_vcf2csv_core PRIVATE "BOOST_TEST_DYN_LINK")
endif()


target_include_directories(test_vcf2csv_core
  PRIVATE
  ../src
  ../src/3rd_party
  ${Boost_INCLUDE_DIRS}
)

target_link_libraries(test_vcf2csv_core
  vcf2csv_core
  ${Boost_LIBRARIES}
)


add_test(vcf test_vcf2csv_core --run_test=vcf)
add_test(test test_vcf2csv_core --run_test=test)
add_test(quoted_printable test_vcf2csv_core --run_test=quoted_printable)
add_test(vcf_iterator test_vcf2csv_core --run_test=vcf_iterator)
