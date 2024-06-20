#include "PyrSlot.h"
#include "PyrObject.h"
#include "PyrKernel.h"
void PyrSlotTest() {
    {
        PyrSlot i = PyrSlot::make(static_cast<int32_t>(32));
        assert(i.isInt());
        assert(!i.isPtr());
        assert(!i.isDouble());
        assert(!i.isObjectHdr());
        assert(!i.isNil());
        assert(!i.isTrue());
        assert(!i.isFalse());
        assert(!i.isChar());
        assert(i.getInt() == 32);
    }
    {
        PyrSlot i = PyrSlot::make(static_cast<char>(52));
        assert(!i.isInt());
        assert(!i.isPtr());
        assert(!i.isDouble());
        assert(!i.isObjectHdr());
        assert(!i.isNil());
        assert(!i.isTrue());
        assert(!i.isFalse());
        assert(i.isChar());
        assert(i.getChar() == 52);
    }
    {
        int a = 10;
        int* ap = &a;

        PyrSlot s_p = PyrSlot::make(ap);
        assert(s_p.isPtr());
        assert(!s_p.isObjectHdr());
        assert(!s_p.isInt());
        assert(!s_p.isChar());
        assert(!s_p.isNil());
        assert(!s_p.isTrue());
        assert(!s_p.isFalse());
        void* s_pt = s_p.getPtr();
        assert(s_pt == ap);
    }
    {
        PyrObject o;
        PyrSlot s = PyrSlot::make(&o);
        assert(s.isObjectHdr());
        assert(!s.isPtr());
        assert(!s.isInt());
        assert(!s.isChar());
        assert(!s.isNil());
        assert(!s.isTrue());
        assert(!s.isFalse());
        assert(s.getObjectHdr() == &o);
    }
    {
        PyrBlock b;
        PyrSlot s = PyrSlot::make(&b);
        assert(s.isObjectHdr());
        assert(!s.isPtr());
        assert(!s.isInt());
        assert(!s.isChar());
        assert(!s.isNil());
        assert(!s.isTrue());
        assert(!s.isFalse());
        assert(s.getObjectHdr() == &b);
    }
    {
        PyrSlot s = PyrSlot::make((PyrBlock*)nullptr);
        assert(s.isObjectHdr());
        assert(!s.isPtr());
        assert(!s.isInt());
        assert(!s.isChar());
        assert(!s.isNil());
        assert(!s.isTrue());
        assert(!s.isFalse());
        assert(s.getObjectHdr() == nullptr);
    }
}
