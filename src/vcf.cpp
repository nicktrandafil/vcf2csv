#include "vcf.hpp"
#include "vcf_iterator.hpp"

// boost spirit
#include <boost/assert.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

// std
#include <cassert>


namespace ascii  = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace qi     = boost::spirit::qi;
namespace phx    = boost::phoenix;


BOOST_FUSION_ADAPT_STRUCT(
    vcf::attribute::Version,
    value
)


BOOST_FUSION_ADAPT_STRUCT(
    vcf::attribute::Name,
    charset,
    encoding,
    surname,
    first_name,
    patronymic,
    prefix,
    suffix
)


BOOST_FUSION_ADAPT_STRUCT(
    vcf::attribute::FormattedName,
    charset,
    encoding,
    value
)


BOOST_FUSION_ADAPT_STRUCT(
    vcf::attribute::Tel,
    type,
    value
)


BOOST_FUSION_ADAPT_STRUCT(
    vcf::attribute::Addr,
    type,
    charset,
    encoding,
    postal_box,
    extended_address,
    street,
    town,
    region,
    postal_code,
    country
)


BOOST_FUSION_ADAPT_STRUCT(
    vcf::VCard,
    version,
    name,
    formatted_name,
    tel,
    address,
    unkonwn
)


namespace vcf {
namespace {


void append(std::optional<std::string> &x, std::string value)
{
    if (!x) {
        x = std::move(value);
    } else {
        *x += ',' + std::move(value);
    }
};



template <typename Iterator>
struct Grammer : qi::grammar<Iterator, VCard(), ascii::blank_type>
{
    Grammer()
        : Grammer::base_type(vcard, "vcard")
    {
        using ascii::alnum;
        using ascii::char_;
        using ascii::graph;
        using ascii::print;
        using qi::eol;
        using qi::lit;
        using qi::lexeme;
        using qi::string;
        using phx::at_c;
        using phx::push_back;

        using namespace qi::labels;

        // Controls
        control %= char_(',') | char_(';') | char_('\\') | char_('\n');

        // Escaped controls
        escaped_control %= '\\' >> control;

        // Text
        text %= lexeme[*((print - control) | escaped_control)];

        // Param value
        param_value %= +(print - ';' - ':');

        // Param
        param %= lit(_r1) >> '=' >> param_value;

        // Verstion attribute
        version %= "VERSION:" >> +graph;

        // Name attribute
        name %=
               'N'
            >> -(lit(';') >> param(phx::ref("CHARSET")))
            >> -(lit(';') >> param(phx::ref("ENCODING")))
            >> ':'
            >> text            // surname
            >> -(';' >> text)  // name
            >> -(';' >> text)  // patronymic
            >> -(';' >> text)  // prefix
            >> -(';' >> text); // sufix

        // Formatted name attribute
        formatted_name %=
               "FN"
            >> -(lit(';') >> param(phx::ref("CHARSET")))
            >> -(lit(';') >> param(phx::ref("ENCODING")))
            >> ':'
            >> text;

        // Tel attribute
        tel =
               "TEL"
            >> *(
                    (lit(';') >> param(phx::ref("TYPE"))[at_c<0>(_val) = _1])
                 |  (lit(';') >> param_value            [phx::bind(&append, at_c<0>(_val), _1)])
                )
            >> ':'
            >> text[at_c<1>(_val) = _1];


        // Address attribute
        addr =
              "ADR"
            >> *(
                   (lit(';') >> param(phx::ref("TYPE")))    [at_c<0>(_val) = _1]
                 | (lit(';') >> param(phx::ref("CHARSET"))) [at_c<1>(_val) = _1]
                 | (lit(';') >> param(phx::ref("ENCODING")))[at_c<2>(_val) = _1]
                 | (lit(';') >> param_value)                [phx::bind(&append, at_c<0>(_val), _1)]
                )
            >> ':' >> text[at_c<3>(_val) = _1]     // postal box
            >> ';' >> text[at_c<4>(_val) = _1]     // extended_address
            >> ';' >> text[at_c<5>(_val) = _1]     // street
            >> ';' >> text[at_c<6>(_val) = _1]     // town
            >> ';' >> text[at_c<7>(_val) = _1]     // region
            >> ';' >> text[at_c<8>(_val) = _1]     // postal_code
            >> ';' >> text[at_c<9>(_val) = _1];    // country

        // Unkonwn attributes
        unkonwn %= *(!eol >> char_) >> (char_('\n') | string("\r\n"));

        // VCard
        vcard =
              "BEGIN:VCARD"                                >> eol
            >> *(
                    !lit("END:VCARD")
                 >> (
                       version        [at_c<0>(_val) =  _1]          >> eol
                     | name           [at_c<1>(_val) =  _1]          >> eol
                     | formatted_name [at_c<2>(_val) =  _1]          >> eol
                     | tel            [push_back(at_c<3>(_val), _1)] >> eol
                     | addr           [push_back(at_c<4>(_val), _1)] >> eol
                     | unkonwn        [at_c<5>(_val) += _1]
                    )
                )
            >> "END:VCARD" >> -eol;
    }

    qi::rule<Iterator, VCard(),                    ascii::blank_type> vcard;
    qi::rule<Iterator, attribute::Version(),       ascii::blank_type> version;
    qi::rule<Iterator, attribute::Name(),          ascii::blank_type> name;
    qi::rule<Iterator, attribute::FormattedName(), ascii::blank_type> formatted_name;
    qi::rule<Iterator, attribute::Tel(),           ascii::blank_type> tel;
    qi::rule<Iterator, attribute::Addr(),          ascii::blank_type> addr;
    qi::rule<Iterator, std::string()                                > unkonwn;

    qi::rule<Iterator, std::string()                                > text;
    qi::rule<Iterator, char()                                       > control;
    qi::rule<Iterator, char()                                       > escaped_control;
    qi::rule<Iterator, std::string(std::string),   ascii::blank_type> param;
    qi::rule<Iterator, std::string(),              ascii::blank_type> param_value;
};


} // namespace


struct Vcf::Impl
{
    using BaseIterator      = std::istreambuf_iterator<char>;
    using MultiPassIterator = spirit::multi_pass<BaseIterator>;

    Impl(
        std::istreambuf_iterator<char> first,
        std::istreambuf_iterator<char> last)
            : it{spirit::make_default_multi_pass(first)}
            , last{spirit::make_default_multi_pass(last)}
            , error{false}
    {}

    MultiPassIterator          it;
    MultiPassIterator          last;
    Grammer<MultiPassIterator> grammer;
    bool                       error;
};


Vcf::Vcf(
    std::istreambuf_iterator<char> first,
    std::istreambuf_iterator<char> last)
        : impl{std::make_unique<Impl>(std::move(first), std::move(last))}
{}


Vcf::~Vcf() = default;


Vcf& Vcf::operator>>(VCard& x)
{
    impl->error =
      !qi::phrase_parse(
          impl->it,
          impl->last,
          impl->grammer,
          ascii::blank,
          qi::skip_flag::dont_postskip,
          x);
    return *this;
}


bool Vcf::good() const
{
    return !eof() && !error();
}


bool Vcf::eof() const
{
    return impl->it == impl->last;
}


bool Vcf::error() const
{
    return impl->error;
}


struct VcfIterator::Impl
{
    explicit Impl(Vcf* x)
        : vcf{x}
        , end{true}
    {}

    Vcf*  vcf;
    VCard current;
    bool end;
};


VcfIterator::VcfIterator()
    : impl{std::make_unique<Impl>(nullptr)}
{}


VcfIterator::~VcfIterator() = default;


VcfIterator::VcfIterator(Vcf& x)
    : impl{std::make_unique<Impl>(&x)}
{
    x >> impl->current;
    impl->end = impl->vcf->error();
}


VcfIterator::VcfIterator(const VcfIterator& rhs)
    : impl{std::make_unique<Impl>(*rhs.impl)}
{}


VcfIterator::VcfIterator(VcfIterator&& rhs)
    : impl{std::move(rhs.impl)}
{}


void VcfIterator::increment()
{
    impl->end = !(impl->vcf && impl->vcf->good());
    if (impl->end) { return; }
    impl->current = {};
    *impl->vcf >> impl->current;
}


VcfIterator::reference VcfIterator::dereference() const
{
    BOOST_ASSERT(!impl->end);
    return impl->current;
}


bool VcfIterator::equal(const VcfIterator& rhs) const
{
    if (!(!impl->vcf ^ !rhs.impl->vcf) && impl->vcf) {
        return impl->vcf == rhs.impl->vcf;
    } else {
        return impl->end == rhs.impl->end;
    }
}


} // namespace vcf
