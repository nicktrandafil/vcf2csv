// boost test
#include <boost/test/unit_test.hpp>

// std
#include <sstream>


BOOST_AUTO_TEST_SUITE(test)


BOOST_AUTO_TEST_CASE(end)
{
    std::string source{"5"};
    std::istringstream ins{source};

    auto it = std::istream_iterator<int>(ins);

    BOOST_CHECK(ins.eof());
    BOOST_CHECK(it != std::istream_iterator<int>());
}



BOOST_AUTO_TEST_SUITE_END()
