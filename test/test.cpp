// boost test
#include <boost/test/unit_test.hpp>

// std
#include <sstream>


BOOST_AUTO_TEST_SUITE(test)


BOOST_AUTO_TEST_CASE(end)
{
    std::string source{"xy"};
    std::istringstream ins{source};

    auto it = std::istreambuf_iterator<char>(ins);
    ++it;

    char c;
    ins >> c;

    BOOST_CHECK(c == 'y');
}



BOOST_AUTO_TEST_SUITE_END()
