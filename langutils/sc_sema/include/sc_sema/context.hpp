#pragma once

#include "sc_sema/details/class_library_sources.hpp"
#include "sc_sema/details/symbols.hpp"
#include "sc_sema/details/const_expr.hpp"
#include "sc_sema/module.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace sc::ir {

struct SymbolTable;

/// @brief Everything the compilation shares: the symbols, constants, declared classes, and diagnostics.
/// Its layout is private to sc_sema, so it is only made with make_context() and read with the functions below.
struct Context;

struct ContextDeleter {
    void operator()(Context* ctx) const;
};
using ContextPtr = std::unique_ptr<Context, ContextDeleter>;

/// @brief Makes the context for a compilation.
/// The symbol table, modules, and class library sources are referred to, not owned, so they must outlive the context.
[[nodiscard]] ContextPtr make_context(SymbolTable& symbols, SymSCClassName root_object, ModuleTable& modules,
                                      ClassLibrarySources& class_library);

[[nodiscard]] const SymbolTable& symbol_table(const Context& ctx);
[[nodiscard]] const ConstantTable& constant_table(const Context& ctx);
[[nodiscard]] SymSCClassName array_class(const Context& ctx);
[[nodiscard]] SymSCClassName event_class(const Context& ctx);

[[nodiscard]] SymSCClassName interpreter_class(const Context& ctx);
[[nodiscard]] SymSCIdentifier this_identifier(const Context& ctx);

/// @brief How many classes were declared without errors, counting each class and its meta class.
[[nodiscard]] std::size_t num_declared_classes(const Context& ctx);

/// @brief Adds a primitive's arguments from the VM's table, which a method calling it must match.
/// Keeps the existing one if already added. Call before compiling the class library.
/// @param num_arguments The number of arguments, which counts the receiver, as the VM does.
void add_primitive_signature(Context& ctx, SymSCPrimitive primitive, std::uint8_t num_arguments,
                             bool variadic_arguments, bool keyword_arguments);

}
