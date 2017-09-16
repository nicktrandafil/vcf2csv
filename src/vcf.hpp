#pragma once


// local
#include "vcard.hpp"

// boost
#include <boost/iterator/iterator_facade.hpp>

// std
#include <iterator>
#include <memory>


namespace vcf {


class Vcf;


class VcfIterator
    : public boost::iterator_facade<
        VcfIterator,
        VCard,
        boost::single_pass_traversal_tag
      >
{
public:
    VcfIterator();
    ~VcfIterator();

    explicit VcfIterator(Vcf& x);
    explicit VcfIterator(const VcfIterator& rhs);
    explicit VcfIterator(VcfIterator&& rhs);

private:
    friend boost::iterator_core_access;

    void increment();
    bool equal(const VcfIterator& rhs) const;
    reference dereference() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


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
