
#include "sc_parser/ast.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/decls.hpp"
#include "sc_sema/intrinsic_constraint.hpp"
#include "sc_sema/symbol_table.hpp"

#include <cstddef>
#include <iterator>
#include <string>

namespace sc::ir {

// Checks the constraint.
// As this mutates the class declaration, it is only thread safe if no other class mutates the same decl,
//  this is relatively easy to enforce as the contraint is const.
[[nodiscard]] std::vector<sc::diag::Diagnostic> check_intrinsic_constraint(const IntrinsicConstraint& c, Context& cxt,
                                                                           ClassDeclaration& decl) {
    assert(decl.name() == c.target);

    std::vector<sc::diag::Diagnostic> out;

    // Check super class
    [&]() {
        if (!c.super_class)
            return; // does not care
        const auto sym = *c.super_class;
        // both abscent
        if (!sym && !decl.super())
            return;

        if (sym && decl.super()) {
            // both present and equal
            if (sym == *decl.super()->first)
                return;

            // present, not equal
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::incorrect_super_class(g->text_info(), g->location(decl.super()->second),
                                                                      cxt.symbol_table({ *sym })));
            return;
        }
        if (sym && !decl.super()) {
            // Should have one, but doesn't, reported at the class name.
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::incorrect_super_class(g->text_info(), g->location(decl.name_index()),
                                                                      cxt.symbol_table({ *sym })));
        } else {
            // Shouldn't have one, but does.
            const auto g = decl.ast_graph();
            out.push_back(
                sc::diag::Diagnostic::incorrect_super_class(g->text_info(), g->location(decl.super()->second)));
        }
    }();

    // check slot def
    [&]() {
        if (!c.slot_def && !decl.slot_def())
            return;
        else if (c.slot_def && !decl.slot_def()) {
            // Should have a slot def, but doesn't
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::incorrect_slot_def(g->text_info(), g->location(decl.name_index()),
                                                                   cxt.symbol_table(*c.slot_def)));
            return;
        } else if (!c.slot_def && decl.slot_def()) {
            // Should not have one, but does
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::incorrect_slot_def(g->text_info(), g->location(decl.name_index())));
            return;
        } else if (*c.slot_def != decl.slot_def()->first) {
            // Not equal
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::incorrect_slot_def(g->text_info(), g->location(decl.name_index()),
                                                                   cxt.symbol_table(*c.slot_def)));
            return;
        }
    }();

    // Check members, the class's own instance variables, which must be the expected names in order. Their initial
    // values are the class library's, e.g., Thread's `state = 0`, as the old compiler had it.
    [&]() {
        if (!c.member_names)
            return; // does not care.
        const auto g = decl.ast_graph();
        const auto& expected_names = *c.member_names;
        const auto& members = decl.member_order();
        std::string expected;
        for (const auto name : expected_names)
            expected += (expected.empty() ? "" : ", ") + std::string { cxt.symbol_table(name) };

        // Too many reports the first extra member, too few the last member or, without any, the class name.
        if (members.size() != expected_names.size()) {
            const auto at = members.size() > expected_names.size()
                ? decl.find_member(*std::next(members.begin(), static_cast<std::ptrdiff_t>(expected_names.size())))
                      ->name_index
                : members.empty() ? decl.name_index()
                                  : decl.find_member(members.back())->name_index;
            out.push_back(
                sc::diag::Diagnostic::intrinsic_class_wrong_members(g->text_info(), g->location(at), expected));
            return;
        }

        auto next_expected = expected_names.begin();
        for (const auto& got_name : members) {
            const auto& got = *decl.find_member(got_name);
            const auto& expected_name = *next_expected++;
            if (got.name != expected_name) {
                out.push_back(sc::diag::Diagnostic::intrinsic_class_wrong_member_name(
                    g->text_info(), g->location(got.name_index), cxt.symbol_table(expected_name)));
                return;
            }
        }
    }();


    return out;
};
}