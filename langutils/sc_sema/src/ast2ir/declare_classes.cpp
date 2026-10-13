
#include "ast2ir/literal_synthesis.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/compiler.hpp"
#include "ast2ir.hpp"


namespace sc::ir::ast2ir {

void DeclareClass::append_member_to_decl(ClassDeclaration& decl, ast::ReadWriteAccessor accessor,
                                         ast::NamedIdentifierIndex name_i) {
    const auto name = cxt.symbol_table.get<SymSCIdentifier>(ast_graph.string_view(name_i));
    decl.members.push_back({ name, accessor, {}, {} });
}

void DeclareClass::append_member_to_decl(ClassDeclaration& decl, ast::ReadWriteAccessor accessor,
                                         ast::DeclareVariableWithDefaultIndex name_with_expr_i) {
    const auto [name_i, expr_i_t] = ast_graph.children(name_with_expr_i);
    ast::AnyExprIndex expr_i = expr_i_t;
    const auto name = cxt.symbol_table.get<SymSCIdentifier>(ast_graph.string_view(name_i));

    std::visit(ast2ir::ConstEval { cxt, ast_graph, ast_index }, ast_graph.index_to_variant(expr_i))
        .visit(
            [&](std::pair<ConstExprIndex, TypeInfo> p) {
                const auto [const_expr_index, type_info] = p;
                decl.members.push_back({ name, accessor, const_expr_index, type_info });
            },
            [&](Diagnostic&& d) {
                cxt.diagnostics(std::move(d));
                decl.poisoned = true;
            });
}

void DeclareClass::append_member_to_decl(ClassDeclaration&, ast::ReadWriteAccessor, ast::VariablePackAssignIndex) {
    assert(false);
}

std::optional<DeclareClass::Result> DeclareClass::operator()(ast::ClassIndex class_i) {
    const auto [class_name_i, backing_slot, super_name_i, members, methods] = ast_graph.children(class_i);

    const auto class_name_str = std::string { ast_graph.string_view(class_name_i) };
    assert(!class_name_str.empty());
    const auto class_name = cxt.symbol_table.get<SymSCClassName>(class_name_str);

    // the one class with no super.
    const bool is_root_class = class_name == cxt.common_classes.root_object;

    // if it already exists, poison it.
    if (auto loc = cxt.class_declarations.with_decl(class_name, [](ClassDeclaration& decl) {
            decl.poisoned = true;
            return decl.location;
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
    const auto meta_class_name = cxt.symbol_table.get<SymSCClassName>(std::move(meta_class_name_str));

    // Create decls
    // These are the only mutable variables in this function, it is what we build.
    ClassDeclaration class_decl { { ast_index, ast::Index { *class_i } }, class_name };
    ClassDeclaration meta_class_decl { { ast_index, ast::Index { *class_i } }, meta_class_name };

    class_decl.meta_class = meta_class_name;

    if (auto super = ast_graph.present(super_name_i)) {
        class_decl.super = cxt.symbol_table.get<SymSCClassName>(std::string { ast_graph.string_view(super_name_i) });
    } else if (is_root_class) {
        // do nothing
    } else {
        class_decl.super = cxt.common_classes.object;
    }


    // aka, if this is not abstract object
    if (class_decl.super) {
        std::string super_meta_class_name = meta_prefix + std::string { cxt.symbol_table(*class_decl.super) };
        meta_class_decl.super = cxt.symbol_table.get<SymSCClassName>(std::move(super_meta_class_name));
    } else {
        // META_AbstractObject inherits from Class
        meta_class_decl.super = cxt.common_classes.class_;
    }

    // Add consts
    for (auto member_list_iter = ast_graph.children(members).iter(); member_list_iter; ++member_list_iter) {
        const auto member_var = ast_graph.index_to_variant(*member_list_iter);
        std::visit( //
            util::overload {
                [](auto) {},
                [&](ast::DeclareConstListIndex list_i) {
                    for (auto class_var_it = ast_graph.children(list_i).iter(); class_var_it; ++class_var_it) {
                        const auto [accessor_i, member_node] = ast_graph.children(*class_var_it);
                        auto accessor = synthesize_literal(accessor_i, ast_graph);

                        if (has(accessor, ast::ReadWriteAccessor::ExternallyWriteable)) {
                            // We remove the writable flag and carry on compiling.
                            // TODO: create a diagnostic, this is an error, but not a fatal one.
                            accessor = remove(accessor, ast::ReadWriteAccessor::ExternallyWriteable);
                        }

                        std::visit([&](auto i) { this->append_member_to_decl(meta_class_decl, accessor, i); },
                                   ast_graph.index_to_variant(member_node));
                    }
                } //
            },
            member_var);
    }
    // Add consts
    for (auto member_list_iter = ast_graph.children(members).iter(); member_list_iter; ++member_list_iter) {
        const auto member_var = ast_graph.index_to_variant(*member_list_iter);
        std::visit( //
            util::overload {
                [](auto) {},
                [&](ast::DeclareMemberListIndex list_i) {
                    for (auto class_var_it = ast_graph.children(list_i).iter(); class_var_it; ++class_var_it) {
                        const auto [accessor_i, member_i] = ast_graph.children(*class_var_it);
                        const auto accessor = synthesize_literal(accessor_i, ast_graph);

                        std::visit([&](auto i) { this->append_member_to_decl(class_decl, accessor, i); },
                                   ast_graph.index_to_variant(member_i));
                    }
                },
                [&](ast::DeclareClassMemberListIndex list_i) {
                    for (auto class_var_it = ast_graph.children(list_i).iter(); class_var_it; ++class_var_it) {
                        const auto [accessor_i, member_i] = ast_graph.children(*class_var_it);
                        const auto accessor = synthesize_literal(accessor_i, ast_graph);

                        std::visit([&](auto i) { this->append_member_to_decl(meta_class_decl, accessor, i); },
                                   ast_graph.index_to_variant(member_i));
                    }
                },

            },
            member_var);
    }


    return { { std::move(class_decl), std::move(meta_class_decl) } };
}


} // sc::ir::ast2ir
