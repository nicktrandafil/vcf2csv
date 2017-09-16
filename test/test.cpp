// boost test
#include <boost/test/unit_test.hpp>

// boost spirit
#include <boost/spirit/include/qi.hpp>


namespace qi = boost::spirit::qi;


BOOST_AUTO_TEST_SUITE(test)


BOOST_AUTO_TEST_CASE(end)
{
    std::string source{"x"};
    std::string res;

    auto it = source.begin();
    const auto ok = qi::phrase_parse(
        it,
        source.end(),
        -(*qi::char_ >> ':'),
        qi::blank,
        qi::skip_flag::dont_postskip,
        res);

    BOOST_CHECK(ok);
}



BOOST_AUTO_TEST_SUITE_END()
