#pragma once

#include "sc_sema/symbols.hpp"
#include <algorithm>
#include <utility>
#include <vector>

namespace sc::ir {

struct ClassDeclarations;
struct CommonClasses;
struct CommonSymbols;

// Constant folding and some rewrites assume what a method does, e.g., that `copySeries` on an Array slices it.
// Extensions can replace these methods, so before compiling, each assumption is checked by looking the method up from
// the receiver's class: it must find the original implementation, rather than an extension or an override to be able to
// do the rewrite. This is create after declaring the classes, but before compiling classes, making it safe for
// multithread access during compilation.
//
struct KnownMethods {
    // Marks the methods the compiler knows, and records the assumptions that no longer hold.
    // This stands in for a pragma on the methods, until that is parsed.
    void check(ClassDeclarations& declarations, const CommonClasses& classes, const CommonSymbols& symbols);

    // Whether sending selector to an instance of receiver runs the implementation the compiler knows.
    // A method that isn't declared at all is assumed to be known.
    [[nodiscard]] bool is_known(SymSCClassName receiver, SymSCSelector selector) const {
        return std::find(m_overridden.begin(), m_overridden.end(), std::pair { receiver, selector })
            == m_overridden.end();
    }

    // `a.perform(\foo)` is rewritten to `a.foo`, which assumes no class implements perform differently to Object.
    [[nodiscard]] bool perform_is_known() const { return m_perform_is_known; }

private:
    // There are only a few known methods, so a search is fine.
    std::vector<std::pair<SymSCClassName, SymSCSelector>> m_overridden {};
    bool m_perform_is_known { true };
};

}
