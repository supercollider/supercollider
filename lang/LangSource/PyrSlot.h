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

#pragma once

#include <functional>
#include <algorithm>
#include <string>
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <cstring>
#include <cmath>
#include "PyrObjectHdr.h"
#include "PyrErrors.h"
#include "function_attributes.h"
#include "Hash.h"
#include "PyrSymbol.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description of how nan-boxing and pointer tagging work in sc.                                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TLDR: SClang uses nan-boxing and requires all pointer to be in user space for a 64 bit slot.

//===========//
// Pointers  //
//===========//

// On 64-bit systems sc requires all pointers to fit into 48 bits, aka, to be in user space.
// On 32 bit systems the whole address space may be used.
// This means the top 16 bits are free to store data in.

// Technically it is possible to have a pointer that doesn't fit into this range, but these are usually reserved for use
// by the operating system kernel
//  --- terms to familiarize yourself with here are 'user' and 'kernel' space addresses along with 'virtual' address.
// Such large pointers only arise when explicitly passing a specific flag to mmap, so this shouldn't happen unless you
// are writing an os kernel in supercollider
//          ... let's assume this isn't the case...
// Here is a good article on the subject of storing extra data inside of pointers
// https://muxup.com/2023q4/storing-data-in-pointers.

// If someone finds a use-case where they need kernel-space addresses this could be added by tagging the lowest bit of
// the pointer and replacing the upper bits before returning from the slot, but until then, we don't support it.

// Note: this would be different across architectures as x86 uses sign extention on bit 47 but aarch64 doesn't
//   --- we've encountered bugs due to this in the past.
// See https://docs.kernel.org/arch/arm64/memory.html and compare with
// https://en.wikipedia.org/wiki/X86-64#Canonical_form_addresses for some more information on this.

// Layout of 64 bit pointer:
//        |-----------------| these are free
// Ptr  = 1111 1111 1111 1111 000000000000000000000000000000000000000000000000


//===========//
// Doubles   //
//===========//

// It is recommend to familiarize yourself with the double specification before reading this
// https://en.wikipedia.org/wiki/Double-precision_floating-point_format.

// Double spec:
//      1 sign bit, 11 exponents bits, 52 fraction bits.

// Layout of double when an inf and -inf:
//     sign bit
//        |  exponent                       fraction
//        |   11-bit                         52-bit
//        ||-----------| |---------------------------------------------------|
//  Inf = 0111 1111 1111 0000 000000000000000000000000000000000000000000000000 = 7FF0 0000 0000 0000
// -Inf = 1111 1111 1111 0000 000000000000000000000000000000000000000000000000 = FFF0 0000 0000 0000

// Layout of double when a nan:
//  If exponents are all 1 and fraction isn't 0, its some type of nan (if fraction is zero, it is an inf).
//  Signaling NaN (sNaN) is used to signal some floating point error has occurred.
//    These are not produced by supercollider.
//  Quiet NaN (qNaN) is normal nan. There are many types of qNaN.
//  Note how the top fraction bit is set here, but not when it is a sNaN.
//
//  sNaN    = 0111 1111 1111 0000 000000000000000000000000000000000000000000000001 = 7FF0 0000 0000 0001
//  qNaN    = 0111 1111 1111 1000 000000000000000000000000000000000000000000000000 = 7FF8 0000 0000 0000
//  qNaN    = 0111 1111 1111 1000 000000000000000000000000000000000000000000000001 = 7FF8 0000 0000 0001
//  qNaN    = 0111 1111 1111 1000 000000000000000000000000000000000000000000000010 = 7FF8 0000 0000 0002
//
//            | sign is irrelevant
//  qNaN    = 1111 1111 1111 1000 000000000000000000000000000000000000000000000001 = FFF8 0000 0000 0001
//  qNaN    = 1111 1111 1111 1111 111111111111111111111111111111111111111111111111 = FFFF FFFF FFFF FFFF
//          .... Still a  qNaN
//

// Quiet nans can be produced through std::nan("0"), std::nan("1").

// By limiting ourselves to only one types of nan, the result of std:nan("0") (7FF8000000000000), we can use the other
// values to store other data types in. That is the basic idea of nan-boxing.

//========================//
// Doubles and Pointers   //
//========================//

// So this means the free bits in each are...
// 0 = used, 1 = free bit
// qNan = 1000 0000 0000 0111 111111111111111111111111111111111111111111111111
// Ptr  = 1111 1111 1111 1111 000000000000000000000000000000000000000000000000

// This leaves these 4 bits in which a tag can be stored.
// 1000 0000 0000 0111 000000000000000000000000000000000000000000000000

// For the value of the tags, see details::Tags below.


#ifdef __SIZEOF_POINTER__
#    define SIZEOF_POINTER __SIZEOF_POINTER__
#elif defined(__x86_64__) || defined(__aarch64__) || defined(_WIN64)
#    define SIZEOF_POINTER 8
#else
#    define SIZEOF_POINTER 4
#endif

// verify the pointer size
static_assert(SIZEOF_POINTER == sizeof(void*), "unexpected pointer size");

// The slot is 64 bits, on 32 bit systems, we must pad the data.
#define POINTER_NEEDS_PADDING (SIZEOF_POINTER != 8)
namespace details {
static constexpr bool pointerNeedsPadding = POINTER_NEEDS_PADDING;
}


// https://stackoverflow.com/questions/60802864/emulating-gccs-builtin-unreachable-in-visual-studio
#ifdef __GNUC__ // GCC 4.8+, Clang, Intel and other compilers compatible with GCC
[[noreturn]] inline __attribute__((always_inline)) void unreachable() { __builtin_unreachable(); }
#elif defined(_MSC_VER) // MSVC
[[noreturn]] __forceinline void unreachable() { __assume(false); }
#else // ???
inline void unreachable() {}
#endif


// This is used as a non-type template parameter to check for nans when creating slots of doubles.
enum struct AssertDouble { Okay, CouldBeBadNan };

[[nodiscard]] inline double removeBadNans(double d) noexcept { return std::isnan(d) ? std::nan("0") : d; }
[[nodiscard]] inline float removeBadNans(float d) noexcept { return std::isnan(d) ? std::nanf("0") : d; }

namespace details {
// cpp reference
template <class To, class From>
std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>,
                 To> inline bit_cast(const From& src) noexcept {
    static_assert(std::is_trivially_constructible_v<To>,
                  "This implementation additionally requires "
                  "destination type to be trivially constructible");
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

struct Masks {
    // Note, nanExponent is the only nan we can store inside the slot.
    static constexpr uint64_t nanExponent = 0x7FF8000000000000;
    static constexpr uint64_t boxedBit = 0x0001000000000000;
    static constexpr uint64_t boxed = boxedBit | nanExponent;
    // This is the valid user space pointer mask.
    static constexpr uint64_t pointer = 0x0000FFFFFFFFFFFF;
};


struct Tags {
    // These tags include the boxed mask, which makes it easier to test if they are active.
    // The double is without a tag.
    static constexpr uint64_t objHdrTag = 0xFFFF000000000000;
    static constexpr uint64_t intTag = 0x7FFB000000000000;
    static constexpr uint64_t symTag = 0x7FFD000000000000;
    static constexpr uint64_t charTag = 0x7FFF000000000000;
    static constexpr uint64_t trueTag = 0xFFF9000000000000;
    static constexpr uint64_t ptrTag = 0xFFFB000000000000;
    static constexpr uint64_t nilTag = 0x7FF9000000000000;
    static constexpr uint64_t falseTag = 0xFFFD000000000000;
};


// Pad any type less than 6 bytes to 8 bytes by inserting zeros after. Endianness doesn't matter here.
template <typename T> struct PadValueTo64Bits {
    static_assert(std::is_default_constructible_v<T>);
    static_assert(sizeof(T) <= 6);
    static constexpr size_t numPadding = 8 - sizeof(T);

    constexpr explicit PadValueTo64Bits(T v) noexcept: value(v) {}
    constexpr PadValueTo64Bits() = default;

    ~PadValueTo64Bits() noexcept = default;

    T value;
    uint8_t paddingForValue[numPadding] = { 0 };
};

static_assert(sizeof(PadValueTo64Bits<uint32_t>) == 8);
static_assert(sizeof(PadValueTo64Bits<uint16_t>) == 8);
static_assert(sizeof(PadValueTo64Bits<uint8_t>) == 8);
static_assert(sizeof(PadValueTo64Bits<char>) == 8);

// A wrapper around T (which is a pointer) that pads the value with known zeros if needed.
// On a 32bit system, the slot has 32bits left over.
template <typename T> struct MaybePadPointerTo64Bits {
    static_assert(std::is_pointer_v<T>);

#if POINTER_NEEDS_PADDING && (BYTE_ORDER == BIG_ENDIAN)
    int32_t padding;
#endif

    T ptr;

#if POINTER_NEEDS_PADDING && (BYTE_ORDER == LITTLE_ENDIAN)
    int32_t padding;
#endif

    [[nodiscard]] int32 getPtrAsInt32() const noexcept { return static_cast<int32>(reinterpret_cast<uintptr_t>(ptr)); }
    [[nodiscard]] T getPtr() const noexcept {
        if constexpr (pointerNeedsPadding) {
            // There is no need to mask when the pointer is padded as the mask doesn't touch the data.
            return ptr;
        } else {
            const auto r = reinterpret_cast<uintptr_t>(ptr);
            return reinterpret_cast<T>(r & Masks::pointer);
        }
    }
};


static_assert(sizeof(MaybePadPointerTo64Bits<void*>) == sizeof(double));
}

// This is the old tag and their values are assumed and used as indices elsewhere in the code, do not change them
// without refactoring the rest of the code base!
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

// A Tag used to construct a slot containing nil
struct PyrNil {};

union PyrSlot {
private:
    using Tags = details::Tags;
    using Masks = details::Masks;
    template <typename T> using PadValueTo64Bits = details::PadValueTo64Bits<T>;
    template <typename T> using MaybePadPointerTo64Bits = details::MaybePadPointerTo64Bits<T>;

    struct PrivateTag {};
    PyrSlot(PrivateTag, uint64_t raw) noexcept: u_raw(raw) {}
    PyrSlot(PrivateTag, uint64_t tag, uint64_t raw) noexcept: u_raw(tag | raw) {}
    /// Requires a valid double or the safe nan value.
    PyrSlot(PrivateTag, double d) noexcept: u_double(d) {
        assert([&]() -> bool {
            if (std::isnan(d)) {
                const auto bits = details::bit_cast<uint64_t>(d);
                const auto t = PyrSlot(PrivateTag {}, bits);
                return t.isDouble();
            } else
                return true;
        }());
    }

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
        return static_cast<uint16_t>(t >> (64ULL - 16ULL));
    }
    [[nodiscard]] inline bool isBoxed() const noexcept { return bitsAreSet(u_raw, Masks::boxed); }
    template <uint64_t T> [[nodiscard]] inline bool tagChecker() const noexcept {
        return getTagAsU16(u_raw) == getTagAsU16(T);
    }

    // Because doubles have odd rules we need to process them differently.
    // Try to avoid introducing any more types with strange comparison rules.
    template <template <typename> typename OP>
    [[nodiscard]] static decltype(auto) apply_binary_op(PyrSlot a, PyrSlot b) noexcept {
        return a.isDouble() && b.isDouble() ? OP {}(a.getDouble(), b.getDouble()) : OP {}(a.u_raw, b.u_raw);
    }

public:
    PyrSlot() noexcept: u_raw(Tags::nilTag) {}
    ~PyrSlot() noexcept = default;
    PyrSlot(PyrSlot&&) noexcept = default;
    PyrSlot(const PyrSlot&) noexcept = default;
    PyrSlot& operator=(PyrSlot&&) noexcept = default;
    PyrSlot& operator=(const PyrSlot&) noexcept = default;

    // This is identity, not equality in supercollider.
    [[nodiscard]] friend inline bool operator==(PyrSlot lhs, PyrSlot rhs) noexcept {
        return PyrSlot::apply_binary_op<std::equal_to>(lhs, rhs);
    }
    [[nodiscard]] friend inline bool operator!=(PyrSlot lhs, PyrSlot rhs) noexcept {
        return PyrSlot::apply_binary_op<std::not_equal_to>(lhs, rhs);
    }

    // NOTE: these comparisons do NOT compare the value in the slot.
    // They are used for sorting and ordering slots.
    [[nodiscard]] friend inline bool operator<(PyrSlot lhs, PyrSlot rhs) noexcept {
        return PyrSlot::apply_binary_op<std::less>(lhs, rhs);
    }
    [[nodiscard]] friend inline bool operator<=(PyrSlot lhs, PyrSlot rhs) noexcept {
        return PyrSlot::apply_binary_op<std::less_equal>(lhs, rhs);
    }
    [[nodiscard]] friend inline bool operator>(PyrSlot lhs, PyrSlot rhs) noexcept {
        return PyrSlot::apply_binary_op<std::greater>(lhs, rhs);
    }
    [[nodiscard]] friend inline bool operator>=(PyrSlot lhs, PyrSlot rhs) noexcept {
        return PyrSlot::apply_binary_op<std::greater_equal>(lhs, rhs);
    }

    template <AssertDouble Check = AssertDouble::Okay> [[nodiscard]] inline static PyrSlot make(double d) noexcept {
        // There are many safe nan values, but a few are not safe.
        // The server's Convolution3 is a known source of unsafe nans.
        // Supercollider VM itself does not generate unsafe nans.
        if constexpr (Check == AssertDouble::CouldBeBadNan) {
            return { PrivateTag(), removeBadNans(d) };
        } else {
            return { PrivateTag(), d };
        }
    }
    [[nodiscard]] inline static PyrSlot make(char c) noexcept {
        return { PrivateTag(), Tags::charTag, static_cast<uint64_t>(details::bit_cast<uint8_t>(c)) };
    }

    [[nodiscard]] inline static PyrSlot make(PyrObjectHdr* o) noexcept {
        return { PrivateTag(), Tags::objHdrTag, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(o)) };
    }
    [[nodiscard]] inline static PyrSlot make(PyrObjectHdr& o) noexcept {
        return { PrivateTag(), Tags::objHdrTag, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&o)) };
    }

    [[nodiscard]] inline static PyrSlot make(PyrSymbol* o) noexcept {
        return { PrivateTag(), Tags::symTag, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(o)) };
    }
    [[nodiscard]] inline static PyrSlot make(PyrSymbol& o) noexcept {
        return { PrivateTag(), Tags::symTag, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&o)) };
    }

    [[nodiscard]] inline static PyrSlot make(int32_t i) noexcept {
        return { PrivateTag(), Tags::intTag, static_cast<uint64_t>(details::bit_cast<uint32_t>(i)) };
    }
    [[nodiscard]] inline static PyrSlot make(void* o) noexcept {
        static_assert(sizeof(uintptr_t) <= sizeof(uint64_t)); // probably always true?
        return { PrivateTag(), Tags::ptrTag, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(o)) };
    }
    [[nodiscard]] inline static PyrSlot make() noexcept { return {}; }
    [[nodiscard]] inline static PyrSlot make(PyrNil) noexcept { return {}; }
    [[nodiscard]] inline static PyrSlot make(bool b) noexcept {
        return { PrivateTag(), b ? Tags::trueTag : Tags::falseTag };
    }

    // This is a bit hard to read (SFINAE), but basically says 'If you aren't derived from PyrObjectHdr, we will not do
    // any casts for you'. One day, when we upgrade to c++ 20, this can be replaced by a simple concept.
    // This also means we will not do the implicit casting to void* if you give it a pointer, this is good, because
    // thats a very powerful overload which you should only use if you really intend to.
    template <typename T,
              std::enable_if_t<!std::is_base_of_v<PyrObjectHdr, typename std::remove_pointer<T>::type>, bool> = true>
    [[nodiscard]] static PyrSlot make(T) = delete;

    [[nodiscard]] bool inline isDouble() const noexcept { return !isBoxed(); }
    [[nodiscard]] inline bool isChar() const noexcept { return tagChecker<Tags::charTag>(); }
    [[nodiscard]] inline bool isInt() const noexcept { return tagChecker<Tags::intTag>(); }
    [[nodiscard]] inline bool isPtr() const noexcept { return tagChecker<Tags::ptrTag>(); }
    [[nodiscard]] inline bool isObjectHdr() const noexcept { return tagChecker<Tags::objHdrTag>(); }
    [[nodiscard]] inline bool isSymbol() const noexcept { return tagChecker<Tags::symTag>(); }
    [[nodiscard]] inline bool isNil() const noexcept { return u_raw == Tags::nilTag; }
    [[nodiscard]] inline bool isTrue() const noexcept { return u_raw == Tags::trueTag; }
    [[nodiscard]] inline bool isFalse() const noexcept { return u_raw == Tags::falseTag; }

    [[nodiscard]] inline double getDouble() const noexcept {
        assert(isDouble());
        return u_double;
    }
    [[nodiscard]] inline char getChar() const noexcept {
        assert(isChar());
        return u_char.value;
    }
    [[nodiscard]] inline int32_t getInt() const noexcept {
        assert(isInt());
        return u_int.value;
    }
    [[nodiscard]] inline void* getPtr() const noexcept {
        if (isPtr())
            return u_ptr.getPtr();
        assert(isNil() || (isInt() && getInt() == 0));
        return nullptr;
    }
    [[nodiscard]] inline struct PyrObjectHdr* getObjectHdr() const noexcept {
        assert(isObjectHdr());
        return u_objectHeader.getPtr();
    }
    [[nodiscard]] inline struct PyrSymbol* getSymbol() const noexcept {
        if (isSymbol())
            return u_symbol.getPtr();
        assert(isNil() || (isInt() && getInt() == 0) || (isDouble() && getDouble() == 0));
        return nullptr;
    }
    template <typename T> [[nodiscard]] inline T* getPyrObjType() const noexcept {
        // types are incomplete here so can't check...
        // static_assert(std::is_base_of<PyrObjectHdr, T>::value, "Type must derive from PyrObjectHeader");
        assert(isBoxed());
        if (isObjectHdr())
            return reinterpret_cast<T*>(u_objectHeader.getPtr());
        // Previously, these values have all been used to mean nullptr. This is quite confusing, but would be a large
        // breaking change that affected the langauge, so they remain.
        assert(isNil() || (isInt() && getInt() == 0) || (isDouble() && getDouble() == 0));
        return nullptr;
    }

    [[nodiscard]] HOT int getTag() const noexcept {
        if (isDouble()) {
            return tagFloat;
        } else {
            switch (getTagAsU16(u_raw)) {
            case getTagAsU16(Tags::nilTag):
                return tagNil;
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
            case getTagAsU16(Tags::trueTag):
                return tagTrue;
            case getTagAsU16(Tags::falseTag):
                return tagFalse;
            default:
                unreachable();
            }
        }
    }

    // getClass is implemented over in PyrObject.h as it needs access to gTagClassTable.
    inline struct PyrClass* getClass();

    [[nodiscard]] inline int32_t hash() const noexcept {
        switch (getTagAsU16(u_raw)) {
        case getTagAsU16(Tags::symTag):
            return static_cast<int32>(getSymbol()->hash);
        case getTagAsU16(Tags::intTag):
            return Hash(getInt());
        case getTagAsU16(Tags::nilTag):
            return static_cast<int32>(0xA5A5A5A5);
        case getTagAsU16(Tags::charTag):
            return Hash(u_char.value & 255);
        case getTagAsU16(Tags::objHdrTag):
            return Hash(u_objectHeader.getPtrAsInt32());
        case getTagAsU16(Tags::ptrTag):
            return Hash(u_ptr.getPtrAsInt32());
        case getTagAsU16(Tags::trueTag):
            return 0x69696969;
        case getTagAsU16(Tags::falseTag):
            return 0x55AA55AA;
        default:
            union {
                int32 i[2];
                double d;
            } u;
            u.d = u_double;
            return Hash(u.i[0] + Hash(u.i[1]));
        }
    }
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

[[nodiscard]] inline int32 calcHash(const PyrSlot* slot) noexcept { return slot->hash(); }

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

template <AssertDouble Check = AssertDouble::Okay> inline void SetFloat(PyrSlot* slot, double value) noexcept {
    *slot = PyrSlot::make<Check>(value);
}

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

[[nodiscard]] inline void* slotRawPtr(PyrSlot* slot) noexcept { return slot->getPtr(); }

[[nodiscard]] inline struct PyrBlock* slotRawBlock(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrBlock>();
}
[[nodiscard]] inline struct PyrSymbolArray* slotRawSymbolArray(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrSymbolArray>();
}
[[nodiscard]] inline struct PyrFloatArray* slotRawFloatArray(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrFloatArray>();
}
[[nodiscard]] inline struct PyrDoubleArray* slotRawDoubleArray(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrDoubleArray>();
}
[[nodiscard]] inline struct PyrInt8Array* slotRawInt8Array(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrInt8Array>();
}
[[nodiscard]] inline struct PyrMethod* slotRawMethod(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrMethod>();
}
[[nodiscard]] inline struct PyrThread* slotRawThread(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrThread>();
}
[[nodiscard]] inline struct PyrString* slotRawString(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrString>();
}
[[nodiscard]] inline struct PyrList* slotRawList(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrList>();
}
[[nodiscard]] inline struct PyrFrame* slotRawFrame(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrFrame>();
}
[[nodiscard]] inline struct PyrClass* slotRawClass(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrClass>();
}
[[nodiscard]] inline struct PyrInterpreter* slotRawInterpreter(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrInterpreter>();
}
[[nodiscard]] inline struct PyrSymbol* slotRawSymbol(const PyrSlot* slot) noexcept { return slot->getSymbol(); }

[[nodiscard]] inline int32_t slotRawInt(const PyrSlot* slot) noexcept { return slot->getInt(); }
[[nodiscard]] inline uint8_t slotRawChar(const PyrSlot* slot) noexcept { return slot->getChar(); }
[[nodiscard]] inline double slotRawFloat(const PyrSlot* slot) noexcept { return slot->getDouble(); }
[[nodiscard]] inline struct PyrObject* slotRawObject(const PyrSlot* slot) noexcept {
    return slot->getPyrObjType<PyrObject>();
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

extern PyrSymbol* s_abstract_object; // "AbstractObjectExperimental"
extern PyrSymbol* s_object; // "Object"
extern PyrSymbol* s_this; // "this"
extern PyrSymbol* s_super; // "super"

void dumpPyrSlot(PyrSlot* slot);
void slotString(PyrSlot* slot, char* str);
void slotOneWord(PyrSlot* slot, char* str);
bool postString(PyrSlot* slot, char* str);
[[nodiscard]] const char* slotSymString(PyrSlot* slot);
[[nodiscard]] int asCompileString(PyrSlot* slot, char* str);

[[nodiscard]] int slotStrVal(PyrSlot* slot, char* str, size_t maxlen);
[[nodiscard]] std::tuple<int, std::string> slotStdStrVal(PyrSlot* slot);
[[nodiscard]] std::tuple<int, std::string> slotStrStdStrVal(PyrSlot* slot);
[[nodiscard]] int slotStrLen(PyrSlot* slot) noexcept;
[[nodiscard]] int slotPStrVal(PyrSlot* slot, unsigned char* str);

template <typename numeric_type> inline void setSlotVal(PyrSlot* slot, numeric_type value) noexcept;
template <> inline void setSlotVal<int>(PyrSlot* slot, int value) noexcept { SetInt(slot, value); }
template <> inline void setSlotVal<double>(PyrSlot* slot, double value) noexcept { SetFloat(slot, value); }
