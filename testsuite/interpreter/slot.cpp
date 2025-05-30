#include <boost/test/unit_test.hpp>

#include "PyrSlot.h"
#include "PyrObject.h"
#include "PyrKernel.h"

BOOST_AUTO_TEST_CASE(slot_test) {
    {
        PyrSlot i = PyrSlot::make(static_cast<int32_t>(32));
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
        PyrSlot s_p = PyrSlot::make(ap);
        BOOST_TEST_REQUIRE(s_p.isPtr());
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
}
