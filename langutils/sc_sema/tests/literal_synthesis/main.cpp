// Must be defined before the include, so the header provides main.
#define BOOST_TEST_MODULE sc_sema_literal_synthesis_test
#include <boost/test/included/unit_test.hpp>

#include "ast2ir/literal_synthesis.hpp"
#include <clocale>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

// Float literals are parsed by the C library, so these check every platform gives the same, correctly rounded, value.

namespace {
using sc::ast::FloatNode;

std::optional<double> parse(std::string_view str, FloatNode::Kind kind = FloatNode::Kind::Normal,
                            FloatNode::Sign sign = FloatNode::Sign::Positive) {
    return sc::ir::synthesize_literal(FloatNode { kind, sign }, str);
}

std::uint64_t bits(double d) {
    std::uint64_t b;
    std::memcpy(&b, &d, sizeof b);
    return b;
}

std::string describe(std::optional<double> d) {
    if (!d)
        return "nothing";
    std::ostringstream stream;
    stream << std::hexfloat << *d;
    return stream.str();
}

// Bit exact, so -0.0 and 0.0 differ.
void check(std::string_view str, double expected, FloatNode::Kind kind = FloatNode::Kind::Normal,
           FloatNode::Sign sign = FloatNode::Sign::Positive) {
    const auto value = parse(str, kind, sign);
    BOOST_TEST((value && bits(*value) == bits(expected)),
               str << " gave " << describe(value) << ", expected " << describe(expected));
}

void check_rejected(std::string_view str, FloatNode::Kind kind = FloatNode::Kind::Normal) {
    const auto value = parse(str, kind);
    BOOST_TEST(!value, str << " gave " << describe(value) << ", expected nothing");
}
}

BOOST_AUTO_TEST_CASE(simple) {
    check("1.5", 0x1.8p0);
    check("0.5", 0x1p-1);
    check("123.0", 123.0);
    check("00012.5", 12.5);
    check("0.1", 0x1.999999999999ap-4);
    check("0.30000000000000004", 0x1.3333333333334p-2);
    check("3.141592653589793238", 0x1.921fb54442d18p1);
}

BOOST_AUTO_TEST_CASE(exponent) {
    check("1e10", 1e10, FloatNode::Kind::Exponent);
    check("1.5e-5", 0x1.f75104d551d69p-17, FloatNode::Kind::Exponent);
    check("1e0", 1.0, FloatNode::Kind::Exponent);
    check("1e22", 0x1.0f0cf064dd592p73, FloatNode::Kind::Exponent);
    // Not exactly representable, the nearest is below.
    check("1e23", 0x1.52d02c7e14af6p76, FloatNode::Kind::Exponent);
}

BOOST_AUTO_TEST_CASE(zero) {
    check("0", 0.0);
    check("0.0", 0.0);
    check("00000.0", 0.0);
    check("0e5", 0.0, FloatNode::Kind::Exponent);
    check("0.000e-999", 0.0, FloatNode::Kind::Exponent);
}

BOOST_AUTO_TEST_CASE(sign) {
    check("1.5", -1.5, FloatNode::Kind::Normal, FloatNode::Sign::Negative);
    check("0.0", -0.0, FloatNode::Kind::Normal, FloatNode::Sign::Negative);
    check("1e10", -1e10, FloatNode::Kind::Exponent, FloatNode::Sign::Negative);
}

BOOST_AUTO_TEST_CASE(infinity) {
    constexpr auto inf = std::numeric_limits<double>::infinity();
    check("inf", inf, FloatNode::Kind::Inf);
    check("inf", -inf, FloatNode::Kind::Inf, FloatNode::Sign::Negative);
    // `inf` is also lexed as a normal float.
    check("inf", inf);
}

BOOST_AUTO_TEST_CASE(round_to_nearest_even) {
    // 2^53 + 1 is halfway between 2^53 and 2^53 + 2, ties go to the even mantissa.
    check("9007199254740993", 9007199254740992.0);
    check("9007199254740995", 9007199254740996.0);
    // Any digit past the halfway point rounds up.
    check("9007199254740993.0000000000000000001", 9007199254740994.0);
    // The exact decimal expansion of 0.1's double.
    check("0.1000000000000000055511151231257827021181583404541015625", 0x1.999999999999ap-4);
    check("1.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
          1.0);
}

BOOST_AUTO_TEST_CASE(largest) {
    constexpr auto max = std::numeric_limits<double>::max();
    check("1.7976931348623157e308", max, FloatNode::Kind::Exponent);
    // Below halfway to the next power of two, so rounds down to the largest.
    check("1.7976931348623158e308", max, FloatNode::Kind::Exponent);
    check_rejected("1.7976931348623159e308", FloatNode::Kind::Exponent);
    check_rejected("1.8e308", FloatNode::Kind::Exponent);
    check_rejected("1e400", FloatNode::Kind::Exponent);
    check_rejected("1e99999", FloatNode::Kind::Exponent);
}

BOOST_AUTO_TEST_CASE(smallest) {
    check("2.2250738585072014e-308", std::numeric_limits<double>::min(), FloatNode::Kind::Exponent);
    // The largest denormal, this hung some strtod implementations.
    check("2.2250738585072011e-308", 0x0.fffffffffffffp-1022, FloatNode::Kind::Exponent);
    check("2.5e-310", 0x0.02e055c9a3f6cp-1022, FloatNode::Kind::Exponent);
    check("4.9406564584124654e-324", 0x1p-1074, FloatNode::Kind::Exponent);
    check("4.9e-324", 0x1p-1074, FloatNode::Kind::Exponent);
    check("3e-324", 0x1p-1074, FloatNode::Kind::Exponent);
    // Either side of half the smallest denormal.
    check("2.4703282292062328e-324", 0x1p-1074, FloatNode::Kind::Exponent);
    check_rejected("2.4703282292062327e-324", FloatNode::Kind::Exponent);
    check_rejected("2e-324", FloatNode::Kind::Exponent);
    check_rejected("1e-400", FloatNode::Kind::Exponent);
    check_rejected("1e-99999", FloatNode::Kind::Exponent);
}

BOOST_AUTO_TEST_CASE(locale) {
    // Qt sets the locale from the environment, where the decimal point may be a comma.
    const char* const comma_locales[] = { "de_DE.UTF-8", "de_DE.utf8", "de_DE", "German_Germany.1252", "de-DE" };
    const char* found = nullptr;
    for (const char* name : comma_locales)
        if (std::setlocale(LC_ALL, name)) {
            found = name;
            break;
        }
    if (!found) {
        BOOST_TEST_MESSAGE("no locale with a comma decimal point is installed, skipping");
        return;
    }
    BOOST_TEST_MESSAGE("using locale " << found);
    check("1.5", 0x1.8p0);
    check("2.5e-310", 0x0.02e055c9a3f6cp-1022, FloatNode::Kind::Exponent);
    check_rejected("1,5");
    std::setlocale(LC_ALL, "C");
}
