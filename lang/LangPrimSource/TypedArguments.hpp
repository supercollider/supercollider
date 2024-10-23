// Created by jordan on 12/06/24.

#pragma once

#include <optional>
#include <array>
#include <variant>
#include <cstring>
#include "PyrSlot.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "PyrSymbol.h"
#include "VMGlobals.h"
#include "SCBase.h"


struct RawSlot {
    static constexpr bool isNode = true;
    using ReturnT = PyrSlot*;
    static void printErrorName() {} // cannot error
    static constexpr uint8 check(PyrSlot*) { return 1; }
    static ReturnT asReturn(PyrSlot* slot, uint8 checkResult) { return slot; }
};

struct FloatChecker {
    static constexpr bool isNode = true;
    using ReturnT = double;
    static void printErrorName() { post("Float "); }
    static uint8 check(PyrSlot* slot) { return IsFloat(slot) ? 1 : 0; }
    static ReturnT asReturn(PyrSlot* slot, uint8) { return slotRawFloat(slot); }
};

struct IntChecker {
    static constexpr bool isNode = true;
    using ReturnT = int;
    static void printErrorName() { post("Int "); }
    static uint8 check(PyrSlot* slot) { return IsInt(slot) ? 1 : 0; }
    static ReturnT asReturn(PyrSlot* slot, uint8) { return slotRawInt(slot); }
};

struct SymbolChecker {
    static constexpr bool isNode = true;
    using ReturnT = PyrSymbol*;
    static void printErrorName() { post("Symbol "); }
    static uint8 check(PyrSlot* slot) { return IsSym(slot) ? 1 : 0; }
    static ReturnT asReturn(PyrSlot* slot, uint8 checkResult) { return slotRawSymbol(slot); }
};

struct ArrayChecker {
    static constexpr bool isNode = true;
    using ReturnT = PyrObject*;
    static void printErrorName() { post("Array "); }
    static uint8 check(PyrSlot* slot) { return (IsObj(slot) && classOfSlot(slot) == class_array) ? 1 : 0; }
    static ReturnT asReturn(PyrSlot* slot, uint8 checkResult) { return slotRawObject(slot); }
};

struct NilChecker {
    static constexpr bool isNode = true;
    using ReturnT = struct { uint8 empty; };
    static void printErrorName() { post("Nil "); }
    static uint8 check(PyrSlot* slot) { return IsNil(slot) ? 1 : 0; }
    static ReturnT asReturn(PyrSlot* slot, uint8 checkResult) { return {}; }
};

namespace sc::priv {
template <typename...> static constexpr auto is_unique = std::true_type {};

template <typename T, typename... Rest>
static constexpr auto is_unique<T, Rest...> =
    std::bool_constant<(!std::is_same_v<T, Rest> && ...) && is_unique<Rest...>> {};
}

template <typename... Checkers> struct SlotContentVariant {
    static_assert(sc::priv::is_unique<Checkers...>, "SlotContentVariant's children must be unique");

    static_assert(sizeof...(Checkers) < 256);
    static_assert(((sizeof(typename Checkers::ReturnT) <= 8) && ...));
    static_assert((Checkers::isNode && ...));

    using tupleReturns = std::tuple<typename Checkers::ReturnT...>;
    using tupleCheckers = std::tuple<Checkers...>;

    template <uint8 I> static SlotContentVariant make(std::tuple_element_t<I, tupleReturns> data) {
        using D = std::tuple_element_t<I, tupleReturns>;
        SlotContentVariant out(0, I);
        std::memcpy(reinterpret_cast<D*>(&out.data), &data, sizeof(D));
        return out;
    }

    static SlotContentVariant createUninitialized() { return { 0, 0 }; }

    template <typename F> void visit(F&& f) {
        static constexpr auto indices = std::make_index_sequence<sizeof...(Checkers)>();
        auto&& func = std::forward<F>(f);
        visitImpl(func, indices);
    }

    template <uint8 I> auto force() {
        using D = std::tuple_element_t<I, tupleReturns>;
        return *reinterpret_cast<D*>(&data);
    }
    template <typename T> auto force() {
        static_assert((std::is_same_v<T, Checkers> || ...));
        return force<indexOfType<T>()>();
    }

    template <typename T> bool holds() {
        static_assert((std::is_same_v<T, Checkers> || ...));
        return index == indexOfType<T>();
    }

    template <typename T> std::optional<typename T::ReturnT> tryHolds() {
        static_assert((std::is_same_v<T, Checkers> || ...));
        if (index == indexOfType<T>()) {
            return { force<indexOfType<T>()>() };
        } else {
            return { std::nullopt };
        }
    }

    SlotContentVariant() = delete;
    SlotContentVariant(SlotContentVariant&&) = default;
    SlotContentVariant(const SlotContentVariant&) = default;
    SlotContentVariant& operator=(SlotContentVariant&&) = default;
    SlotContentVariant& operator=(const SlotContentVariant&) = default;

    template <typename T> static constexpr int32_t indexOfType() {
        constexpr int32_t out = SlotContentVariant::indexOfTypeImpl<T>(std::make_index_sequence<sizeof...(Checkers)>());
        return out;
    }

private:
    SlotContentVariant(uint64 d, uint8 i): data(d), index(i) {}
    uint64 data;
    uint8_t index;

    template <typename Type, typename I, I... ints>
    static constexpr size_t indexOfTypeImpl(std::integer_sequence<I, ints...>) {
        return (SlotContentVariant::indexOfTypeForEachImpl<Type, ints>() + ...);
    }
    template <typename Type, size_t I> static constexpr size_t indexOfTypeForEachImpl() {
        if constexpr (std::is_same<Type, typename std::tuple_element<I, tupleCheckers>::type>::value) {
            return I;
        } else {
            return 0;
        }
    }

    template <typename F, typename T, T... ints> void visitImpl(F& f, std::integer_sequence<T, ints...>) {
        (SlotContentVariant::visitEachImpl<ints>(f) || ...);
    }
    template <uint8 I, typename F> bool visitEachImpl(F& f) {
        if (index == I) {
            f(std::tuple_element_t<I, tupleCheckers> {}, force<I>());
            return true;
        } else {
            return false;
        }
    }
};

template <typename Checker> struct OrNil {
    static_assert(Checker::isNode);
    static constexpr bool isNode = false;
    using ReturnT = std::optional<typename Checker::ReturnT>;
    static void printErrorName() {
        post("Nil or ");
        Checker::printErrorName();
    }
    static uint8 check(PyrSlot* slot) { return IsNil(slot) ? 2 : Checker::check(slot); }
    static ReturnT asReturn(PyrSlot* slot, uint8 checkResult) {
        if (checkResult == 2)
            return std::nullopt;
        return ReturnT(Checker::asReturn(slot, 1));
    }
};

template <typename... Checkers> struct OrChecker {
    static_assert(sc::priv::is_unique<Checkers...>, "OrChecker's children must be unique");
    static_assert((Checkers::isNode && ...));
    static constexpr bool isNode = false;

    using ReturnT = SlotContentVariant<Checkers...>;

    static void printErrorName() {
        post("any of( ");
        (Checkers::printErrorName(), ...);
        post(") ");
    }

    static uint8 check(PyrSlot* slot) {
        static constexpr auto indices = std::make_index_sequence<sizeof...(Checkers)>();
        return OrChecker::checkImpl(slot, indices);
    }

    static ReturnT asReturn(PyrSlot* slot, uint8 checkResult) {
        ReturnT out = ReturnT::createUninitialized();
        OrChecker::asReturnImpl(slot, checkResult, std::make_index_sequence<sizeof...(Checkers)>(), out);
        return out;
    }

private:
    template <typename T, T... ints> static uint8 checkImpl(PyrSlot* slot, std::integer_sequence<T, ints...>) {
        uint8 out;
        (OrChecker::checkImplSingle<Checkers, ints>(slot, &out) || ...);
        return out;
    }

    template <typename C, size_t I> static uint8 checkImplSingle(PyrSlot* slot, uint8* out) {
        const auto r = C::check(slot);
        if (r > 0) {
            *out = static_cast<uint8>(I + 1);
            return true;
        }
        return false;
    }


    template <typename T, T... ints>
    static void asReturnImpl(PyrSlot* slot, uint8 checkResult, std::integer_sequence<T, ints...>, ReturnT& out) {
        // or short circuits to the first
        (OrChecker::asReturnForEach<Checkers, ints>(slot, checkResult, out) || ...);
    }

    template <typename C, size_t I> static bool asReturnForEach(PyrSlot* slot, uint8 checkResult, ReturnT& result) {
        if (checkResult - 1 == I) {
            result = ReturnT::template make<I>(C::asReturn(slot, 1));
            return true;
        }
        return false;
    }
};


template <typename... Checkers> class TypedArgsFromStack {
    static constexpr size_t N = sizeof...(Checkers);
    static constexpr auto indices = std::make_index_sequence<N>();
    using ReturnT = std::tuple<typename Checkers::ReturnT...>;

public:
    static std::optional<ReturnT> get(VMGlobals* g, const char* primitiveName) {
        const std::array<PyrSlot*, N> uncheckedArray = TypedArgsFromStack::getArray(indices, g);
        const auto results = TypedArgsFromStack::checkAll(uncheckedArray, indices);

        if (std::all_of(results.begin(), results.end(), [](uint8 c) { return c != 0; }))
            return TypedArgsFromStack::getRet(uncheckedArray, indices, results);

        post("Wrong argument type in primitive call to %s.\n", primitiveName);
        TypedArgsFromStack::printErrors(uncheckedArray, indices, results);

        return std::nullopt;
    }


private:
    template <typename T, T... ints>
    static ReturnT getRet(const std::array<PyrSlot*, N> array, std::integer_sequence<T, ints...>,
                          const std::array<uint8, N> res) {
        return { Checkers::asReturn(array[ints], res[ints])... };
    }

    template <typename T, T... ints>
    static std::array<PyrSlot*, N> getArray(std::integer_sequence<T, ints...>, VMGlobals* g) {
        // Supercollider's stack points (sp) points to the last element, not past it.
        // Add one to compensate.
        return { (g->sp - (N - (ints + 1)))... };
    }

    template <typename T, T... ints>
    static std::array<uint8, N> checkAll(const std::array<PyrSlot*, N>& array, std::integer_sequence<T, ints...>) {
        return { Checkers::check(array[ints])... };
    }

    template <typename T, T... ints>
    static void printErrors(const std::array<PyrSlot*, N>& array, std::integer_sequence<T, ints...>,
                            const std::array<uint8, N> results) {
        (TypedArgsFromStack::printIfError<Checkers, ints>(array[ints], results[ints]), ...);
    }

    template <typename C, size_t I> static void printIfError(PyrSlot* slot, uint8 result) {
        if (result > 0)
            return;

        post("\tArgument %d expected a ", I);
        C::printErrorName();
        post("but received a ");

        if (IsObj(slot)) {
            post("%s ", slotRawSymbol(&slotRawObject(slot)->classptr->name)->name);
        } else {
            switch (slot->tag) {
            case tagNotInitialized:
                post("Uninitialized Slot");
                break;
            case tagInt:
                post("Int");
                break;
            case tagSym:
                post("Symbol");
                break;
            case tagChar:
                post("Char");
                break;
            case tagNil:
                post("Nil");
                break;
            case tagFalse:
                [[fallthrough]];
            case tagTrue:
                post("Boolean");
                break;
            case tagPtr:
                post("Pointer");
                break;
            case tagFloat:
                post("Float");
                break;
            case tagUnused:
                post("Unused tag (please report this error on github)");
            }
        }

        char str[128];
        slotString(slot, str);
        post(" with value %s.\n", str);
    }
};


// Copied from cpp reference
// helper type for the visitor #4
template <class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;
