#pragma once


// std
#include <memory>
#include <optional>
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
    std::optional<std::string> charset;
    std::optional<std::string> encoding;
    std::string                value;
};


struct Name
{
    std::optional<std::string> charset;
    std::optional<std::string> encoding;
    std::string                surname;
    std::string                first_name;
    std::string                patronymic;
    std::string                prefix;
    std::string                suffix;
};


struct Tel
{
    std::optional<std::string> type;
    std::string                value;
};


struct Addr
{
    std::optional<std::string> type;
    std::optional<std::string> charset;
    std::optional<std::string> encoding;
    std::string                postal_box;
    std::string                extended_address;
    std::string                street;
    std::string                town;
    std::string                region;
    std::string                postal_code;
    std::string                country;
};


} // namespace attributes


struct VCard
{
    std::optional<attribute::Version>       version;
    std::optional<attribute::Name>          name;
    std::optional<attribute::FormattedName> formatted_name;
    std::optional<attribute::Tel>           tel;
    std::optional<attribute::Addr>          address;
    std::string                             unkonwn;
};


} // namespace vcf
