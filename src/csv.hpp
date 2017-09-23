#pragma once

// std
#include <string>
#include <vector>


namespace vcf {
namespace csv {


std::vector<std::string> parse(const std::string &row);


std::string serialize(const std::vector<std::string> &row);


} // namespace csv
} // namespace vcf
