#include "context_impl.hpp"

namespace sc::ir {

Context::Context(SymbolTable& sym_table, CommonClasses com, ModuleTable& mod_t, ClassLibrarySources& class_library):
    symbol_table(sym_table),
    common_classes(com),
    common_symbols(sym_table),
    modules(mod_t),
    class_library_ast(class_library) {}

void ContextDeleter::operator()(Context* ctx) const { delete ctx; }

ContextPtr make_context(SymbolTable& symbols, SymSCClassName root_object, ModuleTable& modules,
                        ClassLibrarySources& class_library) {
    return ContextPtr { new Context { symbols, symbol_table_and_common_classes(symbols, root_object), modules,
                                      class_library } };
}

const SymbolTable& symbol_table(const Context& ctx) { return ctx.symbol_table; }
const ConstantTable& constant_table(const Context& ctx) { return ctx.constant_table; }
SymSCClassName array_class(const Context& ctx) { return ctx.common_classes.array; }
SymSCClassName event_class(const Context& ctx) { return ctx.common_classes.event; }
SymSCClassName interpreter_class(const Context& ctx) { return ctx.common_symbols.interpreter; }
SymSCIdentifier this_identifier(const Context& ctx) { return ctx.common_symbols.this_; }

std::size_t num_declared_classes(const Context& ctx) { return ctx.class_declarations.num_not_poisoned(); }

void add_primitive_signature(Context& ctx, SymSCPrimitive primitive, std::uint8_t num_arguments,
                             bool variadic_arguments, bool keyword_arguments) {
    ctx.primitive_signatures.insert(
        { primitive, PrimitiveSignature { num_arguments, variadic_arguments, keyword_arguments } });
}

}
