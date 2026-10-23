#include "literal_synthesis.hpp"
#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <sstream>

#include "sc_parser/ast.hpp"

namespace sc::ir {

namespace priv {
// from cpp ref
[[noreturn]] inline void unreachable() {
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
}
}

template <typename T> std::optional<T> from(const char* begin, const char* end) {
    T t;
    const auto [ptr, er] = std::from_chars(begin, end, t);
    if (ptr != end)
        return {};
    if (er != std::errc())
        return {};
    return { t };
}
template <typename T> std::optional<T> from(std::string_view v) { return from<T>(v.data(), v.data() + v.size()); }

namespace {
// A digit's value, the letters are 10 to 35 in either case. The lexer only accepts digits valid for the base.
int digit_value(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    return c - 'A' + 10;
}

// The digits as a number in base, saturating at the largest uint64, so it can be checked against a limit.
std::uint64_t integer_magnitude(std::string_view digits, std::uint64_t base) {
    constexpr auto max = std::numeric_limits<std::uint64_t>::max();
    std::uint64_t value { 0 };
    for (const char c : digits) {
        const auto d = static_cast<std::uint64_t>(digit_value(c));
        if (base != 0 && value > (max - d) / base)
            return max;
        value = value * base + d;
    }
    return value;
}

// The base of a radix literal, e.g., 16 in `16rFF`, and its digits.
std::pair<std::optional<std::uint64_t>, std::string_view> split_radix(std::string_view str) {
    const auto r = str.find('r');
    assert(r != std::string_view::npos); // The lexer ensures this.
    return { from<std::uint64_t>(str.substr(0, r)), str.substr(r + 1) };
}
}

std::optional<std::int32_t> synthesize_literal(const ast::IntNode& node, std::string_view str) {
    const bool negative = node.sign == ast::IntNode::Sign::Negative;
    // The most an unsigned value can be, a negative one can be one more, as -2147483648 is an Integer.
    std::uint64_t limit = std::numeric_limits<std::int32_t>::max() + std::uint64_t { negative ? 1u : 0u };
    std::uint64_t magnitude { 0 };
    switch (node.kind) {
    case ast::IntNode::Kind::Normal:
        magnitude = integer_magnitude(str, 10);
        break;
    case ast::IntNode::Kind::Hexadecimal: {
        // The 32 bits, so `0xFFFFFFFF` is -1. The digits before the x are ignored, as the old compiler did.
        const auto x = str.find_first_of("xX");
        assert(x != std::string_view::npos); // The lexer ensures this.
        magnitude = integer_magnitude(str.substr(x + 1), 16);
        limit = std::numeric_limits<std::uint32_t>::max();
        break;
    }
    case ast::IntNode::Kind::Radix: {
        const auto [base, digits] = split_radix(str);
        if (!base)
            return std::nullopt;
        magnitude = integer_magnitude(digits, *base);
        break;
    }
    default:
        priv::unreachable();
    }
    if (magnitude > limit)
        return std::nullopt;
    // Unsigned, so the hexadecimal bits wrap and -2147483648 doesn't overflow, then reinterpreted as signed.
    const auto bits = static_cast<std::uint32_t>(magnitude);
    return static_cast<std::int32_t>(negative ? 0u - bits : bits);
}

char synthesize_literal(const ast::ASCIINode&, std::string_view str) {
    assert(str.front() == '$');
    assert(str.size() == 2 || str.size() == 3);
    if (str.size() == 2) {
        assert(str[1] != 0); // lexer ensures this.
        return str[1];
    } else {
        assert(str[1] == '\\');
        switch (str[2]) {
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'f':
            return '\f';
        case 'v':
            return '\v';
        case '0':
            return '\0';
        default:
            return str[2];
        }
    }
}
std::optional<double> synthesize_literal(const ast::AccidentalNode& node, std::string_view str) {
    // The degree, then an `s` for sharp or `b` for flat, then for steps the same again, or for cents the cents.
    const auto accidental = str.find_first_of("bs");
    assert(accidental != std::string_view::npos); // The lexer ensures this.
    double degree { 0 };
    for (const char c : str.substr(0, accidental))
        degree = degree * 10. + digit_value(c);
    const bool flat = str[accidental] == 'b';
    const auto rest = str.substr(accidental + 1);

    double value { 0 };
    if (node.kind == ast::AccidentalNode::Kind::Cents) {
        // Thousandths, at most 499, so it can't reach the next degree, e.g., `3s40` is 3.04.
        double cents { 0 };
        for (const char c : rest)
            cents = cents * 10. + digit_value(c);
        cents = std::min(cents, 499.);
        value = degree + cents / (flat ? -1000. : 1000.);
    } else {
        // A tenth for each accidental, at most 4, e.g., `3ss` is 3.2.
        const double steps = std::min(static_cast<double>(rest.size() + 1), 4.);
        value = degree + (flat ? -steps : steps) / 10.;
    }
    if (!std::isfinite(value))
        return std::nullopt;
    if (node.sign == ast::AccidentalNode::Sign::Negative) {
        // Only the degree is negated, the accidental still raises or lowers it, e.g., `-3s` is -2.9, as the old
        // compiler did, by rounding to the degree.
        const double whole = std::floor(value + 0.5);
        value = -whole + (value - whole);
    }
    return value;
}

namespace {
// A radix float, e.g., `16rA.8`, as the old compiler computed it, the digits as one number divided by the base for each
// digit after the point. Digits that are too small to change the value are dropped, and the division is split, so a
// long literal whose value is representable still is. Nothing if base isn't known.
std::optional<double> radix_float(std::string_view str) {
    const auto [maybe_base, digits] = split_radix(str);
    if (!maybe_base)
        return std::nullopt;
    const auto base = static_cast<double>(*maybe_base);
    // Once the number is this large, a double can't hold more digits after the point.
    constexpr double significant = 1e20;
    double z { 0 };
    // The power of the base to scale z by, positive for integer digits that were too small to add.
    int exponent { 0 };
    bool after_point = false;
    for (const char c : digits) {
        if (c == '.') {
            after_point = true;
            continue;
        }
        const bool fits = z < significant;
        if (fits) {
            z = z * base + digit_value(c);
            if (after_point)
                --exponent;
        } else if (!after_point) {
            ++exponent;
        }
    }
    if (exponent >= 0)
        return z * std::pow(base, exponent);
    // Divided in two steps, as base to the power can overflow when z divided by it doesn't.
    const int half = -exponent / 2;
    return z / std::pow(base, half) / std::pow(base, -exponent - half);
}

// Whether the digits are all zero, so a value of zero is exact rather than an underflow.
bool all_zero(std::string_view digits) {
    return std::all_of(digits.begin(), digits.end(), [](char c) { return c == '0' || c == '.'; });
}
}

std::optional<double> synthesize_literal(const ast::FloatNode& node, std::string_view str) {
    std::optional<double> value;
    switch (node.kind) {
    case ast::FloatNode::Kind::Inf:
        return node.sign == ast::FloatNode::Sign::Negative ? -std::numeric_limits<double>::infinity()
                                                           : std::numeric_limits<double>::infinity();
    case ast::FloatNode::Kind::Exponent:
        [[fallthrough]];
    case ast::FloatNode::Kind::Normal:
        // `inf` is lexed as a float. from_chars reports values that overflow or underflow as out of range.
        value = from<double>(str);
        break;
    case ast::FloatNode::Kind::Radix: {
        value = radix_float(str);
        if (value && *value == 0 && !all_zero(split_radix(str).second))
            return std::nullopt;
        break;
    }
    default:
        priv::unreachable();
    }
    if (!value || (!std::isfinite(*value) && str != "inf"))
        return std::nullopt;
    return node.sign == ast::FloatNode::Sign::Negative ? -*value : *value;
}

std::string synthesize_literal(const ast::SymbolQuote&, std::string_view str) {
    // See github issue: #7783
    // '\n' === 'n'
    // '\n' !== "\n".asSymbol

    assert(str.front() == '\'');
    assert(str.back() == '\'');
    str.remove_prefix(1);
    str.remove_suffix(1);

    std::stringstream ss;

    bool escaped = false;
    for (auto it = str.begin(); it < str.end(); ++it) {
        if (*it == '\\' && !escaped) {
            escaped = true;
            continue;
        }
        ss << *it;
        escaped = false;
    }
    return std::move(ss).str();
}
std::string_view synthesize_literal(const ast::SymbolSlash&, std::string_view str) {
    assert(str.front() == '\\');
    str.remove_prefix(1);
    return str;
}
std::string_view synthesize_literal(const ast::KeyBinOp&, std::string_view str) {
    assert(str.back() == ':');
    str.remove_suffix(1);
    return str;
}
std::string synthesize_literal(ast::StringLitIndex i, const ast::ASTGraph& ast_graph) {
    std::stringstream out;
    for (auto it = ast_graph.children(i).iter(); it; it += 1) {
        auto line = ast_graph.string_view(*it);

        assert(line.size() >= 2);
        assert(line.front() == '"');
        assert(line.back() == '"');
        // removes quotes
        line.remove_suffix(1);
        line.remove_prefix(1);

        bool escaped = false;
        for (auto ch = line.begin(); ch < line.end(); ++ch) {
            if (*ch == '\\' && !escaped) {
                escaped = true;
                continue;
            }
            if (escaped) {
                switch (*ch) {
                case 'n':
                    out << '\n';
                    break;
                case 'r':
                    out << '\r';
                    break;
                case 't':
                    out << '\t';
                    break;
                case 'f':
                    out << '\f';
                    break;
                case 'v':
                    out << '\v';
                    break;
                default:
                    out << *ch;
                    break;
                }
                escaped = false;
            } else {
                out << *ch;
            }
        }
    }
    return std::move(out).str();
}
bool synthesize_literal(const ast::BooleanNode&, std::string_view str) { return str == "true"; }
ast::ReadWriteAccessor synthesize_literal(const ast::MaybeRWAccessorIndex i, const ast::ASTGraph& g) {
    if (const auto m = g.as_a<ast::RWAccessorIndex>(i))
        return g.payload(*m).accessor;
    else
        return ast::ReadWriteAccessor::Private;
}
}