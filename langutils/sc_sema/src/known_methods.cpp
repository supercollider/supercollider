#include "sc_sema/known_methods.hpp"
#include "sc_sema/decls.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir {

namespace {

// The methods the compiler assumes the implementation of.
// receiver is the class the assumption is about, defined_in is the class whose original method is assumed.
// These will become a pragma on the methods.
struct KnownMethod {
    const char* receiver;
    const char* selector;
    const char* defined_in;
};

constexpr KnownMethod known_methods[] = {
    { "Integer", "series", "SimpleNumber" }, // ranges, `(1..5)`
    { "Array", "copySeries", "ArrayedCollection" }, // slicing, `a[1..3]`
    { "String", "copySeries", "ArrayedCollection" },
    { "Array", "add", "ArrayedCollection" }, // `Array[1, 2]`
    { "Event", "add", "Dictionary" }, // `Event[a: 1]`
};

// The method found by sending selector to an instance of name, or nullptr if there isn't one.
const ClassDeclaration::Method* lookup(ClassDeclarations& declarations, SymSCClassName name, SymSCSelector selector) {
    std::size_t depth { 0 };
    while (true) {
        const auto* decl = declarations.get_unsafe(name);
        if (!decl || decl->poisoned())
            return nullptr;
        if (const auto* method = decl->find_method(selector))
            return method;
        const auto super = decl->super();
        if (!super)
            return nullptr;
        name = super->first;

        depth += 1;
        // The depth is here just in case.
        // The only way a loop can occur is if loops haven't been poisoned.
        // This is done while compileing the class library, so should never happen.
        assert(depth < 1024);
    }
    return nullptr;
}

}

void KnownMethods::check(ClassDeclarations& declarations, SymbolTable& symbols) {
    for (const auto& k : known_methods) {
        const auto selector = symbols.get_selector(k.selector);
        if (auto* decl = declarations.get_unsafe(symbols.get_class_name(k.defined_in)))
            if (auto* method = decl->find_method(selector); method && !method->is_extention)
                method->compiler_known_implementation = true;
    }

    for (const auto& k : known_methods) {
        const auto receiver = symbols.get_class_name(k.receiver);
        const auto selector = symbols.get_selector(k.selector);
        if (const auto* method = lookup(declarations, receiver, selector);
            method && !method->compiler_known_implementation)
            m_overridden.push_back({ receiver, selector });
    }

    // Only Object's perform may exist.
    const auto perform = symbols.get_selector("perform");
    const auto object = symbols.get_class_name("Object");
    if (auto* decl = declarations.get_unsafe(object))
        if (auto* method = decl->find_method(perform); method && !method->is_extention)
            method->compiler_known_implementation = true;
    for (auto it = declarations.unsafe_begin(); it != declarations.unsafe_end(); ++it)
        if (const auto* method = it->second.find_method(perform); method && !method->compiler_known_implementation)
            m_perform_is_known = false;
}

}
