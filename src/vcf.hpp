#pragma once


// local
#include "vcard.hpp"

// std
#include <iterator>
#include <memory>


namespace vcf {


class Vcf
{
public:
    Vcf(
        std::istreambuf_iterator<char> first,
        std::istreambuf_iterator<char> last);

    ~Vcf();

    Vcf& operator>>(VCard& x);

    bool good() const;
    bool eof()  const;
    bool error() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


} // namesapce vcf
