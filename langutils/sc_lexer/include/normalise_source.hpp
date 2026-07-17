#pragma once

#include <string>

namespace sc::lex {

// Removes platform specific features (such as newline encoding).
struct NormalisedSource {
    NormalisedSource(const char* source, std::size_t sz): s(normalise_string(source, sz)) {}
    NormalisedSource(const std::string& raw): s(normalise_string(raw.c_str(), raw.size())) {}
    NormalisedSource(NormalisedSource&&) noexcept = default;
    NormalisedSource(const NormalisedSource&) = default;
    NormalisedSource& operator=(NormalisedSource&&) noexcept = default;
    NormalisedSource& operator=(const NormalisedSource&) = default;

    // Turns into a string or immutable reference to one depending on the qualifier
    [[nodiscard]] explicit operator const std::string&() const& { return s; }
    [[nodiscard]] explicit operator std::string() && { return std::move(s); }
    [[nodiscard]] explicit operator std::string() const&& { return s; }

private:
    std::string s {};

    [[nodiscard]] static std::string normalise_string(const char* in, std::size_t sz) {
        std::string out {};
        out.reserve(sz);
        for (std::size_t i { 0 }; i < sz; ++i) {
            switch (in[i]) {
            case '\r': {
                if (i + 1 < sz && in[i + 1] == '\n') {
                    i += 1; // skip \n too
                    out.push_back('\n');
                } else {
                    out.push_back('\n');
                }

            } break;
            case '\v':
            case '\f':
                out.push_back('\n');
                break;
            default:
                out.push_back(in[i]);
                break;
            }
        }
        return out;
    }
};


}
