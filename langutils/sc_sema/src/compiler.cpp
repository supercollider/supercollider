#include "sc_diagnostic/sc_diagnostic.hpp"

#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"

#include "sc_sema/compiler.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/decls.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/ir.hpp"

#include "ast2ir.hpp"

#include <memory>
#include <unordered_map>


namespace sc::ir {


struct ModuleExtentions {
    [[nodiscard]] bool encountered(ModuleIndex mod, SymSCClassName cl, SymSCSelector sel) {
        std::scoped_lock lk { m_lock };
        auto& m = m_ext[mod];
        auto& c = m[cl];
        if (auto fnd = c.find(sel); fnd != c.end()) {
            return false;
        }
        c.insert(sel);
        return true;
    }

private:
    std::mutex m_lock;
    using M =
        std::unordered_map<SymSCClassName, std::unordered_set<SymSCSelector, std::hash<Symbol>>, std::hash<Symbol>>;
    std::unordered_map<ModuleIndex, M> m_ext {};
};


std::vector<sc::diag::Diagnostic> IRCompiler::compile_class_library( //
    const IntrinsicConstraints& intrinsic_constraints, //
    size_t num_threads) //
{
    (void)num_threads; // Later we should use a thread pool.

    if (m_compile_has_been_called) {
        // slowest memory order is fine here. This shouldn't happen
        assert(false);
        std::terminate();
    }
    m_compile_has_been_called = true;

    // Declare classes
    {
        size_t i { 0 };
        for (const auto& graph_and_index : ctx.class_library_ast) {
            const auto& graph = graph_and_index.graph;

            const auto classes_or_extensions = graph->children(graph_and_index.index);

            for (auto child { classes_or_extensions.iter() }; child; ++child) {
                if (const auto class_i = graph->as_a<ast::ClassIndex>(*child)) {
                    auto result =
                        ast2ir::DeclareClass { ctx, graph, ASTGraphIndex::from(i), graph_and_index.module }(*class_i);
                    if (result) {
                        auto& c = result->class_;
                        auto& m = result->meta_class_;

                        const auto valid_c = ctx.class_declarations.register_class_declaration(c);
                        const auto valid_m = ctx.class_declarations.register_class_declaration(m);
                    }
                }
            }
            i += 1;
        }
    }

    // add extentsion methods to decl
    {
        ModuleExtentions exts {};

        size_t i { 0 };
        for (const auto& graph_and_index : ctx.class_library_ast) {
            const auto& ast_graph_ptr = graph_and_index.graph;
            const auto& ast_graph = *ast_graph_ptr;

            const auto classes_or_extensions = ast_graph.children(graph_and_index.index);

            for (auto child { classes_or_extensions.iter() }; child; ++child) {
                if (const auto ext_i = ast_graph.as_a<ast::ClassExtensionIndex>(*child)) {
                    const auto [class_name_i, method_list_i] = ast_graph.children(*ext_i);

                    // Inserts methods
                    const auto class_name = ctx.symbol_table.get_class_name(ast_graph.string(class_name_i));
                    ctx.class_declarations.with_decl(
                        class_name,
                        [&, method_list_i = method_list_i](ClassDeclaration& decl) {
                            for (auto method_it = ast_graph.children(method_list_i).iter(); method_it; ++method_it) {
                                if (auto meth_i = ast_graph.as_a<ast::MethodIndex>(*method_it)) {
                                    const auto [selector, args, maybe_prim, body] = ast_graph.children(*meth_i);
                                    const auto selector_sym = ctx.symbol_table.get_selector(ast_graph.string(selector));

                                    if (!exts.encountered(graph_and_index.module, class_name, selector_sym)) {
                                        const auto* ptr = decl.find_method(selector_sym);
                                        assert(ptr); // Encountered only checks extentions methods, so we know its a
                                                     // method.
                                        ctx.diagnostics(sc::diag::Diagnostic::duplicate_extention(
                                            ast_graph.text_info(), ast_graph.location(selector),
                                            ptr->ast_graph->text_info(), ptr->ast_graph->location(ptr->name_index)));
                                        continue;
                                    }

                                    const auto r = decl.insert(ClassDeclaration::Method {
                                        ast_graph_ptr, //
                                        graph_and_index.module, //
                                        *meth_i, //
                                        selector_sym, //
                                        ast::Index::from(*selector), //
                                        true, //
                                    });
                                    switch (r) {
                                    case ClassDeclaration::MethodInsertResult::Ok:
                                        break;
                                    case ClassDeclaration::MethodInsertResult::DuplicatedByMethod: {
                                        // TODO: I don't think this is possible.
                                        const auto* ptr = decl.find_method(selector_sym);

                                        ctx.diagnostics(sc::diag::Diagnostic::duplicate_method_name(
                                            ast_graph.text_info(), ast_graph.location(selector),
                                            ptr->ast_graph->text_info(), ptr->ast_graph->location(ptr->name_index)));
                                        break;
                                    }
                                    case ClassDeclaration::MethodInsertResult::DuplicatedByExtentionMethod: {
                                        const auto* ptr = decl.find_method(selector_sym);
                                        assert(ptr); // shouldn't have told us this error if doesn't exist.
                                        ctx.diagnostics(sc::diag::Diagnostic::duplicate_extention(
                                            ast_graph.text_info(), ast_graph.location(selector),
                                            ptr->ast_graph->text_info(), ptr->ast_graph->location(ptr->name_index)));
                                        break;
                                    }
                                    case ClassDeclaration::MethodInsertResult::DuplicatedByAccessor: {
                                        const auto* member = decl.find_member(selector_sym);
                                        assert(member);

                                        ctx.diagnostics(sc::diag::Diagnostic::duplicate_method_name(
                                            ast_graph.text_info(), ast_graph.location(selector),
                                            decl.ast_graph()->text_info(),
                                            decl.ast_graph()->location(member->name_index)));
                                        break;
                                    }
                                    }
                                }
                            }
                        } //
                    );


                    // Inserts class methods
                    const auto meta_class_name = ctx.symbol_table.get_meta_class_name(ast_graph.string(class_name_i));
                    ctx.class_declarations.with_decl(
                        meta_class_name,
                        [&, method_list_i = method_list_i](ClassDeclaration& decl) {
                            for (auto method_it = ast_graph.children(method_list_i).iter(); method_it; ++method_it) {
                                if (auto meth_i = ast_graph.as_a<ast::ClassMethodIndex>(*method_it)) {
                                    const auto [selector, args, maybe_prim, body] = ast_graph.children(*meth_i);
                                    const auto selector_sym = ctx.symbol_table.get_selector(ast_graph.string(selector));
                                    if (!exts.encountered(graph_and_index.module, meta_class_name, selector_sym)) {
                                        const auto* ptr = decl.find_method(selector_sym);
                                        assert(ptr); // Encountered only checks extentions methods, so we know its a
                                                     // method.
                                        ctx.diagnostics(sc::diag::Diagnostic::duplicate_extention(
                                            ast_graph.text_info(), ast_graph.location(selector),
                                            ptr->ast_graph->text_info(), ptr->ast_graph->location(ptr->name_index)));
                                        continue;
                                    }

                                    const auto r = decl.insert(ClassDeclaration::Method {
                                        ast_graph_ptr, //
                                        graph_and_index.module, //
                                        *meth_i, //
                                        selector_sym, //
                                        ast::Index::from(*selector), //
                                        true, //
                                    });
                                    switch (r) {
                                    case ClassDeclaration::MethodInsertResult::Ok:
                                        break;
                                    case ClassDeclaration::MethodInsertResult::DuplicatedByMethod: {
                                        // TODO: I don't think this is possible.
                                        const auto* ptr = decl.find_method(selector_sym);

                                        ctx.diagnostics(sc::diag::Diagnostic::duplicate_method_name(
                                            ast_graph.text_info(), ast_graph.location(selector),
                                            ptr->ast_graph->text_info(), ptr->ast_graph->location(ptr->name_index)));
                                        break;
                                    }
                                    case ClassDeclaration::MethodInsertResult::DuplicatedByExtentionMethod: {
                                        const auto* ptr = decl.find_method(selector_sym);
                                        assert(ptr); // shouldn't have told us this error if doesn't exist.
                                        ctx.diagnostics(sc::diag::Diagnostic::duplicate_extention(
                                            ast_graph.text_info(), ast_graph.location(selector),
                                            ptr->ast_graph->text_info(), ptr->ast_graph->location(ptr->name_index)));
                                        break;
                                    }
                                    case ClassDeclaration::MethodInsertResult::DuplicatedByAccessor: {
                                        const auto* member = decl.find_member(selector_sym);
                                        assert(member);

                                        ctx.diagnostics(sc::diag::Diagnostic::duplicate_method_name(
                                            ast_graph.text_info(), ast_graph.location(selector),
                                            decl.ast_graph()->text_info(),
                                            decl.ast_graph()->location(member->name_index)));
                                        break;
                                    }
                                    }
                                }
                            }
                        } //
                    );
                }
            }
            i += 1;
        }
    }


    // Check common classes exist

    bool common_missing = false;
    ctx.common_classes.for_each([&](SymSCClassName c) {
        if (auto decl = ctx.class_declarations.get_unsafe(c); !decl) {
            ctx.diagnostics(sc::diag::Diagnostic::missing_common_class(std::string { ctx.symbol_table(c) }));
            common_missing = true;
        }
    });


    // Check intrinsics, thread safe, but probably not needed.
    std::atomic<bool> intrinsic_constraints_met { true };
    for (const auto& [k, constraint] : intrinsic_constraints) {
        // We know this is okay to multithread because the constraints are a map, so we only ever access the decl
        // one at a time and we are no longer inserting any new ones.
        auto* decl = ctx.class_declarations.get_unsafe(k);
        if (!decl) {
            ctx.diagnostics(sc::diag::Diagnostic::missing_intrinsic_class(std::string { ctx.symbol_table(k) }));
            intrinsic_constraints_met = false;
            continue;
        }

        for (auto& diags : check_intrinsic_constraint(constraint, ctx, *decl)) {
            ctx.diagnostics(std::move(diags));
            intrinsic_constraints_met = false;
            decl->poison();
        }
    }


    // It is not safe to sort if these have not been satisfied.
    if (!intrinsic_constraints_met || common_missing) {
        ctx.class_declarations.poison_all();
        return ctx.diagnostics.consume();
    }


    // Step 2: Check hierarchy makes sense.

    // Chains don't intersect, this is thread safe.
    const auto cycles = ctx.inheritance_edges.cycle_check({ *ctx.common_classes.root_object });
    for (const auto& chain : cycles) {
        // TODO: create diagnostic for inheritance chain.
        std::optional<SymSCClassName> missing { };
        for (const auto c : chain) {
            if (auto decl = ctx.class_declarations.get_unsafe(c)) {
                decl->poison();
            } else {
                // Since it isn't possible to create an edge without defining a class and you can only have one
                // superclass, there should only ever be one missing superclass within a cycle
                assert(!missing);
                missing = c;
            }
        }

        if (missing) {
            if (!ctx.symbol_table.is_meta(*missing)) {
                // super class does not exist.
                std::vector<std::pair<std::shared_ptr<const sc::lex::TextInfo>, sc::lex::SourceCodeRange>> a;

                for (SymSCClassName child : ctx.inheritance_edges.get_unsafe(*missing)) {
                    if (auto child_ptr = ctx.class_declarations.get_unsafe(child)) {
                        const auto loc = child_ptr->super() ? child_ptr->super()->second : child_ptr->name_index();
                        const auto& child_graph = *child_ptr->ast_graph();
                        a.push_back({ child_graph.text_info(), child_graph.location(loc) });
                    }
                }

                ctx.diagnostics(sc::diag::Diagnostic::super_class_not_defined(
                    std::string { ctx.symbol_table(*missing) }, std::move(a)));
            }
        } else if (chain.size() == 1) {
            // Class inherits from itself.
            if (!ctx.symbol_table.is_meta(chain[0])) {
                auto ptr = ctx.class_declarations.get_unsafe(chain[0]);
                assert(ptr);
                assert(ptr->super());
                const auto super_loc = ptr->super()->second;
                const auto& graph = *ptr->ast_graph();
                ctx.diagnostics(sc::diag::Diagnostic::self_inheritance(graph.text_info(), graph.location(super_loc)));
            }
        } else {
            // Loop

            std::vector<std::pair<std::shared_ptr<const sc::lex::TextInfo>, sc::lex::SourceCodeRange>> a;

            for (const auto c : chain) {
                auto child_ptr = ctx.class_declarations.get_unsafe(c);
                assert(child_ptr);
                const auto loc = child_ptr->super() ? child_ptr->super()->second : child_ptr->name_index();
                const auto& child_graph = *child_ptr->ast_graph();
                a.push_back({ child_graph.text_info(), child_graph.location(loc) });
            }
            ctx.diagnostics(sc::diag::Diagnostic::inheritance_cycle(std::move(a)));
        }
    }

    // Step 3: build class member layout
    std::unordered_map<SymSCClassName,
                       std::vector<std::tuple<SCMember, std::shared_ptr<const ast::ASTGraph>, ast::Index>>,
                       std::hash<Symbol>>
        class_member_layout;

    // Linear, not threadable.
    ctx.inheritance_edges.depth_first_do(ctx.common_classes.root_object, [&](SymSCClassName c) {
        const auto& decl = *ctx.class_declarations.get_unsafe(c);
        std::vector<std::tuple<SCMember, std::shared_ptr<const ast::ASTGraph>, ast::Index>> members;
        if (decl.super()) {
            // copy from parent.
            members = class_member_layout.at(decl.super()->first);
        }

        for (auto s : decl.member_order()) {
            const auto& mem = decl.find_member(s);
            SCMember ref { mem->name, mem->name_index, decl.name() };
            if (mem->expr_index) {
                ref.expr = mem->expr_index->first;
                ref.expr_i = mem->expr_index->second;
                ref.expr_type_info = *mem->type_info;
            }
            ref.accessor = mem->accessor;
            // ref.is_const = mem->
            members.push_back({ std::move(ref), decl.ast_graph(), mem->total_index });
        }
        class_member_layout.insert({ decl.name(), std::move(members) });
    });


    // Step 4: Compile all ast class nodes.

    // There is exactly one thread that accesses each class declaration here.
    // We do not insert or remove any.
    // No need to lock when mutating (poisoning).
    for (auto class_it = ctx.class_declarations.unsafe_begin(); class_it != ctx.class_declarations.unsafe_end();
         ++class_it) {
        std::shared_ptr<IRGraph> class_ir = std::make_shared<IRGraph>();
        auto& [name, decl] = *class_it;

        const auto member_list_i = class_ir->create(SCMemberList {}, { decl.ast_graph(), decl.name_index() });
        for (const auto& [m, g, i] : class_member_layout[name]) {
            const auto member = class_ir->create(std::move(m), { g, i });
            class_ir->append(member_list_i, member);
        }


        const auto method_list_i = class_ir->create(SCMethodList {}, { decl.ast_graph(), decl.name_index() });

        SCClass class_node {};
        if (auto slot = decl.slot_def()) {
            const auto [n, i] = *slot;
            if (n == ctx.symbol_table.get_identifier("slot")) {
                // already a slot
            } else if (n == ctx.symbol_table.get_identifier("float")) {
                class_node.memory_layout = SCClass::MemoryLayout::Float;
            } else if (n == ctx.symbol_table.get_identifier("int")) {
                class_node.memory_layout = SCClass::MemoryLayout::Int;
            } else {
                assert(false); // should be guarded in the declaration phase.
                decl.poison();
            }
        }
        class_node.meta_class = decl.meta_class();

        const auto class_i =
            class_ir->create(std::move(class_node), { decl.ast_graph(), decl.index() }, member_list_i, method_list_i);

        for (auto meth_it = decl.unsafe_methods_begin(); meth_it != decl.unsafe_methods_end(); ++meth_it) {
            const auto r = ast2ir::Method { *class_ir, meth_it->second.ast_graph, ctx }(meth_it->second.ast_node_index);
            class_ir->append(method_list_i, r.ir_node);
        }

        {
            std::scoped_lock lk { m_classes_lock };
            assert(m_classes.count(name) == 0);
            m_classes.insert({ name, class_ir });
        }
    }


    return ctx.diagnostics.consume();
}
}
