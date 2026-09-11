#pragma once
#include "text_location.hpp"
#include "ByteCodeArray.h"
#include <cassert>
#include <tuple>
#include <cstring>

namespace Opcode::details {
template <typename I> constexpr static Byte to_byte(I i) {
    Byte to {};
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

    Tuple pullOperandsFromInstructions(unsigned char*& ip) const { return { OPERANDS::fromRaw(*(++ip))... }; }

    void emit(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, OPERANDS... operands) const {
        bc.emit(loc, code, static_cast<Byte>(operands)...);
    }

    // Emits the code with a prefix, used in the extended opcodes.
protected:
    void emitPrefix(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, Byte prefix, OPERANDS... operands) const {
        bc.emit(loc, prefix, code, static_cast<Byte>(operands)...);
    }
};

template <Byte PREFIX, typename OPSPEC> struct WithPrefix : public OPSPEC {
    constexpr WithPrefix() = delete;

    unsigned int byteSize = 1U + OPSPEC::byteSize;

    template <typename... OPERANDS>
    void emit(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, OPERANDS... operands) const {
        OPSPEC::emitPrefix(bc, loc, PREFIX, operands...);
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

    bool validNibble(unsigned int nibble) const { return startCode + nibble < endCode; }

    const char* name;
    const char* secondNibbleDescription = "";
    unsigned int byteSize = sizeof...(OPERANDS) + 1U;

    void emit(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, unsigned int nibble, OPERANDS... operands) const {
        const Byte bytecode = startCode + nibble;
        assert(bytecode < endCode);
        bc.emit(loc, bytecode, static_cast<Byte>(operands)...);
    }

    Tuple pullOperandsFromInstructions(unsigned char*& ip) const {
        // increment instruction pointer and get the values for each operand.
        return { (*ip) - startCode, OPERANDS::fromRaw(*(++ip))... };
    }

protected:
    void emitPrefix(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, Byte prefix, unsigned int nibble,
                    OPERANDS... operands) const {
        const Byte bytecode = startCode + nibble;
        assert(bytecode < endCode);
        bc.emit(loc, prefix, bytecode, static_cast<Byte>(operands)...);
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

    bool validNibble(unsigned int nibble) const { return startCode + (nibble - 1) < endCode; }

    void emit(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, unsigned int nibble, OPERANDS... operands) const {
        assert(nibble != 0);
        const Byte bytecode = startCode + (nibble - 1);
        assert(bytecode < endCode);
        bc.emit(loc, bytecode, static_cast<Byte>(operands)...);
    }

    Tuple pullOperandsFromInstructions(unsigned char*& ip) const {
        // increment instruction pointer and get the values for each operand.
        return { (*ip) + 1 - startCode, OPERANDS::fromRaw(*(++ip))... };
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

    void emit(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, unsigned int fullValue,
              OPERANDS... operands) const {
        assert(fullValue < (1 << 12));
        const Byte code = startCode + ((fullValue >> 8) & 15);
        assert(code >= startCode);
        assert(code < endCode);
        bc.emit(loc, code, fullValue & 255, static_cast<Byte>(operands)...);
    }

    Tuple pullOperandsFromInstructions(unsigned char*& ip) const {
        // increment instruction pointer and get the values for each operand.
        const unsigned int highbits = (static_cast<unsigned int>(*ip) - startCode) << 8;
        const unsigned int lowbits = *(++ip);
        return { highbits | lowbits, OPERANDS::fromRaw(*(++ip))... };
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


    void emit(CompilingBytecodes& bc, sc::lex::SourceCodeRange loc, ENUM_T e, OPERANDS... operands) const {
        const Byte nibble = static_cast<Byte>(e);
        assert(nibble < 16);
        const Byte bytecode = startCode + nibble;
        assert(bytecode < endCode);
        bc.emit(loc, bytecode, static_cast<Byte>(operands)...);
    }

    Tuple pullOperandsFromInstructions(unsigned char*& ip) const {
        return { { details::to_enum<typename ENUM_T::UnderlyingEnum>(*ip - startCode) },
                 OPERANDS::fromRaw(*(++ip))... };
    }
};


} // namespace details
