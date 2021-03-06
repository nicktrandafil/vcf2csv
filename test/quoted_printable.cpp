// tested
#include <quoted_printable.hpp>

// boost test
#include <boost/test/unit_test.hpp>


using namespace vcf::quoted_printable;


BOOST_AUTO_TEST_SUITE(quoted_printable)


BOOST_AUTO_TEST_CASE(pure)
{
    std::string x{"abc"};
    BOOST_CHECK_EQUAL("abc", decode(x));
}


BOOST_AUTO_TEST_CASE(hello)
{
    std::string x{"=d0=bf=d1=80=d0=b8=d0=b2=d0=b5=d1=82"};
    std::string expected{"привет"};
    const auto actual = decode(x);

    BOOST_CHECK_EQUAL(expected, actual);
}


BOOST_AUTO_TEST_CASE(hello_mixed)
{
    std::string x{"=d0=bf=d1=80=d0=b8w=d0=b5=d1=82"};
    std::string expected{"приwет"};
    const auto actual = decode(x);

    BOOST_CHECK_EQUAL(expected, actual);
}


BOOST_AUTO_TEST_CASE(soft_break)
{
    std::string x{
        "=d0=bf=d1=80=d0=b8w=\n"
        "=d0=b5=d1=82"
    };

    std::string expected{"приwет"};
    const auto actual = decode(x);

    BOOST_CHECK_EQUAL(expected, actual);
}


BOOST_AUTO_TEST_SUITE_END()
