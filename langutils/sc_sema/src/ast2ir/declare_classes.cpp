
#include "declare_classes.hpp"

#include "consteval.hpp"
#include "ast2ir/literal_synthesis.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/common_symbols.hpp"
#include "sc_sema/source_location.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/decls.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_parser/ast_untyped.hpp"
#include "sc_parser/ast.hpp"


namespace sc::ir::ast2ir {

void DeclareClass::append_member_to_decl(ClassDeclaration& decl, ClassDeclaration&, bool is_const,
                                         ast::Index total_index, ast::Index accessor_i, ast::ReadWriteAccessor accessor,
                                         ast::NamedIdentifierIndex name_i) {
    const auto name = cxt.symbol_table.get_identifier(get_string(*ast_graph, name_i));
    if (cxt.common_symbols.is_special(name)) {
        cxt.diagnostics(
            diag::Diagnostic::declared_special_variable(get_text_info(*ast_graph), get_location(*ast_graph, name_i)));
        return;
    }
    const auto r = decl.insert(
        ClassDeclaration::Member { total_index, name, { *name_i }, accessor, accessor_i, {}, {}, is_const });
    if (!r) {
        const auto* mem = decl.find_member(name);
        assert(mem);
        cxt.diagnostics(diag::Diagnostic::duplicate_member(
            get_text_info(*decl.ast_graph()), get_location(*decl.ast_graph(), mem->total_index),
            get_text_info(*decl.ast_graph()), get_location(*decl.ast_graph(), total_index)));
    }
}

void DeclareClass::append_member_to_decl(ClassDeclaration& decl, ClassDeclaration& constants, bool is_const,
                                         ast::Index total, ast::Index accessor_i, ast::ReadWriteAccessor accessor,
                                         ast::DeclareVariableWithDefaultIndex name_with_expr_i) {
    const auto [name_i_t, expr_i_t] = ast_graph->children(name_with_expr_i);
    const ast::NamedIdentifierIndex name_i = name_i_t;
    ast::AnyExprIndex expr_i = expr_i_t;
    const auto name = cxt.symbol_table.get_identifier(get_string(*ast_graph, name_i));
    if (cxt.common_symbols.is_special(name)) {
        cxt.diagnostics(
            diag::Diagnostic::declared_special_variable(get_text_info(*ast_graph), get_location(*ast_graph, name_i)));
        return;
    }

    // Legacy code only allows a literal, as the old compiler's grammar did, see ConstEval::is_legacy_literal.
    auto result = !folds_constants(decl.version()) && !ast2ir::ConstEval::is_legacy_literal(*ast_graph, expr_i)
        ? ast2ir::ConstEval::Result { diag::Diagnostic::not_constant(get_text_info(*ast_graph),
                                                                     get_location(*ast_graph, expr_i)) }
        : ast2ir::ConstEval { cxt, ast_graph, ast_index, decl.version(), &constants }.eval(expr_i);
    std::move(result).visit(
        [&](std::pair<ConstExprIndex, TypeInfo> p) {
            const auto [const_expr_index, type_info] = p;
            const auto name = cxt.symbol_table.get_identifier(get_string(*ast_graph, name_i));
            const auto r = decl.insert(ClassDeclaration::Member { total,
                                                                  name,
                                                                  { *name_i },
                                                                  accessor,
                                                                  accessor_i,
                                                                  { { const_expr_index, expr_i } },
                                                                  { type_info },
                                                                  is_const });
            if (!r) {
                cxt.diagnostics(
                    diag::Diagnostic::todo(get_text_info(*decl.ast_graph()), get_location(*decl.ast_graph(), name_i)));
            }
        },
        [&](diag::Diagnostic&& d) {
            cxt.diagnostics(std::move(d));
            // Right here, we don't create the member, this might be a mistake?
            // Consider creating the member and then poisoning it.
        });
}

void DeclareClass::append_member_to_decl(ClassDeclaration&, ClassDeclaration&, bool, ast::Index, ast::Index,
                                         ast::ReadWriteAccessor, ast::VariablePackAssignIndex) {
    assert(false);
}

std::optional<DeclareClass::Result> DeclareClass::operator()(ast::ClassIndex class_i) {
    assert(ast_graph);
    const auto [class_name_i, backing_slot, super_name_i, members, methods] = ast_graph->children(class_i);

    const auto class_name_str = std::string { get_string_view(*ast_graph, class_name_i) };
    assert(!class_name_str.empty());
    if (class_name_str.substr(0, 5) == "Meta_") {
        cxt.diagnostics(diag::Diagnostic::todo(get_text_info(*ast_graph), get_location(*ast_graph, class_name_i)));
        return { }; // Hard error here, this class cannot exist.
    }
    const auto class_name = cxt.symbol_table.get_class_name(class_name_str);

    if (auto before = cxt.class_names.encountered_class_before(class_name, ast_graph, class_name_i)) {
        const auto& [g, i] = *before;
        cxt.diagnostics( //
            diag::Diagnostic::duplicate_class(g->text_info(), get_location(*g, i), get_text_info(*ast_graph),
                                              get_location(*ast_graph, class_name_i)));
        return { };
    }

    // the one class with no super.
    const bool is_root_class = class_name == cxt.common_classes.root_object;

    const auto meta_class_name = cxt.symbol_table.get_meta_class_name(class_name_str);

    // Create decls
    // These are the only mutable variables in this function, it is what we build.
    const auto version = ast_graph->payload(class_i).version;
    ClassDeclaration class_decl { ast_graph, module, { *class_i }, class_name, { *class_name_i }, version };

    ClassDeclaration meta_class_decl { ast_graph, module, { *class_i }, meta_class_name, { *class_name_i }, version };

    class_decl.meta_class(meta_class_name);


    if (auto super = ast_graph->present(super_name_i)) {
        if (!is_root_class) {
            const auto sym = cxt.symbol_table.get_class_name(get_string(*ast_graph, super_name_i));
            class_decl.super(sym, { *super_name_i });
            cxt.inheritance_edges.insert(sym, class_name);
        } else {
            // the root object cannot inheirt from anything.
            cxt.diagnostics(diag::Diagnostic::root_object_must_not_inherit(get_text_info(*ast_graph),
                                                                           get_location(*ast_graph, super_name_i)));
            return { };
        }
    } else if (is_root_class) {
        cxt.inheritance_edges.insert(class_name);
    } else if (cxt.common_classes.default_inheritor == class_name) {
        // The default inheritor must inherit from something (unless it is also the root).
        cxt.diagnostics(diag::Diagnostic::default_inheritor_must_have_super(get_text_info(*ast_graph),
                                                                            get_location(*ast_graph, class_name_i)));
        return { };
    } else {
        // Location here is just white space.
        class_decl.super(cxt.common_classes.default_inheritor, { *super_name_i });
        cxt.inheritance_edges.insert(cxt.common_classes.default_inheritor, class_name);
    }

    // aka, if this is not abstract object
    if (class_decl.super()) {
        const auto super_meta_class_name_sym = cxt.symbol_table.get_meta_class_name(*class_decl.super()->first);
        meta_class_decl.super(super_meta_class_name_sym, class_decl.super()->second);
        cxt.inheritance_edges.insert(super_meta_class_name_sym, meta_class_name);
    } else {
        // META_AbstractObject inherits from Class
        // Basically a random location here.
        meta_class_decl.super(cxt.common_classes.class_, meta_class_decl.name_index());
        cxt.inheritance_edges.insert(cxt.common_classes.class_, meta_class_name);
    }


    // Slot def
    std::visit( //
        util::overload {
            //
            [&](ast::MissingIndex) -> void {}, //
            [&](ast::NamedIdentifierIndex i) -> void {
                const auto slot_sym = cxt.symbol_table.get_identifier(get_string(*ast_graph, i));
                if (cxt.common_symbols.is_slot_type(slot_sym)) {
                    class_decl.slot_def(slot_sym, { *i });
                } else {
                    cxt.diagnostics(
                        diag::Diagnostic::unexpected_slot_def(get_text_info(*ast_graph), get_location(*ast_graph, i)));
                    class_decl.poison();
                }
            } //
        },
        ast_graph->index_to_variant(backing_slot));

    // Add consts
    for (auto member_list_iter = ast_graph->children(members).iter(); member_list_iter; ++member_list_iter) {
        const auto member_var = ast_graph->index_to_variant(*member_list_iter);
        std::visit( //
            util::overload {
                [](auto) { },
                [&](ast::DeclareConstListIndex list_i) {
                    for (auto class_var_it = ast_graph->children(list_i).iter(); class_var_it; ++class_var_it) {
                        const auto [accessor_i_t, member_node] = ast_graph->children(*class_var_it);
                        const ast::Index accessor_i { *accessor_i_t };
                        auto accessor = synthesize_literal(accessor_i_t, *ast_graph);


                        if (has(accessor, ast::ReadWriteAccessor::ExternallyWriteable)) {
                            // We remove the writable flag and carry on compiling.
                            // TODO: create a diagnostic, this is an error, but not a fatal one.
                            accessor = remove(accessor, ast::ReadWriteAccessor::ExternallyWriteable);
                        }

                        std::visit(
                            [&](auto i) {
                                this->append_member_to_decl(meta_class_decl, meta_class_decl, true, { **class_var_it },
                                                            accessor_i, accessor, i);
                            },
                            ast_graph->index_to_variant(member_node));
                    }
                } //
            },
            member_var);
    }
    // Add other members
    for (auto member_list_iter = ast_graph->children(members).iter(); member_list_iter; ++member_list_iter) {
        const auto member_var = ast_graph->index_to_variant(*member_list_iter);
        std::visit( //
            util::overload {
                [](auto) { },
                [&](ast::DeclareMemberListIndex list_i) {
                    for (auto class_var_it = ast_graph->children(list_i).iter(); class_var_it; ++class_var_it) {
                        const auto [accessor_i_t, member_i] = ast_graph->children(*class_var_it);
                        const ast::Index accessor_i { *accessor_i_t };
                        const auto accessor = synthesize_literal(accessor_i_t, *ast_graph);

                        std::visit(
                            [&](auto i) {
                                this->append_member_to_decl(class_decl, meta_class_decl, false, { **class_var_it },
                                                            accessor_i, accessor, i);
                            },
                            ast_graph->index_to_variant(member_i));
                    }
                },
                [&](ast::DeclareClassMemberListIndex list_i) {
                    for (auto class_var_it = ast_graph->children(list_i).iter(); class_var_it; ++class_var_it) {
                        const auto [accessor_i_t, member_i] = ast_graph->children(*class_var_it);
                        const ast::Index accessor_i { *accessor_i_t };
                        const auto accessor = synthesize_literal(accessor_i_t, *ast_graph);

                        std::visit(
                            [&](auto i) {
                                this->append_member_to_decl(meta_class_decl, meta_class_decl, false, { **class_var_it },
                                                            accessor_i, accessor, i);
                            },
                            ast_graph->index_to_variant(member_i));
                    }
                },

            },
            member_var);
    }


    // Add methods
    for (auto method_it = ast_graph->children(methods).iter(); method_it; ++method_it) {
        std::visit(util::overload {
                       [&](ast::MethodIndex i) {
                           const auto [name_i, args_i, maybe_prim_i, body_i] = ast_graph->children(i);
                           const auto name = cxt.symbol_table.get_selector(get_string(*ast_graph, name_i));

                           const auto r = class_decl.insert_no_ext(
                               ClassDeclaration::Method { ast_graph, module, i, name, { *name_i }, version });
                           switch (r) {
                           case ClassDeclaration::DuplicateLocation::None:
                               return;
                           case ClassDeclaration::DuplicateLocation::Method: {
                               const auto* other = class_decl.find_method(name);
                               assert(other);
                               assert(other->ast_graph == ast_graph);

                               cxt.diagnostics(diag::Diagnostic::duplicate_method_name(
                                   get_text_info(*ast_graph), get_location(*ast_graph, name_i),
                                   get_text_info(*ast_graph), get_location(*ast_graph, other->name_index)));
                               break;
                           };
                           case ClassDeclaration::DuplicateLocation::Member: {
                               const auto* other = class_decl.find_member(name);
                               assert(other);

                               cxt.diagnostics(diag::Diagnostic::duplicate_method_name(
                                   get_text_info(*ast_graph), get_location(*ast_graph, name_i),
                                   get_text_info(*ast_graph), get_location(*ast_graph, other->name_index)));
                               break;
                           }
                           }
                       },
                       [&](ast::ClassMethodIndex i) {
                           const auto [name_i, args_i, maybe_prim_i, body_i] = ast_graph->children(i);
                           const auto name = cxt.symbol_table.get_selector(get_string(*ast_graph, name_i));
                           const auto r = meta_class_decl.insert_no_ext(
                               ClassDeclaration::Method { ast_graph, module, i, name, { *name_i }, version });
                           switch (r) {
                           case ClassDeclaration::DuplicateLocation::None:
                               return;
                           case ClassDeclaration::DuplicateLocation::Method: {
                               const auto* other = meta_class_decl.find_method(name);
                               assert(other);
                               assert(other->ast_graph == ast_graph);

                               cxt.diagnostics(diag::Diagnostic::duplicate_method_name(
                                   get_text_info(*ast_graph), get_location(*ast_graph, name_i),
                                   get_text_info(*ast_graph), get_location(*ast_graph, other->name_index)));
                               break;
                           };
                           case ClassDeclaration::DuplicateLocation::Member: {
                               const auto* other = meta_class_decl.find_member(name);
                               assert(other);

                               cxt.diagnostics(diag::Diagnostic::duplicate_method_name(
                                   get_text_info(*ast_graph), get_location(*ast_graph, name_i),
                                   get_text_info(*ast_graph), get_location(*ast_graph, other->name_index)));
                               break;
                           }
                           }
                       },
                       [](ast::ErrorIndex) {
                           // This means a method cannot be compiled.
                           // A diagnostic should have been produced by the parser.
                           // In future, we should add a special error type for failed methods and then add it as a
                           // posioned method.
                       },
                   },
                   ast_graph->index_to_variant(*method_it));
    }


    return { Result { std::move(class_decl), std::move(meta_class_decl) } };
}


} // sc::ir::ast2ir
