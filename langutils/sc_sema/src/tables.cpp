#include "sc_sema/const_expr.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/decls.hpp"
#include <algorithm>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace sc::ir {

constexpr std::string_view meta_prefix { "Meta_" };

namespace {
// The position of the highest set bit, v isn't zero.
std::size_t highest_bit(std::uint64_t v) {
    std::size_t bit { 0 };
    while (v >>= 1)
        ++bit;
    return bit;
}

// The chunk an id's entry is in, and where in it, see SymbolTable::m_chunks. Chunk k holds 2^(first_bits + k).
std::pair<std::size_t, std::size_t> chunk_of(std::uint32_t id, std::size_t first_bits) {
    const std::uint64_t v = std::uint64_t { id } + (std::uint64_t { 1 } << first_bits);
    const auto k = highest_bit(v) - first_bits;
    return { k, static_cast<std::size_t>(v - (std::uint64_t { 1 } << (first_bits + k))) };
}
}

// A shard is just a map, but a portion of the larger map.
// String_views hash then mod to a shard.
// This means rather than have one lock shared between all Symbols, we have a bunch of locks but since symbols only lock
// one, there is less contention.
struct SymbolTable::Shard {
    std::mutex lock;
    // The keys are views into the arena.
    std::unordered_map<std::string_view, Symbol> ids;


    // This is the storage of the strings, they never move so pointer stability is preserved.
    // If a string is longer than the block_size, it gets its own unique block.
    static constexpr std::size_t block_size { 64 * 1024 };
    std::vector<std::unique_ptr<char[]>> blocks;
    char* free { nullptr };
    std::size_t free_size { 0 };

    std::string_view store(std::string_view s) {
        if (s.size() > free_size) {
            const auto size = std::max(block_size, s.size());
            blocks.push_back(std::make_unique<char[]>(size));
            free = blocks.back().get();
            free_size = size;
        }
        std::copy(s.begin(), s.end(), free);
        const std::string_view stored { free, s.size() };
        free += s.size();
        free_size -= s.size();
        return stored;
    }
};

SymbolTable::SymbolTable(): m_shards(std::make_unique<Shard[]>(shard_count)) {}

SymbolTable::~SymbolTable() {
    for (auto& chunk : m_chunks)
        delete[] chunk.load();
}

std::string_view& SymbolTable::entry(std::uint32_t id) {
    const auto [k, offset] = chunk_of(id, first_chunk_bits);

    // TODO: I am unsure about this. I think it is correct, but my knowledge of memory ordering is a bit limited.
    auto* chunk = m_chunks[k].load(std::memory_order_acquire);
    if (!chunk) {
        // Two threads can make it at once, from different shards, the loser deletes its own.
        auto* made = new std::string_view[std::size_t { 1 } << (first_chunk_bits + k)];
        if (m_chunks[k].compare_exchange_strong(chunk, made, std::memory_order_acq_rel))
            chunk = made;
        else
            delete[] made;
    }
    return chunk[offset];
}

[[nodiscard]] Symbol SymbolTable::get(std::string_view s) {
    auto& shard = m_shards[std::hash<std::string_view> {}(s) % shard_count];
    std::scoped_lock lock { shard.lock };
    if (const auto fnd = shard.ids.find(s); fnd != shard.ids.end())
        return fnd->second;
    const auto stored = shard.store(s);
    const auto id = m_count.fetch_add(1, std::memory_order_relaxed);
    entry(id) = stored;
    const auto symbol = Symbol::from(id);
    shard.ids.insert({ stored, symbol });
    return symbol;
}

[[nodiscard]] SymSCSelector SymbolTable::get_selector(std::string_view s) { return SymSCSelector::from(*get(s)); }
[[nodiscard]] SymSCClassName SymbolTable::get_class_name(std::string_view s) { return SymSCClassName::from(*get(s)); }
[[nodiscard]] SymSCSymbol SymbolTable::get_symbol(std::string_view s) { return SymSCSymbol::from(*get(s)); }
[[nodiscard]] SymSCIdentifier SymbolTable::get_identifier(std::string_view s) { return SymSCIdentifier::from(*get(s)); }
[[nodiscard]] SymSCString SymbolTable::get_string(std::string_view s) { return SymSCString::from(*get(s)); }
[[nodiscard]] bool SymbolTable::is_meta(SymSCClassName n) const {
    return (*this)(n).substr(0, meta_prefix.size()) == meta_prefix;
}

[[nodiscard]] SymSCClassName SymbolTable::get_meta_class_name(std::string_view s) {
    std::string name { meta_prefix };
    name += s;
    return get_class_name(name);
}
[[nodiscard]] SymSCClassName SymbolTable::get_meta_class_name(SymSCClassName n) {
    return get_meta_class_name((*this)(n));
}


[[nodiscard]] SymSCPrimitive SymbolTable::get_primitive(std::string_view s) { return SymSCPrimitive::from(*get(s)); }

std::string_view SymbolTable::operator()(Symbol i) const noexcept {
    const auto [k, offset] = chunk_of(*i, first_chunk_bits);
    return m_chunks[k].load(std::memory_order_acquire)[offset];
}


[[nodiscard]] const ClassDeclaration::Member* ClassDeclaration::find_member(Symbol i) const {
    const auto fnd = m_members_map.find(i);
    if (fnd == m_members_map.end())
        return nullptr;
    return &fnd->second;
}

[[nodiscard]] ClassDeclaration::Member* ClassDeclaration::find_member(Symbol i) {
    const auto fnd = m_members_map.find(i);
    if (fnd == m_members_map.end())
        return nullptr;
    return &fnd->second;
}

using Method = ClassDeclaration::Method;
using Member = ClassDeclaration::Member;

[[nodiscard]] const Method* ClassDeclaration::find_method(Symbol i) const {
    if (auto fnd = m_methods.find(i); fnd != m_methods.end())
        return &fnd->second;
    else
        return nullptr;
}

[[nodiscard]] Method* ClassDeclaration::find_method(Symbol i) {
    if (auto fnd = m_methods.find(i); fnd != m_methods.end())
        return &fnd->second;
    else
        return nullptr;
}


ClassDeclaration::MethodInsert ClassDeclaration::insert(Method m) {
    assert(m.is_extention);
    const auto name = m.name;
    const auto existing = m_methods.find(name);
    if (existing == m_methods.end()) {
        m_methods.insert({ name, std::move(m) });
        return { MethodInsertResult::Ok, std::nullopt };
    }
    auto& current = existing->second;
    if (!current.is_extention) {
        m_overriden_methods.insert({ name, std::move(current) });
        current = std::move(m);
        return { MethodInsertResult::Ok, std::nullopt };
    }

    const Method previous = current;
    if (current.module_of_def == m.module_of_def) {
        current.poisoned = true;
        return { MethodInsertResult::DuplicateInModule, previous };
    }
    const bool legacy = current.version == ast::LanguageVersion::Legacy && m.version == ast::LanguageVersion::Legacy;
    if (legacy) {
        current = std::move(m);
        return { MethodInsertResult::OverwrittenBetweenModules, previous };
    }
    current.poisoned = true;
    return { MethodInsertResult::ConflictBetweenModules, previous };
}

ClassDeclaration::DuplicateLocation ClassDeclaration::insert_no_ext(Method m) {
    const auto name = m.name;

    if (m_names_in_scope.count(name) == 1) {
        if (m_members_map.count(name) == 1) {
            return DuplicateLocation::Member;
        } else
            return DuplicateLocation::Method;
    }

    m_methods.insert({ name, std::move(m) });
    return DuplicateLocation::None;
}

bool ClassDeclaration::insert(Member m) {
    const auto name = m.name;
    if (auto fnd = m_members_map.find(name); fnd != m_members_map.end()) {
        return false;
    }
    m_members_map.insert({ name, std::move(m) });
    m_members_order.push_back(name);
    return true;
}


void ClassDeclarations::register_class_declaration(ClassDeclaration decl) {
    SymSCClassName name = decl.name();
    std::scoped_lock lock { m_insert_lock };
    assert(m_class_declarations.count(name) == 0);
    m_class_declarations.insert({ name, std::move(decl) });
    assert(m_class_locks.count(name) == 0);
    m_class_locks[name];
}

[[nodiscard]] bool ClassDeclarations::exists(SymSCClassName name) const {
    std::scoped_lock lock { m_insert_lock };
    return m_class_declarations.find(name) != m_class_declarations.end();
}

[[nodiscard]] size_t ClassDeclarations::size() const { return m_class_declarations.size(); }

[[nodiscard]] size_t ClassDeclarations::num_not_poisoned() const {
    size_t count { 0 };
    for (auto& [k, v] : m_class_declarations)
        count += v.poisoned() ? 0 : 1;
    return count;
}

[[nodiscard]] ClassDeclaration* ClassDeclarations::get_unsafe(SymSCClassName n) {
    if (auto fnd = m_class_declarations.find(n); fnd != m_class_declarations.end()) {
        return &fnd->second;
    } else {
        return nullptr;
    }
}

[[nodiscard]] const ClassDeclaration* ClassDeclarations::get_unsafe(SymSCClassName n) const {
    if (auto fnd = m_class_declarations.find(n); fnd != m_class_declarations.end()) {
        return &fnd->second;
    } else {
        return nullptr;
    }
}
void ClassDeclarations::poison_all() {
    for (auto& [k, v] : m_class_declarations)
        v.poison();
}


[[nodiscard]] ConstExprIndex ConstantTable::register_constexpr(ConstExpression&& expr) {
    std::scoped_lock lock { m_lock };
    if (const auto fnd = expr_to_index.find(expr); fnd != expr_to_index.end()) {
        return fnd->second;
    }
    const auto index = m_rolling;
    m_rolling += 1;
    expr_to_index.insert({ expr, index });
    index_to_expr.insert({ index, expr });
    return index;
}

[[nodiscard]] const ConstExpression& ConstantTable::operator()(ConstExprIndex index) const noexcept {
    std::scoped_lock lock { m_lock };
    return index_to_expr.at(index);
}

} // sc::ir
