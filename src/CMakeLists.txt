# vcf2csv_core target src list
set(vcf2csv_core_src
  3rd_party/folly/ScopeGuard.cpp
  vcf.cpp
)


#
# vcf2csv_core target
#

add_library(vcf2csv_core ${vcf2csv_core_src})
target_compile_options(vcf2csv_core PUBLIC ${compile_options})
set_target_properties(vcf2csv_core PROPERTIES ${properties})
target_compile_definitions(vcf2csv_core PUBLIC "FOLLY_NO_CONFIG")

target_include_directories(vcf2csv_core
  PUBLIC
  .
  3rd_party
  ${Boost_INCLUDE_DIRS}
)


# vcf2csv src list
set(vcf2csv_src main.cpp)


#
# vcf2csv target
#

add_executable(vcf2csv ${vcf2csv_src})
set_target_properties(vcf2csv PROPERTIES ${properties})


# Install
install(TARGETS vcf2csv RUNTIME DESTINATION bin)
