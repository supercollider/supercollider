#pragma once

#include "sc_sema/symbols.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <string_view>
namespace sc::ir {

// Interns strings as Symbols, from many threads at once.
// Each string is stored once, in an arena that never moves, and the maps are split into shards, each with its own lock,
// so threads interning different strings rarely wait for each other. Looking a Symbol's string up takes no lock.
// Symbols are numbered in the order they are first seen, which varies between runs when threaded, so nothing should be
// ordered by them.
struct SymbolTable {
    [[nodiscard]] Symbol get(std::string_view s);

    [[nodiscard]] SymSCSelector get_selector(std::string_view s);
    [[nodiscard]] SymSCClassName get_class_name(std::string_view s);
    [[nodiscard]] SymSCClassName get_meta_class_name(std::string_view s);
    [[nodiscard]] SymSCClassName get_meta_class_name(SymSCClassName);


    [[nodiscard]] SymSCPrimitive get_primitive(std::string_view);

    [[nodiscard]] SymSCSymbol get_symbol(std::string_view s);
    [[nodiscard]] SymSCIdentifier get_identifier(std::string_view s);
    [[nodiscard]] SymSCString get_string(std::string_view s);

    // Meta classes are named with a prefix, `Meta_Object`, which a class can't be named with.
    [[nodiscard]] bool is_meta(SymSCClassName n) const;

    SymbolTable();
    ~SymbolTable();
    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    // The view stays valid while the table exists, even as other threads add symbols.
    [[nodiscard]] std::string_view operator()(Symbol i) const noexcept;

private:
    struct Shard;
    static constexpr std::size_t shard_count { 64 };
    std::unique_ptr<Shard[]> m_shards;

    // Each Symbol's string, in chunks that double in size, the first holding 2^first_chunk_bits, so a chunk never moves
    // once it is made, and reading doesn't need a lock. An entry is written before its Symbol is returned, so any
    // thread given the Symbol can read it. 32 bit Symbols need at most chunk_count chunks.
    static constexpr std::size_t first_chunk_bits { 10 };
    static constexpr std::size_t chunk_count { 33 - first_chunk_bits };
    [[nodiscard]] std::string_view& entry(std::uint32_t id);
    std::array<std::atomic<std::string_view*>, chunk_count> m_chunks {};
    std::atomic<std::uint32_t> m_count { 0 };
};
}
