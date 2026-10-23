#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <utility>

namespace sc::ast{
// Used as the semantic value when returning a token from the lexer in the bison parser.
struct LexerToken {};

enum struct LanguageVersion : std::uint8_t { Legacy, V3_16 };

struct LanguageRelease {
    unsigned major;
    unsigned minor;
    LanguageVersion version;
};
// The releases that changed the rules, oldest first. Those before the first are Legacy.
inline constexpr std::array language_releases { LanguageRelease { 3, 16, LanguageVersion::V3_16 } };

// The newest version at or before the release, or nothing after the latest release, as its rules aren't known.
[[nodiscard]] constexpr std::optional<LanguageVersion> version_for_release(unsigned major, unsigned minor) {
    const auto& latest = language_releases.back();
    if (std::pair { major, minor } > std::pair { latest.major, latest.minor })
        return std::nullopt;
    auto out = LanguageVersion::Legacy;
    for (const auto& r : language_releases)
        if (std::pair { major, minor } >= std::pair { r.major, r.minor })
            out = r.version;
    return out;
}

[[nodiscard]] constexpr const char* to_string(LanguageVersion v) {
    switch (v) {
    case LanguageVersion::Legacy:
        return "legacy";
    case LanguageVersion::V3_16:
        return "3.16";
    }
    return "unknown";
}

// Defines <, >, and <> accessors on variables.
enum struct ReadWriteAccessor : std::uint8_t {
    Private = 0b000,
    ExternallyReadable = 0b001,
    ExternallyWriteable = 0b010,
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

[[nodiscard]] constexpr ReadWriteAccessor combine(ReadWriteAccessor lhs, ReadWriteAccessor rhs) {
    return static_cast<ReadWriteAccessor>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}
}
