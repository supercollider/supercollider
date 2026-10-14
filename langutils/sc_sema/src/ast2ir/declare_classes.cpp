
#include "ast2ir/literal_synthesis.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/compiler.hpp"
#include "ast2ir.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/tables.hpp"


namespace sc::ir::ast2ir {

void DeclareClass::append_member_to_decl(ClassDeclaration& decl, ast::UntypedIndex accessor_i,
                                         ast::ReadWriteAccessor accessor, ast::NamedIdentifierIndex name_i) {
    const auto name = cxt.symbol_table.get_identifier(ast_graph.string_view(name_i));
    decl.members.push_back(ClassDeclaration::Member {
        { { ast_index, *name_i }, name }, { { ast_index, *accessor_i }, accessor }, {}, {} });
}

void DeclareClass::append_member_to_decl(ClassDeclaration& decl, ast::UntypedIndex accessor_i,
                                         ast::ReadWriteAccessor accessor,
                                         ast::DeclareVariableWithDefaultIndex name_with_expr_i) {
    const auto [name_i_t, expr_i_t] = ast_graph.children(name_with_expr_i);
    const ast::NamedIdentifierIndex name_i = name_i_t;
    ast::AnyExprIndex expr_i = expr_i_t;
    const auto name = cxt.symbol_table.get_identifier(ast_graph.string_view(name_i));

    std::visit(ast2ir::ConstEval { cxt, ast_graph, ast_index }, ast_graph.index_to_variant(expr_i))
        .visit(
            [&](std::pair<ConstExprIndex, TypeInfo> p) {
                const auto [const_expr_index, type_info] = p;

                decl.members.push_back(ClassDeclaration::Member { { { ast_index, *name_i }, name },
                                                                  { { ast_index, *accessor_i }, accessor },
                                                                  { { { ast_index, *expr_i }, const_expr_index } },
                                                                  type_info });
            },
            [&](Diagnostic&& d) {
                cxt.diagnostics(std::move(d));
                decl.poisoned = true;
            });
}

void DeclareClass::append_member_to_decl(ClassDeclaration&, ast::UntypedIndex, ast::ReadWriteAccessor,
                                         ast::VariablePackAssignIndex) {
    assert(false);
}

std::optional<DeclareClass::Result> DeclareClass::operator()(ast::ClassIndex class_i) {
    const auto [class_name_i, backing_slot, super_name_i, members, methods] = ast_graph.children(class_i);

    const auto class_name_str = std::string { ast_graph.string_view(class_name_i) };
    assert(!class_name_str.empty());
    const auto class_name = cxt.symbol_table.get_class_name(class_name_str);

    // the one class with no super.
    const bool is_root_class = class_name == cxt.common_classes.root_object;

    // if it already exists, poison it.
    if (auto loc = cxt.class_declarations.with_decl(class_name, [](ClassDeclaration& decl) {
            decl.poisoned = true;
            return decl.name.location;
        })) {
        const auto& other_ast = cxt.ast_of_class_library()[*loc->graph_index].first;

        cxt.diagnostics( //
            Diagnostic::duplicate_class( //
                other_ast.text_info(), other_ast.location(loc->node_index), ast_graph.text_info(),
                ast_graph.location(class_i)) //
        );
        return {};
    }

    auto meta_class_name_str = meta_prefix + class_name_str;
    const auto meta_class_name = cxt.symbol_table.get_class_name(std::move(meta_class_name_str));

    // Create decls
    // These are the only mutable variables in this function, it is what we build.
    const ASTLocation name_loc { ast_index, ast::Index { *class_i } };
    ClassDeclaration class_decl { { name_loc, class_name } };

    ClassDeclaration meta_class_decl { { name_loc, meta_class_name } };

    class_decl.meta_class = meta_class_name;

    if (auto super = ast_graph.present(super_name_i)) {
        const auto super_name_view = ast_graph.string_view(super_name_i);
        const auto sym = cxt.symbol_table.get_class_name(super_name_view);
        class_decl.super = { ASTLocation { ast_index, ast::Index { *super_name_i } }, sym };

        cxt.inheritance_edges.insert(sym, class_name);
    } else if (is_root_class) {
        // do nothing
    } else {
        // Location here is just white space.
        class_decl.super = { ASTLocation { ast_index, ast::Index { *super_name_i } },
                             cxt.common_classes.default_inheritor };

        cxt.inheritance_edges.insert(cxt.common_classes.default_inheritor, class_name);
    }

    // aka, if this is not abstract object
    if (class_decl.super) {
        std::string super_meta_class_name = meta_prefix + std::string { cxt.symbol_table(*class_decl.super->value) };
        const auto super_meta_class_name_sym = cxt.symbol_table.get_class_name(super_meta_class_name);
        meta_class_decl.super = { class_decl.super->location, super_meta_class_name_sym };

        cxt.inheritance_edges.insert(super_meta_class_name_sym, meta_class_name);
    } else {
        // META_AbstractObject inherits from Class
        // Basically a random location here.
        meta_class_decl.super = { meta_class_decl.name.location, cxt.common_classes.class_ };
        cxt.inheritance_edges.insert(cxt.common_classes.class_, meta_class_name);
    }

    // Add consts
    for (auto member_list_iter = ast_graph.children(members).iter(); member_list_iter; ++member_list_iter) {
        const auto member_var = ast_graph.index_to_variant(*member_list_iter);
        std::visit( //
            util::overload {
                [](auto) {},
                [&](ast::DeclareConstListIndex list_i) {
                    for (auto class_var_it = ast_graph.children(list_i).iter(); class_var_it; ++class_var_it) {
                        const auto [accessor_i_t, member_node] = ast_graph.children(*class_var_it);
                        const ast::UntypedIndex accessor_i = accessor_i_t;
                        auto accessor = synthesize_literal(accessor_i_t, ast_graph);


                        if (has(accessor, ast::ReadWriteAccessor::ExternallyWriteable)) {
                            // We remove the writable flag and carry on compiling.
                            // TODO: create a diagnostic, this is an error, but not a fatal one.
                            accessor = remove(accessor, ast::ReadWriteAccessor::ExternallyWriteable);
                        }

                        std::visit(
                            [&](auto i) { this->append_member_to_decl(meta_class_decl, accessor_i, accessor, i); },
                            ast_graph.index_to_variant(member_node));
                    }
                } //
            },
            member_var);
    }
    // Add other members
    for (auto member_list_iter = ast_graph.children(members).iter(); member_list_iter; ++member_list_iter) {
        const auto member_var = ast_graph.index_to_variant(*member_list_iter);
        std::visit( //
            util::overload {
                [](auto) {},
                [&](ast::DeclareMemberListIndex list_i) {
                    for (auto class_var_it = ast_graph.children(list_i).iter(); class_var_it; ++class_var_it) {
                        const auto [accessor_i_t, member_i] = ast_graph.children(*class_var_it);
                        const ast::UntypedIndex accessor_i = accessor_i_t;
                        const auto accessor = synthesize_literal(accessor_i_t, ast_graph);

                        std::visit([&](auto i) { this->append_member_to_decl(class_decl, accessor_i, accessor, i); },
                                   ast_graph.index_to_variant(member_i));
                    }
                },
                [&](ast::DeclareClassMemberListIndex list_i) {
                    for (auto class_var_it = ast_graph.children(list_i).iter(); class_var_it; ++class_var_it) {
                        const auto [accessor_i_t, member_i] = ast_graph.children(*class_var_it);
                        const ast::UntypedIndex accessor_i = accessor_i_t;
                        const auto accessor = synthesize_literal(accessor_i_t, ast_graph);

                        std::visit(
                            [&](auto i) { this->append_member_to_decl(meta_class_decl, accessor_i, accessor, i); },
                            ast_graph.index_to_variant(member_i));
                    }
                },

            },
            member_var);
    }


    // Add methods
    for (auto method_it = ast_graph.children(methods).iter(); method_it; ++method_it) {
        std::visit(util::overload {
                       [&](ast::MethodIndex i) {
                           const auto [name_i, args_i, maybe_prim_i, body_i] = ast_graph.children(i);
                           const auto name = cxt.symbol_table.get_selector(ast_graph.string_view(name_i));
                           ClassDeclaration::Method m { { { ast_index, *name_i }, name } };
                           class_decl.methods.insert({ name, m });
                       },
                       [&](ast::ClassMethodIndex i) {
                           const auto [name_i, args_i, maybe_prim_i, body_i] = ast_graph.children(i);
                           const auto name = cxt.symbol_table.get_selector(ast_graph.string_view(name_i));
                           ClassDeclaration::Method m { { { ast_index, *name_i }, name } };
                           meta_class_decl.methods.insert({ name, m });
                       },
                       [](ast::ErrorIndex) {
                           // This means a method cannot be compiled.
                           // A diagnostic should have been produced by the parser.
                       },
                   },
                   ast_graph.index_to_variant(*method_it));
    }


    return { { std::move(class_decl), std::move(meta_class_decl) } };
}


} // sc::ir::ast2ir
