// Copyright Jordan Henderson 2026
#pragma once
#include <cstdint>

namespace sc::ast {
// Which accessors a class variable, instance variable, or constant declares, e.g., `<`, `>`, and `<>`.
//
// The values are bit flags, so they can be combined and tested with has().
enum struct ReadWriteAccessor : std::uint8_t {
    Private = 0b000,
    // `<`, generates a getter.
    ExternallyReadable = 0b001,
    // `>`, generates a setter.
    ExternallyWriteable = 0b010,
    // `<>`, generates a getter and a setter.
    ExternallyReadAndWritable = 0b011,
    Const = 0b100,
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
}
