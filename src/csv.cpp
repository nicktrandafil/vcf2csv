#include "csv.hpp"

// local
#include <meta/any.hpp>

// 3rd
#include <folly/ScopeGuard.h>

// boost
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>

// std
#include <algorithm>
#include <sstream>


namespace vcf {
namespace csv {
namespace {


constexpr char delim{','};
constexpr char bound{'"'};


} // namespace


std::vector<std::string> parse(const std::string &row) {
  std::vector<std::string> ret;
  bool delim_enabled{true};
  char prev{-1};
  std::string cell;

  for (auto cur: row) {
    SCOPE_EXIT { prev = cur; };

    if (cur == delim && delim_enabled) {
      ret.push_back(cell);
      cell.clear();
      continue;
    }

    if (bound == cur && bound == prev) {
      cell += bound;
      delim_enabled = !delim_enabled;
      cur = -1;
      continue;
    }

    if (bound == cur) {
      delim_enabled = !delim_enabled;
      continue;
    }

    cell += cur;
  }

  ret.push_back(cell);

  return ret;
}


namespace {


std::string serializeCell(const std::string &cell) {
  const auto tmp = boost::replace_all_copy(
    cell, std::string{bound},  std::string{bound, bound});

  const auto escape = std::any_of(
    tmp.begin(), tmp.end(),
    [](char c) { return meta::any(c, delim, bound, '\n', '\r'); });

  return escape ? bound + tmp + bound : tmp;
}


} // namespace


std::string serialize(const std::vector<std::string> &row) {
  return boost::join(
    row | boost::adaptors::transformed(&serializeCell), std::string{delim});
}


} // namespace csv
} // namespace vcf
