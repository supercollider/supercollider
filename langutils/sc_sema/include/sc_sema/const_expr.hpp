#pragma once

#include "sc_util/overload.hpp"
#include "const_expr_index.hpp"
#include "symbols.hpp"

#include <cstdint>
#include <mutex>
#include <ostream>
#include <utility>
#include <variant>
#include <vector>

namespace sc::ir {

struct ConstExpression;

using ConstExprArray = std::vector<ConstExpression>;
using ConstExprEvent = std::vector<std::pair<ConstExpression, ConstExpression>>;


using Nil = std::monostate;

using ConstStorage = std::variant< //
    double, //
    std::int32_t, //
    char, //
    bool, //
    Nil,
    SymSCSymbol, //
    SymSCString, //
    SymSCClassName, //
    ConstExprArray, //
    ConstExprEvent>;

struct ConstExpression {
    constexpr ConstExpression(double d): storage(d) { }
    constexpr ConstExpression(std::int32_t d): storage(d) { }
    constexpr ConstExpression(char d): storage(d) { }
    constexpr ConstExpression(bool d): storage(d) { }
    constexpr ConstExpression(SymSCSymbol d): storage(d) { }
    constexpr ConstExpression(SymSCString d): storage(d) { }
    constexpr ConstExpression(SymSCClassName d): storage(d) { }
    constexpr ConstExpression(Nil n): storage(n) {}
    ConstExpression(ConstExprArray d) noexcept: storage(std::move(d)) { }
    ConstExpression(ConstExprEvent d) noexcept: storage(std::move(d)) { }
    template <typename T> ConstExpression(T) = delete;

    ConstExpression(ConstExpression&&) noexcept = default;
    ConstExpression(const ConstExpression&) = default;
    ConstExpression& operator=(ConstExpression&&) = default;
    ConstExpression& operator=(const ConstExpression&) = default;

    ~ConstExpression() = default;


    [[nodiscard]] constexpr friend bool operator==(const ConstExpression& lhs, const ConstExpression& rhs) {
        return lhs.storage == rhs.storage;
    }

    template <typename F> decltype(auto) visit(F&& f) const { return std::visit(std::forward<F>(f), storage); }

    template <typename T> [[nodiscard]] const T* get_if() const { return std::get_if<T>(&storage); }

    [[nodiscard]] bool is_mutable() const {
        return get_if<ConstExprArray>() || get_if<ConstExprEvent>() || get_if<SymSCString>();
    }

    friend struct std::hash<ConstExpression>;

    void to_json(std::ostream&, const struct SymbolTable&) const;
    // As SuperCollider source, e.g., `[1, \a, "b"]`, kept on one line.
    void to_literal(std::ostream&, const struct SymbolTable&) const;

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
                [](bool i) -> size_t { return std::hash<bool>()(i); }, [](std::monostate) -> size_t { return 0; },
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
            e.storage);
    }
};

}


namespace sc::ir {
struct ConstantTable {
    [[nodiscard]] ConstExprIndex register_constexpr(ConstExpression&& expr);

    // this is marked noexcept because you should never construct an index yourself.
    [[nodiscard]] const ConstExpression& operator()(ConstExprIndex index) const noexcept;


private:
    mutable std::mutex m_lock {};
    ConstExprIndex::Underlying m_rolling { 0 };
    // This means we duplicate the constexpr.
    // This could probably be optimized.
    std::unordered_map<ConstExprIndex, ConstExpression> index_to_expr { };
    std::unordered_map<ConstExpression, ConstExprIndex> expr_to_index { };
};

}