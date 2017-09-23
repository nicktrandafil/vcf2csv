#pragma once


#include <boost/optional/optional.hpp>

// std
#include <string>
#include <vector>


namespace vcf {
namespace attribute {


struct Version
{
    std::string value;
};


struct FormattedName
{
    boost::optional<std::string> charset;
    boost::optional<std::string> encoding;
    std::string                  value;
};


struct Name
{
    boost::optional<std::string> charset;
    boost::optional<std::string> encoding;
    std::string                  surname;
    std::string                  first_name;
    std::string                  patronymic;
    std::string                  prefix;
    std::string                  suffix;
};


struct Tel
{
    boost::optional<std::string> type;
    std::string                  value;
};


struct Addr
{
    boost::optional<std::string> type;
    boost::optional<std::string> charset;
    boost::optional<std::string> encoding;
    std::string                  postal_box;
    std::string                  extended_address;
    std::string                  street;
    std::string                  town;
    std::string                  region;
    std::string                  postal_code;
    std::string                  country;
};


} // namespace attributes


struct VCard
{
    boost::optional<attribute::Version>       version;
    boost::optional<attribute::Name>          name;
    boost::optional<attribute::FormattedName> formatted_name;
    std::vector<attribute::Tel>               tel;
    std::vector<attribute::Addr>              address;
    std::string                               unkonwn;
};


} // namespace vcf
