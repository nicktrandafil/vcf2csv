// tested
#include <vcf.hpp>

// boost
#include <boost/test/unit_test.hpp>

// std
#include <sstream>


using namespace vcf;


BOOST_AUTO_TEST_SUITE(vcf)


BOOST_AUTO_TEST_CASE(initial_state_for_empy_range)
{
    Vcf parser({}, {});
    BOOST_CHECK(!parser.good());
}


BOOST_AUTO_TEST_CASE(initial_state_for_string)
{
    std::istringstream ins{""};

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    BOOST_CHECK(!parser.good());
}


BOOST_AUTO_TEST_CASE(initial_state_for_nonempty_string)
{
    std::istringstream ins{"f"};

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    BOOST_CHECK(parser.good());
}


BOOST_AUTO_TEST_CASE(unkonwn_version_fn_tel)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "VERSION:2.1\n"
        "N:surname;na\\;m\\\\e;;;\n"
        "FN:surname name\n"
        "TEL:+12345\n"
        "bzz\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK(!parser.error());
    BOOST_CHECK(parser.eof());

    BOOST_CHECK_EQUAL("2.1", x.version->value);

    BOOST_CHECK(!x.name->charset);
    BOOST_CHECK(!x.name->encoding);
    BOOST_CHECK_EQUAL(x.name->surname, "surname");
    BOOST_CHECK_EQUAL(x.name->first_name, "na;m\\e");
    BOOST_CHECK(x.name->patronymic.empty());
    BOOST_CHECK(x.name->prefix.empty());
    BOOST_CHECK(x.name->suffix.empty());

    BOOST_CHECK(!x.formatted_name->charset);
    BOOST_CHECK(!x.formatted_name->encoding);
    BOOST_CHECK_EQUAL(x.formatted_name->value, "surname name");

    BOOST_CHECK(!x.tel->type);
    BOOST_CHECK_EQUAL(x.tel->value, "+12345");

    BOOST_CHECK_EQUAL(
        "bzz\n",
        x.unkonwn);
}


BOOST_AUTO_TEST_CASE(version_n_fn_tel_2_1)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "VERSION:2.1\n"
        "N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=54=C3su rn=B8ame;name;;;\n"
        "FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=54=C3su rn=B8ame name\n"
        "TEL;TYPE=home,work:+12345\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK(!parser.error());
    BOOST_CHECK(parser.eof());

    BOOST_CHECK_EQUAL("2.1", x.version->value);

    BOOST_CHECK_EQUAL("UTF-8", *x.name->charset);
    BOOST_CHECK_EQUAL("QUOTED-PRINTABLE", *x.name->encoding);
    BOOST_CHECK_EQUAL("=54=C3su rn=B8ame", x.name->surname);
    BOOST_CHECK_EQUAL("name", x.name->first_name);
    BOOST_CHECK(x.name->patronymic.empty());
    BOOST_CHECK(x.name->prefix.empty());
    BOOST_CHECK(x.name->suffix.empty());

    BOOST_CHECK_EQUAL("UTF-8", *x.formatted_name->charset);
    BOOST_CHECK_EQUAL("QUOTED-PRINTABLE", *x.formatted_name->encoding);
    BOOST_CHECK_EQUAL("=54=C3su rn=B8ame name", x.formatted_name->value);

    BOOST_CHECK_EQUAL("home,work", *x.tel->type);
    BOOST_CHECK_EQUAL(x.tel->value, "+12345");
}


BOOST_AUTO_TEST_CASE(version_n_fn_tel_3_0)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "VERSION:2.1\n"
        "N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=54=C3su rn=B8ame;name;;;\n"
        "FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=54=C3su rn=B8ame name\n"
        "TEL;CELL:+12345\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK_EQUAL("CELL", *x.tel->type);
    BOOST_CHECK_EQUAL(x.tel->value, "+12345");
}


BOOST_AUTO_TEST_CASE(unknown_version_n_fn_tel_no_params)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "VERSION:2.1\n"
        "N:surname;na\\;m\\\\e;;;\n"
        "bzz\n"
        "FN:surname name\n"
        "bzz\n"
        "TEL:+12345\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK(!parser.error());
    BOOST_CHECK(parser.eof());

    BOOST_CHECK_EQUAL("2.1", x.version->value);

    BOOST_CHECK(!x.name->charset);
    BOOST_CHECK(!x.name->encoding);
    BOOST_CHECK_EQUAL(x.name->surname, "surname");
    BOOST_CHECK_EQUAL(x.name->first_name, "na;m\\e");
    BOOST_CHECK(x.name->patronymic.empty());
    BOOST_CHECK(x.name->prefix.empty());
    BOOST_CHECK(x.name->suffix.empty());

    BOOST_CHECK(!x.formatted_name->charset);
    BOOST_CHECK(!x.formatted_name->encoding);
    BOOST_CHECK_EQUAL(x.formatted_name->value, "surname name");

    BOOST_CHECK(!x.tel->type);
    BOOST_CHECK_EQUAL(x.tel->value, "+12345");

    BOOST_CHECK_EQUAL(
        "bzz\n"
        "bzz\n",
        x.unkonwn);
}


BOOST_AUTO_TEST_CASE(addr_no_params)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "ADR:;;123 Main Street;Any Town;CA;91921-1234;US\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK(!parser.error());
    BOOST_CHECK(parser.eof());

    BOOST_CHECK(!x.address->type);
    BOOST_CHECK(!x.address->charset);
    BOOST_CHECK(!x.address->encoding);

    BOOST_CHECK_EQUAL("", x.address->postal_box);
    BOOST_CHECK_EQUAL("", x.address->extended_address);
    BOOST_CHECK_EQUAL("123 Main Street", x.address->street);
    BOOST_CHECK_EQUAL("Any Town", x.address->town);
    BOOST_CHECK_EQUAL("CA", x.address->region);
    BOOST_CHECK_EQUAL("91921-1234", x.address->postal_code);
    BOOST_CHECK_EQUAL("US", x.address->country);
}


BOOST_AUTO_TEST_CASE(addr_2_1_type_params)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "ADR;CELL;HOME:;;;;;;\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK(!parser.error());
    BOOST_CHECK(parser.eof());

    BOOST_CHECK(x.address->type);
    BOOST_CHECK(!x.address->charset);
    BOOST_CHECK(!x.address->encoding);

    BOOST_CHECK_EQUAL("CELL,HOME", *x.address->type);
}


BOOST_AUTO_TEST_CASE(addr_3_0_type_params)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "ADR;TYPE=cell,home:;;;;;;\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK(!parser.error());
    BOOST_CHECK(parser.eof());

    BOOST_CHECK(x.address->type);
    BOOST_CHECK(!x.address->charset);
    BOOST_CHECK(!x.address->encoding);

    BOOST_CHECK_EQUAL("cell,home", *x.address->type);
}


BOOST_AUTO_TEST_CASE(addr_3_0_charset_encoding_params)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "ADR;TYPE=cell,home;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;;;;;\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK(!parser.error());
    BOOST_CHECK(parser.eof());

    BOOST_CHECK(x.address->type);
    BOOST_CHECK(x.address->charset);
    BOOST_CHECK(x.address->encoding);

    BOOST_CHECK_EQUAL("cell,home", *x.address->type);

    BOOST_CHECK_EQUAL("UTF-8", *x.address->charset);
    BOOST_CHECK_EQUAL("QUOTED-PRINTABLE", *x.address->encoding);
}


BOOST_AUTO_TEST_SUITE_END()
