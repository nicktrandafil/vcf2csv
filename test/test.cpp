// boost test
#include <boost/test/unit_test.hpp>

// boost spirit
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>

// std
#include <sstream>


namespace qi  = boost::spirit::qi;
namespace phx = boost::phoenix;


using namespace qi::labels;


BOOST_AUTO_TEST_SUITE(test)


BOOST_AUTO_TEST_CASE(end)
{
    const std::string ins{"123=\n456"};
    qi::rule<std::string::const_iterator, void()> soft_break = '=' >> qi::eol;

    std::string out;
    const auto r = qi::parse(
        ins.begin(),
        ins.end(),
        *(
            (
                 qi::eps(phx::val(true))
              >> soft_break
            )
          |
            (qi::char_ - qi::eol)[phx::ref(out) += _1]));

    BOOST_CHECK(r);
    BOOST_CHECK_EQUAL("123456", out);
}



BOOST_AUTO_TEST_SUITE_END()
