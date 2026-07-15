#pragma once
#include <cstdint>

namespace sc::ast{
// Used as the semantic value when returning a token from the lexer in the bison parser.
struct LexerToken {};

// Defines <, >, and <> accessors on variables.
enum struct ReadWriteAccessor : std::uint8_t {
    Private = 0b000,
    ExternallyReadable = 0b001,
    ExternallyWriteable = 0b010,
    ExternallyReadAndWritable = 0b011,
};

[[nodiscard]] constexpr bool has(ReadWriteAccessor r, ReadWriteAccessor flag) {
    return static_cast<std::uint8_t>(r) & static_cast<std::uint8_t>(flag);
}

[[nodiscard]] constexpr ReadWriteAccessor remove(ReadWriteAccessor from, ReadWriteAccessor to_remove) {
    return static_cast<ReadWriteAccessor>(static_cast<std::uint8_t>(from) & ~static_cast<std::uint8_t>(to_remove));
}

template <ReadWriteAccessor... Rs> [[nodiscard]] constexpr ReadWriteAccessor combine() {
    return static_cast<ReadWriteAccessor>((static_cast<std::uint8_t>(Rs) | ...));
}

[[nodiscard]] constexpr ReadWriteAccessor combine(ReadWriteAccessor lhs, ReadWriteAccessor rhs) {
    return static_cast<ReadWriteAccessor>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}
}
