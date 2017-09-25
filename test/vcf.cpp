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

    BOOST_CHECK(!x.tel[0].type);
    BOOST_CHECK_EQUAL(x.tel[0].value, "+12345");

    BOOST_CHECK_EQUAL(
        "bzz\n",
        x.unkonwn);
}


BOOST_AUTO_TEST_CASE(v2_1_version_n_fn_tel)
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

    BOOST_CHECK_EQUAL("home,work", *x.tel[0].type);
    BOOST_CHECK_EQUAL(x.tel[0].value, "+12345");
}


BOOST_AUTO_TEST_CASE(v3_0_version_n_fn_tel)
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

    BOOST_CHECK_EQUAL("CELL", *x.tel[0].type);
    BOOST_CHECK_EQUAL(x.tel[0].value, "+12345");
}


BOOST_AUTO_TEST_CASE(v_2_1_tel_multiple_times)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "TEL;WORK;VOICE:(111) 555-1212\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK_EQUAL("WORK,VOICE", *x.tel[0].type);
    BOOST_CHECK_EQUAL(x.tel[0].value, "(111) 555-1212");
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

    BOOST_CHECK(!x.tel[0].type);
    BOOST_CHECK_EQUAL(x.tel[0].value, "+12345");

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

    BOOST_CHECK(x.address.size() == 1);

    const auto& address = x.address[0];
    BOOST_CHECK(!address.type);
    BOOST_CHECK(!address.charset);
    BOOST_CHECK(!address.encoding);

    BOOST_CHECK_EQUAL("", address.postal_box);
    BOOST_CHECK_EQUAL("", address.extended_address);
    BOOST_CHECK_EQUAL("123 Main Street", address.street);
    BOOST_CHECK_EQUAL("Any Town", address.town);
    BOOST_CHECK_EQUAL("CA", address.region);
    BOOST_CHECK_EQUAL("91921-1234", address.postal_code);
    BOOST_CHECK_EQUAL("US", address.country);
}


BOOST_AUTO_TEST_CASE(v2_1_addr_type_params)
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

    BOOST_CHECK(x.address.size() == 1);
    const auto& address = x.address[0];
    BOOST_CHECK(address.type);
    BOOST_CHECK(!address.charset);
    BOOST_CHECK(!address.encoding);

    BOOST_CHECK_EQUAL("CELL,HOME", *address.type);
}


BOOST_AUTO_TEST_CASE(v3_0_addr_type_params)
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

    BOOST_CHECK(x.address.size() == 1);
    const auto& address = x.address[0];
    BOOST_CHECK(address.type);
    BOOST_CHECK(!address.charset);
    BOOST_CHECK(!address.encoding);

    BOOST_CHECK_EQUAL("cell,home", *address.type);
}


BOOST_AUTO_TEST_CASE(v3_0_addr_charset_encoding_params)
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

    BOOST_CHECK(x.address.size() == 1);
    const auto& address = x.address[0];
    BOOST_CHECK(address.type);
    BOOST_CHECK(address.charset);
    BOOST_CHECK(address.encoding);

    BOOST_CHECK_EQUAL("cell,home", *address.type);

    BOOST_CHECK_EQUAL("UTF-8", *address.charset);
    BOOST_CHECK_EQUAL("QUOTED-PRINTABLE", *address.encoding);
}


BOOST_AUTO_TEST_CASE(v3_0_multi_entry)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "ADR:;;;;;;\n"
        "END:VCARD\n"
        "BEGIN:VCARD\n"
        "ADR:abc;;;;;;\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    {
        VCard x;
        parser >> x;
    
        BOOST_CHECK(!parser.error());
        BOOST_CHECK(!parser.eof());
    
        BOOST_CHECK(x.address.size() == 1);
        BOOST_CHECK_EQUAL("", x.address[0].postal_box);
    }

    {
        VCard x;
        parser >> x;
    
        BOOST_CHECK(!parser.error());
        BOOST_CHECK(parser.eof());
    
        BOOST_CHECK(x.address.size() == 1);
        BOOST_CHECK_EQUAL("abc", x.address[0].postal_box);
    }
}


BOOST_AUTO_TEST_CASE(v3_0_multi_address)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "ADR:abc;;;;;;\n"
        "ADR:def;;;;;;\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK(!parser.error());
    BOOST_CHECK(parser.eof());
 
    BOOST_CHECK(x.address.size() == 2);
    BOOST_CHECK_EQUAL("abc", x.address[0].postal_box);
    BOOST_CHECK_EQUAL("def", x.address[1].postal_box);
}


BOOST_AUTO_TEST_CASE(line_carry)
{
    std::istringstream ins{
        "BEGIN:VCARD\n"
        "FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=d0=bf=d1=80=d0=b8w=\n"
        "=d0=b5=d1=82\n"
        "END:VCARD"
    };

    Vcf parser(
        std::istreambuf_iterator<char>{ins},
        std::istreambuf_iterator<char>{});

    VCard x;
    parser >> x;

    BOOST_CHECK(!parser.error());
    BOOST_CHECK(parser.eof());

    BOOST_CHECK_EQUAL("=d0=bf=d1=80=d0=b8w=d0=b5=d1=82", x.formatted_name->value);
}


BOOST_AUTO_TEST_SUITE_END()
