#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <sys/types.h>
#include <type_traits>

#include "PyrSlot.h"
#include "PyrObject.h"
#include "PyrKernel.h"

BOOST_TEST_DONT_PRINT_LOG_VALUE(PyrSlot)

BOOST_AUTO_TEST_CASE(slot_test) { { PyrSlot i = PyrSlot::make(static_cast<int32_t>(32));
BOOST_TEST_REQUIRE(i.isInt());
BOOST_TEST_REQUIRE(!i.isPtr());
BOOST_TEST_REQUIRE(!i.isDouble());
BOOST_TEST_REQUIRE(!i.isObjectHdr());
BOOST_TEST_REQUIRE(!i.isNil());
BOOST_TEST_REQUIRE(!i.isTrue());
BOOST_TEST_REQUIRE(!i.isFalse());
BOOST_TEST_REQUIRE(!i.isChar());
BOOST_TEST_REQUIRE(i.getInt() == 32);
}
{
    PyrSlot i = PyrSlot::make(static_cast<char>(52));
    BOOST_TEST_REQUIRE(!i.isInt());
    BOOST_TEST_REQUIRE(!i.isPtr());
    BOOST_TEST_REQUIRE(!i.isDouble());
    BOOST_TEST_REQUIRE(!i.isObjectHdr());
    BOOST_TEST_REQUIRE(!i.isNil());
    BOOST_TEST_REQUIRE(!i.isTrue());
    BOOST_TEST_REQUIRE(!i.isFalse());
    BOOST_TEST_REQUIRE(i.isChar());
    BOOST_TEST_REQUIRE(i.getChar() == 52);
}
{
    int a = 10;
    int* ap = &a;
    PyrSlot s_p = PyrSlot::make((void*)ap);
    BOOST_TEST_REQUIRE(s_p.isPtr());
    BOOST_TEST_REQUIRE(!s_p.isDouble());
    BOOST_TEST_REQUIRE(!s_p.isSymbol());
    BOOST_TEST_REQUIRE(!s_p.isObjectHdr());
    BOOST_TEST_REQUIRE(!s_p.isInt());
    BOOST_TEST_REQUIRE(!s_p.isChar());
    BOOST_TEST_REQUIRE(!s_p.isNil());
    BOOST_TEST_REQUIRE(!s_p.isTrue());
    BOOST_TEST_REQUIRE(!s_p.isFalse());
    void* s_pt = s_p.getPtr();
    BOOST_TEST_REQUIRE(s_pt == ap);
}
{
    PyrObject o;
    PyrSlot s = PyrSlot::make(&o);
    BOOST_TEST_REQUIRE(s.isObjectHdr());
    BOOST_TEST_REQUIRE(!s.isPtr());
    BOOST_TEST_REQUIRE(!s.isInt());
    BOOST_TEST_REQUIRE(!s.isChar());
    BOOST_TEST_REQUIRE(!s.isNil());
    BOOST_TEST_REQUIRE(!s.isTrue());
    BOOST_TEST_REQUIRE(!s.isFalse());
    BOOST_TEST_REQUIRE(s.getObjectHdr() == &o);
}
{
    PyrBlock b;
    PyrSlot s = PyrSlot::make(&b);
    BOOST_TEST_REQUIRE(s.isObjectHdr());
    BOOST_TEST_REQUIRE(!s.isPtr());
    BOOST_TEST_REQUIRE(!s.isInt());
    BOOST_TEST_REQUIRE(!s.isChar());
    BOOST_TEST_REQUIRE(!s.isNil());
    BOOST_TEST_REQUIRE(!s.isTrue());
    BOOST_TEST_REQUIRE(!s.isFalse());
    BOOST_TEST_REQUIRE(s.getObjectHdr() == &b);
}
{
    PyrSlot s = PyrSlot::make((PyrBlock*)nullptr);
    BOOST_TEST_REQUIRE(s.isObjectHdr());
    BOOST_TEST_REQUIRE(!s.isPtr());
    BOOST_TEST_REQUIRE(!s.isInt());
    BOOST_TEST_REQUIRE(!s.isChar());
    BOOST_TEST_REQUIRE(!s.isNil());
    BOOST_TEST_REQUIRE(!s.isTrue());
    BOOST_TEST_REQUIRE(!s.isFalse());
    BOOST_TEST_REQUIRE(s.getObjectHdr() == nullptr);
}
#ifndef _MSC_VER
// Cannot divide by zero in constexpr on MSVC
{
    const auto r = PyrSlot::make(1.0 / 0.0);
    BOOST_TEST_REQUIRE(r.isDouble());
    BOOST_TEST_REQUIRE(r.getDouble() == 1.0 / 0.0);
}
{
    const auto r = PyrSlot::make(-1.0 / 0.0);
    BOOST_TEST_REQUIRE(r.isDouble());
    BOOST_TEST_REQUIRE(r.getDouble() == -1.0 / 0.0);
}
{
    const auto r = PyrSlot::make(-0.0 / 0.0);
    BOOST_TEST_REQUIRE(r.isDouble());
    const auto d = r.getDouble();
    BOOST_TEST_REQUIRE(std::isnan(r.getDouble()));
}
{
    const auto r = PyrSlot::make(0.0 / 0.0);
    BOOST_TEST_REQUIRE(r.isDouble());
    BOOST_TEST_REQUIRE(std::isnan(r.getDouble()));
}
#endif
}

BOOST_AUTO_TEST_CASE(nan_tests) {
    // const auto safe_nan_double = details::bit_cast<double>(details::safeNaN);
    // BOOST_TEST(std::isnan(safe_nan_double));

    BOOST_TEST(removeBadNans(1.0) == 1.0);
    BOOST_TEST(removeBadNans(-1.0) == -1.0);

    const auto test_bad_nan_conversion = [](const char* s) {
        const auto nan = std::nan(s);
        const auto safe_nan = removeBadNans(nan);
        BOOST_TEST(std::isnan(safe_nan));
        const auto nan_slot = PyrSlot::make<AssertDouble::CouldBeBadNan>(nan);
        BOOST_TEST(nan_slot.isDouble());
        BOOST_TEST(std::isnan(nan_slot.getDouble()));
        const auto d = nan_slot.getDouble();
        const auto d_int = details::bit_cast<uint64_t>(d);

        // After going through the slot, all nans should be the same.
        const auto nan_0_int = details::bit_cast<uint64_t>(std::nan("0"));
        BOOST_TEST(d_int == nan_0_int);
    };

    // Just some random quiet nans. These should all be converted to the safe nan.
    test_bad_nan_conversion("1");
    test_bad_nan_conversion("2");
    test_bad_nan_conversion("3");
    test_bad_nan_conversion("33456344");
    test_bad_nan_conversion("93563455656");

    {
        const auto nan = std::nan("0");
        BOOST_TEST(std::isnan(nan));
        // Assume this is a safe nan value. This is the type of nan we might produce.
        const auto nan_slot = PyrSlot::make<AssertDouble::Okay>(nan);
        BOOST_TEST(nan_slot.isDouble());
        BOOST_TEST(std::isnan(nan_slot.getDouble()));
    }
}

BOOST_AUTO_TEST_CASE(char_tests) {
    for (size_t i { 0 }; i < 256; ++i) {
        const auto c = static_cast<char>(i);
        const auto slot = PyrSlot::make(c);
        BOOST_TEST(slot.isChar());
        BOOST_TEST(slot.getChar() == c);
    }
}

BOOST_AUTO_TEST_CASE(bool_test) {
    const auto true_slot = PyrSlot::make(true);
    const auto false_slot = PyrSlot::make(false);

    BOOST_TEST(true_slot.isTrue());
    BOOST_TEST(false_slot.isFalse());
    BOOST_TEST(!true_slot.isFalse());
    BOOST_TEST(!false_slot.isTrue());
}

BOOST_AUTO_TEST_CASE(ordering_test) {
    const auto a = PyrSlot::make(0.1);
    const auto b = PyrSlot::make(0.2);
    const auto c = PyrSlot::make(2);

    BOOST_TEST(a < b);
    BOOST_TEST(a <= b);

    // strict weak ordering
    BOOST_TEST((a < a) == false);

    BOOST_TEST((a < b));
    BOOST_TEST(b < c);
    BOOST_TEST(a < c);
}
