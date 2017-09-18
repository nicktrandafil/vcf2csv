#pragma once


// local
#include "vcard.hpp"

// boost
#include <boost/iterator/iterator_facade.hpp>

// std
#include <memory>


namespace vcf {


class Vcf;


class VcfIterator
    : public boost::iterator_facade<
        VcfIterator,
        VCard const,
        boost::single_pass_traversal_tag
      >
{
public:
    VcfIterator();
    ~VcfIterator();

    explicit VcfIterator(Vcf& x);
    VcfIterator(const VcfIterator& rhs);
    VcfIterator(VcfIterator&& rhs);

private:
    friend boost::iterator_core_access;

    void increment();
    bool equal(const VcfIterator& rhs) const;
    reference dereference() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};


} // namespace vcf
