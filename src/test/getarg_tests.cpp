#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-P33");
    BOOST_CHECK(GetBoolArg("-P33"));
    BOOST_CHECK(GetBoolArg("-P33", false));
    BOOST_CHECK(GetBoolArg("-P33", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-P33o"));
    BOOST_CHECK(!GetBoolArg("-P33o", false));
    BOOST_CHECK(GetBoolArg("-P33o", true));

    ResetArgs("-P33=0");
    BOOST_CHECK(!GetBoolArg("-P33"));
    BOOST_CHECK(!GetBoolArg("-P33", false));
    BOOST_CHECK(!GetBoolArg("-P33", true));

    ResetArgs("-P33=1");
    BOOST_CHECK(GetBoolArg("-P33"));
    BOOST_CHECK(GetBoolArg("-P33", false));
    BOOST_CHECK(GetBoolArg("-P33", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noP33");
    BOOST_CHECK(!GetBoolArg("-P33"));
    BOOST_CHECK(!GetBoolArg("-P33", false));
    BOOST_CHECK(!GetBoolArg("-P33", true));

    ResetArgs("-noP33=1");
    BOOST_CHECK(!GetBoolArg("-P33"));
    BOOST_CHECK(!GetBoolArg("-P33", false));
    BOOST_CHECK(!GetBoolArg("-P33", true));

    ResetArgs("-P33 -noP33");  // -P33 should win
    BOOST_CHECK(GetBoolArg("-P33"));
    BOOST_CHECK(GetBoolArg("-P33", false));
    BOOST_CHECK(GetBoolArg("-P33", true));

    ResetArgs("-P33=1 -noP33=1");  // -P33 should win
    BOOST_CHECK(GetBoolArg("-P33"));
    BOOST_CHECK(GetBoolArg("-P33", false));
    BOOST_CHECK(GetBoolArg("-P33", true));

    ResetArgs("-P33=0 -noP33=0");  // -P33 should win
    BOOST_CHECK(!GetBoolArg("-P33"));
    BOOST_CHECK(!GetBoolArg("-P33", false));
    BOOST_CHECK(!GetBoolArg("-P33", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--P33=1");
    BOOST_CHECK(GetBoolArg("-P33"));
    BOOST_CHECK(GetBoolArg("-P33", false));
    BOOST_CHECK(GetBoolArg("-P33", true));

    ResetArgs("--noP33=1");
    BOOST_CHECK(!GetBoolArg("-P33"));
    BOOST_CHECK(!GetBoolArg("-P33", false));
    BOOST_CHECK(!GetBoolArg("-P33", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-P33", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-P33", "eleven"), "eleven");

    ResetArgs("-P33 -bar");
    BOOST_CHECK_EQUAL(GetArg("-P33", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-P33", "eleven"), "");

    ResetArgs("-P33=");
    BOOST_CHECK_EQUAL(GetArg("-P33", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-P33", "eleven"), "");

    ResetArgs("-P33=11");
    BOOST_CHECK_EQUAL(GetArg("-P33", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-P33", "eleven"), "11");

    ResetArgs("-P33=eleven");
    BOOST_CHECK_EQUAL(GetArg("-P33", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-P33", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-P33", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-P33", 0), 0);

    ResetArgs("-P33 -bar");
    BOOST_CHECK_EQUAL(GetArg("-P33", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-P33=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-P33", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-P33=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-P33", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--P33");
    BOOST_CHECK_EQUAL(GetBoolArg("-P33"), true);

    ResetArgs("--P33=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-P33", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noP33");
    BOOST_CHECK(!GetBoolArg("-P33"));
    BOOST_CHECK(!GetBoolArg("-P33", true));
    BOOST_CHECK(!GetBoolArg("-P33", false));

    ResetArgs("-noP33=1");
    BOOST_CHECK(!GetBoolArg("-P33"));
    BOOST_CHECK(!GetBoolArg("-P33", true));
    BOOST_CHECK(!GetBoolArg("-P33", false));

    ResetArgs("-noP33=0");
    BOOST_CHECK(GetBoolArg("-P33"));
    BOOST_CHECK(GetBoolArg("-P33", true));
    BOOST_CHECK(GetBoolArg("-P33", false));

    ResetArgs("-P33 --noP33");
    BOOST_CHECK(GetBoolArg("-P33"));

    ResetArgs("-noP33 -P33"); // P33 always wins:
    BOOST_CHECK(GetBoolArg("-P33"));
}

BOOST_AUTO_TEST_SUITE_END()
