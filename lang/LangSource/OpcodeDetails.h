#pragma once
#include "ByteCodeArray.h"
#include <cassert>
#include <tuple>
#include <cstdint>
#include <cstring>
#include <tuple>


namespace Opcode::details {
template <typename I> constexpr static Byte to_byte(I i) {
    Byte to;
    std::memcpy(&to, &i, sizeof(Byte));
    return to;
}

template <typename ENUM_T, typename I> constexpr static ENUM_T to_enum(I i) {
    assert(i >= 0);
    assert(i < (Byte)ENUM_T::COUNT);
    return static_cast<ENUM_T>((Byte)i);
}

template <Byte CODE, typename... OPERANDS> struct SimpleOpSpec {
    using Tuple = std::tuple<OPERANDS...>;

    const char* name;
    Byte code { CODE };
    unsigned int byteSize = sizeof...(OPERANDS) + sizeof(Byte); // Also include the code

    // This method is designed to be called from the interpreter, where PRE INCREMENT is needed.
    // destructure, uses POST INCREMENT.
    Tuple pullOperandsFromInstructions(unsigned char*& ip) const { return { OPERANDS::fromRaw(*(++ip))... }; }

    void emit(LocationInSourceCode loc, OPERANDS... operands) const {
        gCompilingBytecodes.emit(loc, code, operands...);
    }

    template <typename Action> void destructure(Action& action, unsigned char*& ip) const {
        const auto c = ip[0];
        ip += 1;
        assert(c == code);
        const auto getNextIp = [&]() {
            const auto out = ip[0];
            ip += 1;
            return out;
        };
        // We must create a tuple, because argument ordering isn't defined, but brace constructors are (regardless
        // whether you do a left or right fold)... c++.
        std::apply([&](auto... args) { action(name, args...); }, Tuple { OPERANDS::fromRaw(getNextIp())... });
    }

    // Emits the code with a prefix, used in the extended opcodes.
protected:
    void emitPrefix(LocationInSourceCode loc, Byte prefix, OPERANDS... operands) const {
        gCompilingBytecodes.emit(loc, prefix, code, operands...);
    }
};

template <Byte PREFIX, typename OPSPEC> struct WithPrefix : public OPSPEC {
    constexpr WithPrefix() = delete;

    unsigned int byteSize = 1U + OPSPEC::byteSize;

    template <typename... OPERANDS> void emit(LocationInSourceCode loc, OPERANDS... operands) const {
        OPSPEC::emitPrefix(loc, PREFIX, operands...);
    }

    template <typename Action> void destructure(Action& action, unsigned char*& ip) const {
        ++ip; // remove the prefix.
        OPSPEC::destructure(action, ip);
    }
};


template <Byte STARTCODE, Byte ENDCODE, typename... OPERANDS> struct SecondNibbleOpSpec {
    static constexpr auto startCode { STARTCODE };
    static constexpr auto endCode { ENDCODE };
    static constexpr auto operandCount { sizeof...(OPERANDS) };

    template <int i> static constexpr auto codeOffset() {
        static_assert(startCode + i < endCode);
        return startCode + i;
    }

    using Tuple = std::tuple<Byte, OPERANDS...>;

    static constexpr bool validNibble(unsigned int nibble) { return startCode + nibble < endCode; }

    const char* name;
    const char* secondNibbleDescription = "";

    unsigned int byteSize = sizeof...(OPERANDS) + 1U;

    void emit(LocationInSourceCode loc, unsigned int nibble, OPERANDS... operands) const {
        const Byte bytecode = startCode + nibble;
        assert(bytecode < endCode);
        gCompilingBytecodes.emit(loc, bytecode, operands...);
    }


    Tuple pullOperandsFromInstructions(unsigned char*& ip) const {
        // increment instruction pointer and get the values for each operand.
        return { (*ip) - startCode, OPERANDS::fromRaw(*(++ip))... };
    }

    template <typename Action> void destructure(Action& action, unsigned char*& ip) const {
        const auto c = ip[0] - startCode;
        ip += 1;
        assert(validNibble(c));
        const auto getNextIp = [&]() {
            const auto out = ip[0];
            ip += 1;
            return out;
        };

        std::apply(
            [&](auto... args) {
                action(std::tuple { name, secondNibbleDescription, c }, args...);
            },
            std::tuple<OPERANDS...> { OPERANDS::fromRaw(getNextIp())... });
    }

protected:
    void emitPrefix(LocationInSourceCode loc, Byte prefix, unsigned int nibble, OPERANDS... operands) const {
        const Byte bytecode = startCode + nibble;
        assert(bytecode < endCode);
        gCompilingBytecodes.emit(loc, prefix, bytecode, operands...);
    }
};
template <Byte STARTCODE, Byte ENDCODE, typename... OPERANDS> struct SecondNibbleNonZeroOpSpec {
    static constexpr auto startCode { STARTCODE };
    static constexpr auto endCode { ENDCODE };
    static constexpr auto operandCount { sizeof...(OPERANDS) };

    template <int i> static constexpr auto codeOffset() {
        static_assert(startCode + i < endCode);
        return startCode + i;
    }

    using Tuple = std::tuple<Byte, OPERANDS...>;

    const char* name;
    unsigned int byteSize = sizeof...(OPERANDS) + 1U;

    static constexpr bool validNibble(unsigned int nibble) { return startCode + (nibble - 1) < endCode; }

    void emit(LocationInSourceCode loc, unsigned int nibble, OPERANDS... operands) const {
        const Byte bytecode = startCode + (nibble - 1);
        assert(bytecode < endCode);
        assert(nibble != 0);

        gCompilingBytecodes.emit(loc, bytecode, operands...);
    }

    Tuple pullOperandsFromInstructions(unsigned char*& ip) const {
        // increment instruction pointer and get the values for each operand.
        return { (*ip) + 1 - startCode, OPERANDS::fromRaw(*(++ip))... };
    }

    template <typename Action> void destructure(Action& action, unsigned char*& ip) const {
        const auto c = ip[0] - startCode + 1;
        ip += 1;
        assert(validNibble(c));
        const auto getNextIp = [&]() {
            const auto out = ip[0];
            ip += 1;
            return out;
        };

        std::apply(
            [&](auto... args) {
                action(std::tuple { name, c }, args...);
            },
            std::tuple<OPERANDS...> { OPERANDS::fromRaw(getNextIp())... });
    }
};
template <Byte STARTCODE, Byte ENDCODE, typename... OPERANDS> struct SecondNibble12bitSpec {
    static constexpr auto startCode { STARTCODE };
    static constexpr auto endCode { ENDCODE };
    static constexpr auto operandCount { sizeof...(OPERANDS) };
    using Tuple = std::tuple<unsigned int, OPERANDS...>;

    const char* name;
    unsigned int byteSize = sizeof...(OPERANDS) + sizeof(Byte); // also include code.

    template <int i> static constexpr auto codeOffset() {
        static_assert(startCode + i < endCode);
        return startCode + i;
    }

    bool validNibble(unsigned int nibble) const {
        const unsigned int highBits = nibble >> 8U;
        return startCode + highBits < endCode;
    }

    void emit(LocationInSourceCode loc, unsigned int fullValue, OPERANDS... operands) const {
        assert(fullValue < (1 << 12));
        const Byte code = startCode + ((fullValue >> 8) & 15);
        assert(code >= startCode);
        assert(code < endCode);

        gCompilingBytecodes.emit(loc, code, fullValue & 255, operands...);
    }

    Tuple pullOperandsFromInstructions(unsigned char*& ip) const {
        // increment instruction pointer and get the values for each operand.
        const unsigned int highbits = (static_cast<unsigned int>(*ip) - startCode) << 8;
        const unsigned int lowbits = *(++ip);
        return { highbits | lowbits, OPERANDS::fromRaw(*(++ip))... };
    }

    template <typename Action> void destructure(Action& action, unsigned char*& ip) const {
        const uint32_t codeAndTopBits = ip[0] - startCode;
        const uint32_t bottomBits = ip[1];
        ip += 2;
        const uint32_t twelveBitValue = (codeAndTopBits << 8UL) | bottomBits;

        assert(validNibble(twelveBitValue));
        const auto getNextIp = [&]() {
            const auto out = ip[0];
            ip += 1;
            return out;
        };

        std::apply(
            [&](auto... args) {
                action(std::tuple { name, twelveBitValue }, args...);
            },
            std::tuple<OPERANDS...> { OPERANDS::fromRaw(getNextIp())... });
    }
};
template <Byte STARTCODE, Byte ENDCODE, typename ENUM_T, typename... OPERANDS> struct SecondNibbleViaEnumOpSpec {
    static constexpr Byte startCode { STARTCODE };
    static constexpr Byte endCode { ENDCODE };
    static_assert(startCode + (int)ENUM_T::COUNT == endCode);
    static constexpr auto operandCount { sizeof...(OPERANDS) };
    using Tuple = std::tuple<ENUM_T, OPERANDS...>;
    using Enum = ENUM_T;

    const char* name;
    unsigned int byteSize = sizeof...(OPERANDS) + 1U;

    template <int i> static constexpr auto codeOffset() {
        static_assert(startCode + i < endCode);
        return startCode + i;
    }


    void emit(LocationInSourceCode loc, ENUM_T e, OPERANDS... operands) const {
        const Byte nibble = static_cast<Byte>(e);
        assert(nibble < 16);
        const Byte bytecode = startCode + nibble;
        assert(bytecode < endCode);

        gCompilingBytecodes.emit(loc, bytecode, operands...);
    }

    Tuple pullOperandsFromInstructions(unsigned char*& ip) const {
        return { { details::to_enum<typename ENUM_T::UnderlyingEnum>(*ip - startCode) },
                 OPERANDS::fromRaw(*(++ip))... };
    }

    template <typename Action> void destructure(Action& action, unsigned char*& ip) const {
        const uint32_t enumValue = ip[0] - startCode;
        ip += 1;
        assert(enumValue < ENDCODE);
        const auto e = static_cast<typename ENUM_T::UnderlyingEnum>(enumValue);
        const auto getNextIp = [&]() {
            const auto out = ip[0];
            ip += 1;
            return out;
        };

        std::apply(
            [&](auto... args) {
                action(std::tuple { name, asName(e) }, args...);
            },
            std::tuple<OPERANDS...> { OPERANDS::fromRaw(getNextIp())... });
    }
};


} // namespace details
