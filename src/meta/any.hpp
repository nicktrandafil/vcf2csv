#pragma once


namespace vcf::meta {


template <typename T, typename ...Args>
constexpr bool any(T&& t, Args&&... args)
{
    return ((t == args) || ...);
}


} // namespace vcf:meta
