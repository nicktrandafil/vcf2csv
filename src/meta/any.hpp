#pragma once


// std
#include <initializer_list>


namespace vcf {
namespace meta {


template <typename T, typename ...Args>
constexpr bool any(T&& t, Args&&... args)
{
    bool match{false};
    (void)std::initializer_list<bool>{(match = match || t == args)...};
    return match;
}


} // namepace meta
} // namespace vcf
