#pragma once

#include "sc_util/strong_index.hpp"
#include "sc_util/overload.hpp"

#include "symbols.hpp"

#include <cstdint>
#include <limits>
#include <utility>
#include <variant>
#include <vector>

namespace sc::ir {

struct ConstExpression;

using ConstExprArray = std::vector<ConstExpression>;
using ConstExprEvent = std::vector<std::pair<ConstExpression, ConstExpression>>;

using ConstStorage = std::variant< //
    double, //
    std::int32_t, //
    char, //
    bool, //
    SymSCSymbol, //
    SymSCString, //
    SymSCClassName, //
    ConstExprArray, //
    ConstExprEvent>;


using ConstExprIndex = sc::util::StrongIndex<std::uint32_t, struct ConstantExpressionIndex__>;
using OptionalConstExprIndex = sc::util::StrongOptionalIndex<std::uint32_t, struct ConstantExpressionIndex__,
                                                             std::numeric_limits<std::uint32_t>::max()>;

struct ConstExpression {
    constexpr ConstExpression(double d): storage(d) { }
    constexpr ConstExpression(std::int32_t d): storage(d) { }
    constexpr ConstExpression(char d): storage(d) { }
    constexpr ConstExpression(bool d): storage(d) { }
    constexpr ConstExpression(SymSCSymbol d): storage(d) { }
    constexpr ConstExpression(SymSCString d): storage(d) { }
    constexpr ConstExpression(SymSCClassName d): storage(d) { }
    ConstExpression(ConstExprArray d) noexcept: storage(std::move(d)) { }
    ConstExpression(ConstExprEvent d) noexcept: storage(std::move(d)) { }

    ConstExpression(ConstExpression&&) noexcept = default;
    ConstExpression(const ConstExpression&) = default;
    ConstExpression& operator=(ConstExpression&&) = default;
    ConstExpression& operator=(const ConstExpression&) = default;

    ~ConstExpression() = default;


    [[nodiscard]] constexpr friend bool operator==(const ConstExpression& lhs, const ConstExpression& rhs) {
        return lhs.storage == rhs.storage;
    }

    template <typename F> decltype(auto) visit(F&& f) const { return std::visit(std::forward<F>(f), storage); }


    friend struct std::hash<ConstExpression>;

private:
    ConstStorage storage;
};


}

namespace std {
template <> struct hash<sc::ir::ConstExpression> {
    size_t operator()(const sc::ir::ConstExpression& e) const {
        return std::visit( //
            sc::util::overload {
                //
                [](std::int32_t i) -> size_t { return std::hash<std::int32_t>()(i); },
                [](double i) -> size_t { return std::hash<double>()(i); },
                [](char i) -> size_t { return std::hash<char>()(i); },
                [](bool i) -> size_t { return std::hash<bool>()(i); },
                [](sc::ir::Symbol i) -> size_t { return std::hash<sc::ir::Symbol>()(i); },
                [](const sc::ir::ConstExprEvent& vec) -> size_t {
                    std::size_t seed = vec.size();
                    for (const auto& pair : vec) {
                        const auto key_hash = std::hash<sc::ir::ConstExpression>()(pair.first);
                        const auto value_hash = std::hash<sc::ir::ConstExpression>()(pair.second);
                        auto x = key_hash ^ value_hash;
                        x = ((x >> 16) ^ x) * 0x45d9f3b;
                        x = ((x >> 16) ^ x) * 0x45d9f3b;
                        x = (x >> 16) ^ x;
                        seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                    }
                    return seed;
                },
                [](const sc::ir::ConstExprArray& vec) -> size_t {
                    std::size_t seed = vec.size();
                    for (const auto& val : vec) {
                        auto x = std::hash<sc::ir::ConstExpression>()(val);
                        x = ((x >> 16) ^ x) * 0x45d9f3b;
                        x = ((x >> 16) ^ x) * 0x45d9f3b;
                        x = (x >> 16) ^ x;
                        seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                    }
                    return seed;
                } //
            },
            e.storage 
        );
    }
};
}
