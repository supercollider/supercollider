
#include "sc_sema/context.hpp"
#include "sc_sema/decls.hpp"
#include "sc_sema/intrinsic_constraint.hpp"
#include "sc_sema/symbol_table.hpp"

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
            // Should have one, but doesn't
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::incorrect_super_class(g->text_info(), g->location(decl.super()->second),
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

    // Check members

    // deal with empty requirement
    [&]() {
        if (!c.member_names)
            return; // does not care.
        // Expected both to be empty
        if (c.member_names->empty() && decl.member_order().empty())
            return;

        else if (c.member_names->empty() && !decl.member_order().empty()) {
            // Shouldn't be any.
            const auto first_name = decl.member_order().front();
            const auto& fnd = *decl.find_member(first_name);
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::todo(g->text_info(), g->location(fnd.name_index)));
            return;
        }
        // member_names is not empty.

        if (decl.member_order().empty()) {
            // Didn't provide any, uses class name location
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::todo(g->text_info(), g->location(decl.name_index())));
            return;
        }

        // both have something.

        if (decl.member_order().size() < c.member_names->size()) {
            // Too few
            const auto& fnd = *decl.find_member(decl.member_order().back());
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::todo(g->text_info(), g->location(fnd.name_index)));
            return;
        } else if (decl.member_order().size() > c.member_names->size()) {
            // Too many
            const auto final_name = decl.member_order()[c.member_names->size()];
            const auto& fnd = *decl.find_member(final_name);
            const auto g = decl.ast_graph();
            out.push_back(sc::diag::Diagnostic::todo(g->text_info(), g->location(fnd.name_index)));
            return;
        }


        assert(c.member_names->size() == decl.member_order().size()); // checked above.

        size_t i { 0 };
        for (auto m : decl.member_order()) {}

        for (size_t i { 0 };; ++i) {
            if (i >= c.member_names->size())
                return;

            const auto& got_name = decl.member_order()[i];
            const auto& got = *decl.find_member(got_name);
            const auto& expected = (*c.member_names)[i];

            if (got.expr_index) {
                //  Intrinsic classes shouldn't have defaults.
                const auto g = decl.ast_graph();
                out.push_back(sc::diag::Diagnostic::todo(g->text_info(), g->location(got.expr_index->second)));
                return;
            }
            if (got.name != expected) {
                //  Incorrect name
                const auto g = decl.ast_graph();
                const auto expected_name = cxt.symbol_table(expected);
                out.push_back(sc::diag::Diagnostic::intrinsic_class_wrong_member_name(
                    g->text_info(), g->location(got.name_index), expected_name));
                return;
            }
        }
    }();


    return out;
};
}