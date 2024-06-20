/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/
/*

PyrSlot is a value holder for SC variables.
A PyrSlot is an 8-byte value which is either a double precision float or a
32-bit tag plus a 32-bit value.

*/

#pragma once

#include <string>
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <cstring>
#include "SC_Endian.h"
#include "PyrErrors.h"
#include "function_attributes.h"

// TODO: are all these necessary? Or does the pointer size suffice?
#if (__SIZEOF_POINTER__ == 8) || defined(__x86_64__) || defined(_M_X64) || defined(__LP64__) || defined(_WIN64)
#    define POINTER_NEEDS_PADDING 0
#elif (__SIZEOF_POINTER__ == 4) || defined(__i386__) || defined(_M_IX86) || defined(__ILP32__) || defined(_WIN32)      \
    || defined(__ppc__) || defined(__arm__)
#    define POINTER_NEEDS_PADDING 1
#else
#    error "no PyrSlot imlementation for this platform"
#endif

// On 64-bit systems the pointer is assumed to fit into 48 bits.
// This is not true for some very modern intel systems which use 56 bits
//      - but this is not in common use and nan boxing is very common,
//      so it is likely there will be a workaround (...hopefully...).

//        |-----------------| these are free in a pointer
// Ptr  = 1111 1111 1111 1111 000000000000000000000000000000000000000000000000

//  Double spec:
//     sign bit
//        |  exponent                       fraction
//        |   11-bit                         52-bit
//        ||-----------| |---------------------------------------------------|
//  Inf = 0111 1111 1111 0000 000000000000000000000000000000000000000000000000 = 7FF0 0000 0000 0000
// -Inf = 1111 1111 1111 0000 000000000000000000000000000000000000000000000000 = FFF0 0000 0000 0000

// If exponents are all 1 and fraction isn't 0, its some type of nan
// sNaN = 0111 1111 1111 0000 000000000000000000000000000000000000000000000001 = 7FF0 0000 0000 0001
// qNaN = 0111 1111 1111 1000 000000000000000000000000000000000000000000000001 = 7FF8 0000 0000 0001
// qNaN = 1111 1111 1111 1000 000000000000000000000000000000000000000000000001 = FFF8 0000 0000 0001
//        | sign is irrelevant to nan
// qNaN = 1111 1111 1111 1111 111111111111111111111111111111111111111111111111 = FFFF FFFF FFFF FFFF
//                     still qNaN


// Signaling NaN (sNaN) is used to signal some floating point error has occurred.
// Quiet NaN (qNaN) is normal nan.
// We only use qNaN to store the tags in.

namespace details {

struct Masks {
    static constexpr uint64_t nanExponent = 0x7FF8000000000000;
    static constexpr uint64_t boxedBit = 0x0001000000000000;
    static constexpr uint64_t boxed = boxedBit | nanExponent;
    static constexpr uint64_t pointer = 0x0000FFFFFFFFFFFF;
    static constexpr uint64_t tagMask = 0xFFFF000000000000;
};

// So this means the free bits in each are...
// 0 = used, 1 = free bit
// qNan = 1000 0000 0000 0111 111111111111111111111111111111111111111111111111
// Ptr  = 1111 1111 1111 1111 000000000000000000000000000000000000000000000000

// This leaves these 4 bits in which a tag can be stored.
// 1000 0000 0000 0111 000000000000000000000000000000000000000000000000

// condensing this to 4 bits...
// 0...000 double - the far right bit is called the boxed bit, it is always set if not a double.
// 0...001 nil
// 0...011 int
// 0...101 sym
// 0...111 char
// 1...011 ptr
// 1...101 false
// 1...111 objHdrPtr

// If the data is less than 48 bits, additional bits can be used to create further tags.


struct Tags {
    // These tags include the boxed mask, which makes it easier to test if they are active.
    // The binary values are written above in the comment. KEEP THEM IN SYNC!
    static constexpr uint64_t objHdrTag = 0xFFFF000000000000;
    static constexpr uint64_t intTag = 0x7FFB000000000000;
    static constexpr uint64_t symTag = 0x7FFD000000000000;
    static constexpr uint64_t charTag = 0x7FFF000000000000;
    static constexpr uint64_t trueTag = 0xFFF9000000000000;
    static constexpr uint64_t ptrTag = 0xFFFB000000000000;
    static constexpr uint64_t nilTag = 0x7FF9000000000000;
    static constexpr uint64_t falseTag = 0xFFFD000000000000;
};


template <typename T> struct PadValueTo64Bits {
    static_assert(std::is_default_constructible_v<T>);
    static_assert(sizeof(T) <= 6);
    static constexpr size_t numPadding = 8 - sizeof(T);

    constexpr explicit PadValueTo64Bits(T v) noexcept: value(v) {}
    constexpr PadValueTo64Bits() = default;

    ~PadValueTo64Bits() noexcept = default;

    // TODO: this could be the wrong way around!
#if (BYTE_ORDER == LITTLE_ENDIAN)
    T value;
    uint8_t paddingForValue[numPadding] = { 0 };
#else
    uint8_t paddingForValue[numPadding] = { 0 };
    T value;
#endif
};

static_assert(sizeof(PadValueTo64Bits<uint32_t>) == 8);
static_assert(sizeof(PadValueTo64Bits<uint16_t>) == 8);
static_assert(sizeof(PadValueTo64Bits<uint8_t>) == 8);

template <typename T> struct MaybePadPointerTo64Bits {
    static_assert(std::is_pointer_v<T>);
#if POINTER_NEEDS_PADDING && (BYTE_ORDER == BIG_ENDIAN)
    int32_t padding;
#endif

    T ptr;

    [[nodiscard]] T getPtr() const noexcept {
#if POINTER_NEEDS_PADDING
        return ptr; // If there is padding, the tag hasn't tainted the pointer, return it.
#else
        return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(ptr) & Masks::pointer);
#endif
    }

#if POINTER_NEEDS_PADDING && (BYTE_ORDER == LITTLE_ENDIAN)
    int32_t padding;
#endif
};

static_assert(sizeof(MaybePadPointerTo64Bits<void*>) == sizeof(double));

// cpp reference
template <class To, class From>
std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>,
                 To>
bit_cast(const From& src) noexcept {
    static_assert(std::is_trivially_constructible_v<To>,
                  "This implementation additionally requires "
                  "destination type to be trivially constructible");
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

}

// This is the old tag and their values are assumed and used as indices, do not change them!
enum {
    tagNotInitialized, // uninitialized slots have a tag of 0
    tagObj,
    tagInt,
    tagSym,
    tagChar,
    tagNil,
    tagFalse,
    tagTrue,
    tagPtr,
    tagFloat,
    tagUnused,
};

// A Tag used in construct of a slot containing nil
struct PyrNil {};

union PyrSlot {
private:
    using Tags = details::Tags;
    using Masks = details::Masks;
    template <typename T> using PadValueTo64Bits = details::PadValueTo64Bits<T>;
    template <typename T> using MaybePadPointerTo64Bits = details::MaybePadPointerTo64Bits<T>;

    struct PrivateTag {};
    constexpr PyrSlot(PrivateTag, uint64_t raw) noexcept: u_raw(raw) {}
    constexpr PyrSlot(PrivateTag, uint64_t tag, uint64_t raw) noexcept: u_raw(tag | raw) {}
    constexpr PyrSlot(PrivateTag, double d) noexcept: u_double(d) {}

    // The held types, note, they should all be 64-bits in size.
    uint64_t u_raw;
    double u_double;
    PadValueTo64Bits<char> u_char;
    PadValueTo64Bits<int32_t> u_int;
    MaybePadPointerTo64Bits<void*> u_ptr;
    MaybePadPointerTo64Bits<struct PyrObjectHdr*> u_objectHeader;
    MaybePadPointerTo64Bits<struct PyrSymbol*> u_symbol;

    [[nodiscard]] inline static constexpr bool bitsAreSet(uint64_t source, uint64_t bits) noexcept {
        return (source & bits) == bits;
    }
    [[nodiscard]] inline static constexpr uint16_t getTagAsU16(uint64_t t) noexcept {
        return static_cast<uint16_t>(t >> 48);
    }
    [[nodiscard]] inline constexpr bool isBoxed() const noexcept { return bitsAreSet(u_raw, Masks::boxed); }
    template <uint64_t T> [[nodiscard]] inline constexpr bool tagChecker() const noexcept {
        return getTagAsU16(u_raw) == getTagAsU16(T);
    }

public:
    constexpr PyrSlot() noexcept: u_raw(Tags::nilTag) {}
    ~PyrSlot() noexcept = default;
    constexpr PyrSlot(PyrSlot&&) noexcept = default;
    constexpr PyrSlot(const PyrSlot&) noexcept = default;
    constexpr PyrSlot& operator=(PyrSlot&&) noexcept = default;
    constexpr PyrSlot& operator=(const PyrSlot&) noexcept = default;

    [[nodiscard]] friend bool operator==(PyrSlot lhs, PyrSlot rhs) noexcept {
        if (lhs.isDouble()) {
            // Doubles have odd comparison rules...
            if (rhs.isDouble())
                return lhs.getDouble() == rhs.getDouble();
            return false;
        }
        if (rhs.isDouble())
            return false;
        return lhs.u_raw == rhs.u_raw;
    }

    [[nodiscard]] constexpr static PyrSlot make(double d) noexcept { return { PrivateTag(), d }; }
    [[nodiscard]] static PyrSlot make(char c) noexcept {
        return { PrivateTag(), Tags::charTag, static_cast<uint64_t>(details::bit_cast<uint8_t>(c)) };
    }
    [[nodiscard]] static PyrSlot make(struct PyrObjectHdr* o) {
        return { PrivateTag(), Tags::objHdrTag, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(o)) };
    }
    [[nodiscard]] static PyrSlot make(struct PyrSymbol* o) {
        return { PrivateTag(), Tags::symTag, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(o)) };
    }
    [[nodiscard]] static PyrSlot make(int32_t i) {
        return { PrivateTag(), Tags::intTag, static_cast<uint64_t>(details::bit_cast<uint32_t>(i)) };
    }
    [[nodiscard]] static PyrSlot make(void* o) {
        return { PrivateTag(), Tags::ptrTag, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(o)) };
    }
    [[nodiscard]] constexpr static PyrSlot make(PyrNil) noexcept { return {}; }
    [[nodiscard]] constexpr static PyrSlot make(bool b) noexcept {
        return { PrivateTag(), b ? Tags::trueTag : Tags::falseTag };
    }

    [[nodiscard]] bool isDouble() const noexcept { return !isBoxed(); }
    [[nodiscard]] bool isChar() const noexcept { return tagChecker<Tags::charTag>(); }
    [[nodiscard]] bool isInt() const noexcept { return tagChecker<Tags::intTag>(); }
    [[nodiscard]] bool isPtr() const noexcept { return tagChecker<Tags::ptrTag>(); }
    [[nodiscard]] bool isObjectHdr() const noexcept { return tagChecker<Tags::objHdrTag>(); }
    [[nodiscard]] bool isSymbol() const noexcept { return tagChecker<Tags::symTag>(); }
    [[nodiscard]] bool isNil() const noexcept { return u_raw == Tags::nilTag; }
    [[nodiscard]] bool isTrue() const noexcept { return u_raw == Tags::trueTag; }
    [[nodiscard]] bool isFalse() const noexcept { return u_raw == Tags::falseTag; }

    [[nodiscard]] double getDouble() const noexcept {
        assert(isDouble());
        return u_double;
    }
    [[nodiscard]] char getChar() const noexcept {
        assert(isChar());
        return u_char.value;
    }
    [[nodiscard]] int32_t getInt() const noexcept {
        assert(isInt());
        return u_int.value;
    }
    [[nodiscard]] void* getPtr() const noexcept {
        assert(isPtr());
        return u_ptr.getPtr();
    }
    [[nodiscard]] struct PyrObjectHdr* getObjectHdr() const noexcept {
        assert(isObjectHdr());
        return u_objectHeader.getPtr();
    }
    [[nodiscard]] struct PyrSymbol* getSymbol() const noexcept {
        assert(isSymbol());
        return u_symbol.getPtr();
    }
    template <typename T> [[nodiscard]] T* getPyrObject() const noexcept {
        // types are incomplete here so can't check...
        // static_assert(std::is_base_of<PyrObjectHdr, T>::value, "Type must derive from PyrObjectHeader");
        if (isObjectHdr())
            return reinterpret_cast<T*>(u_objectHeader.getPtr());
        assert(isNil() || (isInt() && getInt() == 0));
        return nullptr;
    }

    [[nodiscard]] HOT int getTag() const noexcept {
        switch (getTagAsU16(u_raw)) {
        case getTagAsU16(Tags::symTag):
            return tagSym;
        case getTagAsU16(Tags::intTag):
            return tagInt;
        case getTagAsU16(Tags::charTag):
            return tagChar;
        case getTagAsU16(Tags::objHdrTag):
            return tagObj;
        case getTagAsU16(Tags::ptrTag):
            return tagPtr;
        case getTagAsU16(Tags::nilTag):
            return tagNil;
        case getTagAsU16(Tags::trueTag):
            return tagTrue;
        case getTagAsU16(Tags::falseTag):
            return tagFalse;
        default:
            return tagFloat;
        }
    }

    // getClass is implemented over in PyrObject.h as it needs access to gTagClassTable.
    inline struct PyrClass* getClass();
};

static_assert(sizeof(PyrSlot) == sizeof(double));

[[nodiscard]] inline int GetTag(const PyrSlot* slot) noexcept { return slot->getTag(); }

[[nodiscard]] inline bool IsObj(const PyrSlot* slot) noexcept { return slot->isObjectHdr(); }
[[nodiscard]] inline bool NotObj(const PyrSlot* slot) noexcept { return !(slot->isObjectHdr()); }
[[nodiscard]] inline bool IsNil(const PyrSlot* slot) noexcept { return slot->isNil(); }
[[nodiscard]] inline bool NotNil(const PyrSlot* slot) noexcept { return !slot->isNil(); }
[[nodiscard]] inline bool IsFalse(const PyrSlot* slot) noexcept { return slot->isFalse(); }
[[nodiscard]] inline bool IsTrue(const PyrSlot* slot) noexcept { return slot->isTrue(); }
[[nodiscard]] inline bool IsSym(const PyrSlot* slot) noexcept { return slot->isSymbol(); }
[[nodiscard]] inline bool NotSym(const PyrSlot* slot) noexcept { return !slot->isSymbol(); }
[[nodiscard]] inline bool IsChar(const PyrSlot* slot) noexcept { return slot->isChar(); }
[[nodiscard]] inline bool NotChar(const PyrSlot* slot) noexcept { return !slot->isChar(); }
[[nodiscard]] inline bool IsInt(const PyrSlot* slot) noexcept { return slot->isInt(); }
[[nodiscard]] inline bool NotInt(const PyrSlot* slot) noexcept { return !slot->isInt(); }
[[nodiscard]] inline bool IsFloat(const PyrSlot* slot) noexcept { return slot->isDouble(); }
[[nodiscard]] inline bool NotFloat(const PyrSlot* slot) noexcept { return !slot->isDouble(); }
[[nodiscard]] inline bool IsPtr(const PyrSlot* slot) noexcept { return slot->isPtr(); }
[[nodiscard]] inline bool NotPtr(const PyrSlot* slot) noexcept { return !slot->isPtr(); }

inline void SetInt(PyrSlot* slot, int32_t value) noexcept { *slot = PyrSlot::make(value); }
inline void SetObject(PyrSlot* slot, struct PyrObjectHdr* value) noexcept { *slot = PyrSlot::make(value); }
inline void SetSymbol(PyrSlot* slot, struct PyrSymbol* value) noexcept { *slot = PyrSlot::make(value); }
inline void SetChar(PyrSlot* slot, char value) noexcept { *slot = PyrSlot::make(value); }
inline void SetPtr(PyrSlot* slot, void* ptr) noexcept { *slot = PyrSlot::make(ptr); }
inline void SetObjectOrNil(PyrSlot* slot, struct PyrObjectHdr* value) noexcept {
    *slot = value ? PyrSlot::make(value) : PyrSlot::make(PyrNil {});
}
inline void SetTrue(PyrSlot* slot) noexcept { *slot = PyrSlot::make(true); }
inline void SetFalse(PyrSlot* slot) noexcept { *slot = PyrSlot::make(false); }
inline void SetBool(PyrSlot* slot, bool value) noexcept { *slot = PyrSlot::make(value); }
inline void SetNil(PyrSlot* slot) noexcept { *slot = PyrSlot::make(PyrNil {}); }
inline void SetFloat(PyrSlot* slot, double value) noexcept { *slot = PyrSlot::make(value); }

template <typename numeric_type> [[nodiscard]] inline int slotVal(PyrSlot* slot, numeric_type* value) noexcept {
    static_assert(std::is_arithmetic_v<numeric_type>);
    if (slot->isDouble()) {
        *value = static_cast<numeric_type>(slot->getDouble());
        return errNone;
    } else if (slot->isInt()) {
        *value = static_cast<numeric_type>(slot->getInt());
        return errNone;
    }
    return errWrongType;
}

[[nodiscard]] inline int slotFloatVal(PyrSlot* slot, float* value) noexcept { return slotVal<float>(slot, value); }
[[nodiscard]] inline int slotIntVal(PyrSlot* slot, int32_t* value) noexcept { return slotVal<int32_t>(slot, value); }
[[nodiscard]] inline int slotDoubleVal(PyrSlot* slot, double* value) noexcept { return slotVal<double>(slot, value); }

[[nodiscard]] inline int slotSymbolVal(PyrSlot* slot, PyrSymbol** symbol) noexcept {
    if (!IsSym(slot))
        return errWrongType;
    *symbol = slot->getSymbol();
    return errNone;
}

[[nodiscard]] inline void* slotRawPtr(PyrSlot* slot) noexcept {
    assert(slot->isPtr());
    return slot->getPtr();
}

[[nodiscard]] inline struct PyrBlock* slotRawBlock(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrBlock>();
}
[[nodiscard]] inline struct PyrSymbolArray* slotRawSymbolArray(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrSymbolArray>();
}
[[nodiscard]] inline struct PyrDoubleArray* slotRawDoubleArray(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrDoubleArray>();
}
[[nodiscard]] inline struct PyrInt8Array* slotRawInt8Array(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrInt8Array>();
}
[[nodiscard]] inline struct PyrMethod* slotRawMethod(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrMethod>();
}
[[nodiscard]] inline struct PyrThread* slotRawThread(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrThread>();
}
[[nodiscard]] inline struct PyrString* slotRawString(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrString>();
}
[[nodiscard]] inline struct PyrList* slotRawList(const PyrSlot* slot) noexcept { return slot->getPyrObject<PyrList>(); }
[[nodiscard]] inline struct PyrFrame* slotRawFrame(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrFrame>();
}
[[nodiscard]] inline struct PyrClass* slotRawClass(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrClass>();
}
[[nodiscard]] inline struct PyrInterpreter* slotRawInterpreter(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrInterpreter>();
}
[[nodiscard]] inline struct PyrSymbol* slotRawSymbol(const PyrSlot* slot) noexcept { return slot->getSymbol(); }

[[nodiscard]] inline int32_t slotRawInt(const PyrSlot* slot) noexcept { return slot->getInt(); }
[[nodiscard]] inline uint8_t slotRawChar(const PyrSlot* slot) noexcept { return slot->getChar(); }
[[nodiscard]] inline double slotRawFloat(const PyrSlot* slot) noexcept { return slot->getDouble(); }
[[nodiscard]] inline struct PyrObject* slotRawObject(const PyrSlot* slot) noexcept {
    return slot->getPyrObject<PyrObject>();
}
[[nodiscard]] inline struct PyrObjectHdr* slotRawObjectHdr(const PyrSlot* slot) noexcept {
    return slot->getObjectHdr();
}

inline void slotCopy(PyrSlot* dst, const PyrSlot* src) noexcept { *dst = *src; }

inline void slotCopy(PyrSlot* dst, const PyrSlot* src, int num) noexcept { std::copy(src, src + num, dst); }

[[nodiscard]] inline bool SlotEq(const PyrSlot* a, const PyrSlot* b) noexcept { return *a == *b; }

template <typename T> inline void SetRaw(PyrSlot* slot, T t) { *slot = PyrSlot::make(t); }


extern PyrSlot o_nil, o_true, o_false, o_inf;
extern PyrSlot o_fhalf, o_fnegone, o_fzero, o_fone, o_ftwo;
extern PyrSlot o_negone, o_zero, o_one, o_two;
extern PyrSlot o_emptyarray, o_onenilarray, o_argnamethis;

extern PyrSymbol* s_object; // "Object"
extern PyrSymbol* s_this; // "this"
extern PyrSymbol* s_super; // "super"

void dumpPyrSlot(PyrSlot* slot);
void slotString(PyrSlot* slot, char* str);
void slotOneWord(PyrSlot* slot, char* str);
bool postString(PyrSlot* slot, char* str);
[[nodiscard]] const char* slotSymString(PyrSlot* slot);
[[nodiscard]] int asCompileString(PyrSlot* slot, char* str);

[[nodiscard]] int slotStrVal(PyrSlot* slot, char* str, int maxlen);
[[nodiscard]] std::tuple<int, std::string> slotStdStrVal(PyrSlot* slot);
[[nodiscard]] std::tuple<int, std::string> slotStrStdStrVal(PyrSlot* slot);
[[nodiscard]] int slotStrLen(PyrSlot* slot) noexcept;
[[nodiscard]] int slotPStrVal(PyrSlot* slot, unsigned char* str);

template <typename numeric_type> inline void setSlotVal(PyrSlot* slot, numeric_type value) noexcept;
template <> inline void setSlotVal<int>(PyrSlot* slot, int value) noexcept { SetInt(slot, value); }
template <> inline void setSlotVal<double>(PyrSlot* slot, double value) noexcept { SetFloat(slot, value); }

void PyrSlotTest();
