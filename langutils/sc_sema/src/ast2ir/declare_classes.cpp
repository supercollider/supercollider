
#include "sc_parser/ast.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/compiler.hpp"
#include "ast2ir.hpp"

const static std::string meta_prefix = "Meta_";

namespace sc::ir::ast2ir {

void declare_class(const ast::ASTGraph& ast, ASTGraphIndex ast_id, Context& cxt, const CommonClasses& common_classes,
                   ast::ClassIndex class_i) {
    const auto [class_name_i, backing_slot, super_name_i, members, methods] = ast.children(class_i);

    const auto class_name_str = std::string { ast.string_view(class_name_i) };
    assert(!class_name_str.empty());
    const auto class_name = cxt.symbol_table.get<SymSCClassName>(class_name_str);

    // the one class with no super.
    const bool is_root_class = class_name == common_classes.root_object;

    // if it already exists, poison it.
    if (auto loc = cxt.class_declarations.with_decl(class_name, [](ClassDeclaration& decl) {
            decl.poisoned = true;
            return decl.location;
        })) {
        const auto& other_ast = cxt.ast_of_class_library()[*loc->graph_index].first;

        cxt.diagnostics( //
            Diagnostic::duplicate_class( //
                other_ast.text_info(), other_ast.location(loc->node_index), ast.text_info(),
                ast.location(class_i)) //
        );
        return;
    }

    auto meta_class_name_str = meta_prefix + class_name_str;
    const auto meta_class_name = cxt.symbol_table.get<SymSCClassName>(std::move(meta_class_name_str));

    // Create decls
    // These are the only mutable variables in this function, it is what we build.
    ClassDeclaration class_decl { { ast_id, ast::Index { *class_i } }, class_name };
    ClassDeclaration meta_class_decl { { ast_id, ast::Index { *class_i } }, meta_class_name };

    class_decl.meta_class = meta_class_name;

    if (auto super = ast.present(super_name_i)) {
        class_decl.super = cxt.symbol_table.get<SymSCClassName>(std::string { ast.string_view(super_name_i) });
    } else if (is_root_class) {
        // do nothing
    } else {
        class_decl.super = common_classes.object;
    }


    // aka, if this is not abstract object
    if (class_decl.super) {
        std::string super_meta_class_name = meta_prefix + std::string { cxt.symbol_table(*class_decl.super) };
        meta_class_decl.super = cxt.symbol_table.get<SymSCClassName>(std::move(super_meta_class_name));
    } else {
        // META_AbstractObject inherits from Class
        meta_class_decl.super = common_classes.class_;
    }

    // TODO: First, hunt for consteval methods


    // Second Declare members.
    // This loop is a little awkward because we have a list of lists of three types of members.
    for (auto member_list_iter = ast.children(members).iter(); member_list_iter; ++member_list_iter) {
        // visitors
        const auto append_member_to_decl = [&](ClassDeclaration& decl, ast::ReadWriteAccessor accessor) {
            return util::overload {
                [&](ast::NamedIdentifierIndex name_i) {
                    const auto name = cxt.symbol_table.get<SymSCIdentifier>(ast.string_view(name_i));
                    decl.members.push_back({ name, accessor, {}, {} });
                },

                [&](ast::DeclareVariableWithDefaultIndex name_with_expr_i) {
                    const auto [name_i, expr_i] = ast.children(name_with_expr_i);
                    const auto name = cxt.symbol_table.get<SymSCIdentifier>(ast.string_view(name_i));

                    ast2ir::ConstEval { cxt, ast, ast_id }(expr_i).visit(
                        [&](std::pair<ConstExprIndex, TypeInfo> p) {
                            const auto [const_expr_index, type_info] = p;
                            decl.members.push_back({ name, accessor, const_expr_index, type_info });
                        },
                        [&](Diagnostic&& d) {
                            cxt.diagnostics(std::move(d));
                            class_decl.poisoned = true;
                        });
                },
            };
        };

        const auto visit_each_list = util::overload {
            //
            [&](ast::DeclareMemberListIndex list_i) {
                for (auto class_var_it = ast.children(list_i).iter(); class_var_it; ++class_var_it) {
                    const ast::DeclareClassVar class_var { ast.payload(*class_var_it) };
                    const auto [member_node] = ast.children(*class_var_it);

                    std::visit(append_member_to_decl(class_decl, class_var.accessor),
                               ast.index_to_variant(member_node));
                }
            },
            [&](ast::DeclareClassMemberListIndex list_i) {
                for (auto class_var_it = ast.children(list_i).iter(); class_var_it; ++class_var_it) {
                    const ast::DeclareClassVar class_var { ast.payload(*class_var_it) };
                    const auto [member_node] = ast.children(*class_var_it);

                    std::visit(append_member_to_decl(meta_class_decl, class_var.accessor),
                               ast.index_to_variant(member_node));
                }
            },
            [&](ast::DeclareConstListIndex list_i) {
                for (auto class_var_it = ast.children(list_i).iter(); class_var_it; ++class_var_it) {
                    const ast::DeclareClassVar class_var { ast.payload(*class_var_it) };
                    const auto [member_node] = ast.children(*class_var_it);
                    auto accessor = class_var.accessor;

                    if (has(accessor, ast::ReadWriteAccessor::ExternallyWriteable)) {
                        // We remove the writable flag and carry on compiling.
                        // TODO: create a diagnostic, this is an error, but not a fatal one.
                        accessor = remove(accessor, ast::ReadWriteAccessor::ExternallyWriteable);
                    }

                    std::visit(
                        append_member_to_decl(meta_class_decl,
                                              combine(class_var.accessor, ast::ReadWriteAccessor::InternallyWrite)),
                        ast.index_to_variant(member_node));
                }
            }
        };

        // Here we actually do the visit.
        std::visit(visit_each_list, ast.index_to_variant(*member_list_iter));
    }


    cxt.class_declarations.register_class_declaration(std::move(class_decl));
    cxt.class_declarations.register_class_declaration(std::move(meta_class_decl));
}

}
