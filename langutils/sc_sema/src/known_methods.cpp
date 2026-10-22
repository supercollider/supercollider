#include "sc_sema/known_methods.hpp"
#include "sc_sema/decls.hpp"
#include "sc_sema/common_classes.hpp"
#include "sc_sema/common_symbols.hpp"

namespace sc::ir {

namespace {

// The method found by sending selector to an instance of name, or nullptr if there isn't one.
const ClassDeclaration::Method* lookup(ClassDeclarations& declarations, SymSCClassName name, SymSCSelector selector) {
    // Bounded, so a broken hierarchy can't loop forever, those classes are poisoned anyway.
    for (std::size_t depth { 0 }; depth < 1024; ++depth) {
        const auto* decl = declarations.get_unsafe(name);
        if (!decl || decl->poisoned())
            return nullptr;
        if (const auto* method = decl->find_method(selector))
            return method;
        const auto super = decl->super();
        if (!super)
            return nullptr;
        name = super->first;
    }
    return nullptr;
}

}

void KnownMethods::check(ClassDeclarations& declarations, const CommonClasses& classes, const CommonSymbols& symbols) {
    // The methods the compiler assumes the implementation of. receiver is the class the assumption is about,
    // defined_in is the class whose original method is assumed. These will become a pragma on the methods.
    struct KnownMethod {
        SymSCClassName receiver;
        SymSCSelector selector;
        SymSCClassName defined_in;
    };
    const KnownMethod known_methods[] = {
        { classes.int_, symbols.series, symbols.simple_number }, // ranges, `(1..5)`
        { classes.array, symbols.copy_series, symbols.arrayed_collection }, // slicing, `a[1..3]`
        { classes.string, symbols.copy_series, symbols.arrayed_collection },
        { classes.array, symbols.at, symbols.arrayed_collection }, // pack assignment, `var #a, b = [1, 2]`
        { classes.array, symbols.add, symbols.arrayed_collection }, // `Array[1, 2]`
        { classes.event, symbols.add, symbols.dictionary }, // `Event[a: 1]`
    };

    for (const auto& k : known_methods)
        if (auto* decl = declarations.get_unsafe(k.defined_in))
            if (auto* method = decl->find_method(k.selector); method && !method->is_extention)
                method->compiler_known_implementation = true;

    for (const auto& k : known_methods)
        if (const auto* method = lookup(declarations, k.receiver, k.selector);
            method && !method->compiler_known_implementation)
            m_overridden.push_back({ k.receiver, k.selector });

    // perform can be sent to anything, so only Object's may exist.
    if (auto* decl = declarations.get_unsafe(symbols.object))
        if (auto* method = decl->find_method(symbols.perform); method && !method->is_extention)
            method->compiler_known_implementation = true;
    for (auto it = declarations.unsafe_begin(); it != declarations.unsafe_end(); ++it)
        if (const auto* method = it->second.find_method(symbols.perform);
            method && !method->compiler_known_implementation)
            m_perform_is_known = false;
}
}
