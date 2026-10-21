#include "boost/asio/post.hpp"
#include "boost/asio/thread_pool.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"

#include "ast2ir/method.hpp"
#include "sc_parser/ast.hpp"

#include "ast2ir/declare_classes.hpp"

#include "sc_sema/compiler.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/ir.hpp"

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
    std::unordered_map<ModuleIndex, M> m_ext { };
};


std::vector<sc::diag::Diagnostic> IRCompiler::compile_class_library( //
    const IntrinsicConstraints& intrinsic_constraints, //
    size_t num_threads) //
{
    num_threads = std::max<size_t>(1, num_threads);

    {
        boost::asio::thread_pool pool { num_threads };
        if (m_compile_has_been_called) {
            // slowest memory order is fine here. This shouldn't happen
            assert(false);
            std::terminate();
        }
        m_compile_has_been_called = true;

        std::atomic<size_t> ast_it { 0 };
        const auto process_graph = [&, max = ctx.class_library_ast.size()]() {
            while (true) {
                const auto index = ast_it.fetch_add(1, std::memory_order_relaxed);
                if (index >= max)
                    return;

                const auto& opt = ctx.class_library_ast[index];
                if (!opt)
                    continue;
                const auto& graph_and_index = *opt;

                const auto& graph = graph_and_index.graph;

                const auto classes_or_extensions = graph->children(graph_and_index.index);

                for (auto child { classes_or_extensions.iter() }; child; ++child) {
                    if (const auto class_i = graph->as_a<ast::ClassIndex>(*child)) {
                        auto result = ast2ir::DeclareClass { ctx, graph, ASTGraphIndex::from(index),
                                                             graph_and_index.module }(*class_i);
                        if (result) {
                            auto& c = result->class_;
                            auto& m = result->meta_class_;

                            ctx.class_declarations.register_class_declaration(c);
                            ctx.class_declarations.register_class_declaration(m);
                        }
                    }
                }
            }
        };
        for (size_t n { 0 }; n < num_threads; ++n)
            boost::asio::post(pool, process_graph);
        pool.join();
    }


    // add extentsion methods to decl
    {
        boost::asio::thread_pool pool { num_threads };
        ModuleExtentions exts { };

        for (const auto& opt : ctx.class_library_ast) {
            if (!opt)
                continue;
            const auto& graph_and_index = *opt;
            const auto& ast_graph_ptr = graph_and_index.graph;
            const auto& ast_graph = *ast_graph_ptr;
            const auto module = graph_and_index.module;

            const auto classes_or_extensions = ast_graph.children(graph_and_index.index);

            for (auto child { classes_or_extensions.iter() }; child; ++child) {
                if (const auto ext_i = ast_graph.as_a<ast::ClassExtensionIndex>(*child)) {
                    const auto [class_name_i, method_list_i_t] = ast_graph.children(*ext_i);
                    const auto method_list_i = method_list_i_t;


                    // TODO: this is a lot of fucking code, it made sense before they were turned into lambda for the
                    // thread pool. Refactor.

                    // Inserts methods
                    const auto class_name = ctx.symbol_table.get_class_name(ast_graph.string(class_name_i));

                    auto& ctx2 = ctx;
                    const auto on_decl = [&ctx2, &exts, module, ast_graph = ast_graph_ptr, class_name,
                                          method_list_i]() {
                        ctx2.class_declarations.with_decl(
                            class_name,
                            [&, method_list_i = method_list_i](ClassDeclaration& decl) {
                                for (auto method_it = ast_graph->children(method_list_i).iter(); method_it;
                                     ++method_it) {
                                    if (auto meth_i = ast_graph->as_a<ast::MethodIndex>(*method_it)) {
                                        const auto [selector, args, maybe_prim, body] = ast_graph->children(*meth_i);
                                        const auto selector_sym =
                                            ctx2.symbol_table.get_selector(ast_graph->string(selector));

                                        if (!exts.encountered(module, class_name, selector_sym)) {
                                            const auto* ptr = decl.find_method(selector_sym);
                                            assert(ptr); // Encountered only checks extentions methods, so we know its a
                                                         // method.
                                            ctx2.diagnostics(sc::diag::Diagnostic::duplicate_extention(
                                                ast_graph->text_info(), ast_graph->location(selector),
                                                ptr->ast_graph->text_info(),
                                                ptr->ast_graph->location(ptr->name_index)));
                                            continue;
                                        }

                                        const auto r = decl.insert(ClassDeclaration::Method {
                                            ast_graph, //
                                            module, //
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

                                            ctx2.diagnostics(sc::diag::Diagnostic::duplicate_method_name(
                                                ast_graph->text_info(), ast_graph->location(selector),
                                                ptr->ast_graph->text_info(),
                                                ptr->ast_graph->location(ptr->name_index)));
                                            break;
                                        }
                                        case ClassDeclaration::MethodInsertResult::DuplicatedByExtentionMethod: {
                                            const auto* ptr = decl.find_method(selector_sym);
                                            assert(ptr); // shouldn't have told us this error if doesn't exist.
                                            ctx2.diagnostics(sc::diag::Diagnostic::duplicate_extention(
                                                ast_graph->text_info(), ast_graph->location(selector),
                                                ptr->ast_graph->text_info(),
                                                ptr->ast_graph->location(ptr->name_index)));
                                            break;
                                        }
                                        case ClassDeclaration::MethodInsertResult::DuplicatedByAccessor: {
                                            const auto* member = decl.find_member(selector_sym);
                                            assert(member);

                                            ctx2.diagnostics(sc::diag::Diagnostic::duplicate_method_name(
                                                ast_graph->text_info(), ast_graph->location(selector),
                                                decl.ast_graph()->text_info(),
                                                decl.ast_graph()->location(member->name_index)));
                                            break;
                                        }
                                        }
                                    }
                                }
                            } //
                        );
                    };


                    // Inserts class methods
                    const auto meta_class_name = ctx.symbol_table.get_meta_class_name(ast_graph.string(class_name_i));


                    const auto on_meta_decl = [&ctx2, &exts, module, ast_graph = ast_graph_ptr, meta_class_name,
                                               method_list_i]() {
                        ctx2.class_declarations.with_decl(
                            meta_class_name,
                            [&, method_list_i = method_list_i](ClassDeclaration& decl) {
                                for (auto method_it = ast_graph->children(method_list_i).iter(); method_it;
                                     ++method_it) {
                                    if (auto meth_i = ast_graph->as_a<ast::ClassMethodIndex>(*method_it)) {
                                        const auto [selector, args, maybe_prim, body] = ast_graph->children(*meth_i);
                                        const auto selector_sym =
                                            ctx2.symbol_table.get_selector(ast_graph->string(selector));
                                        if (!exts.encountered(module, meta_class_name, selector_sym)) {
                                            const auto* ptr = decl.find_method(selector_sym);
                                            assert(ptr); // Encountered only checks extentions methods, so we know its a
                                                         // method.
                                            ctx2.diagnostics(sc::diag::Diagnostic::duplicate_extention(
                                                ast_graph->text_info(), ast_graph->location(selector),
                                                ptr->ast_graph->text_info(),
                                                ptr->ast_graph->location(ptr->name_index)));
                                            continue;
                                        }

                                        const auto r = decl.insert(ClassDeclaration::Method {
                                            ast_graph, //
                                            module, //
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

                                            ctx2.diagnostics(sc::diag::Diagnostic::duplicate_method_name(
                                                ast_graph->text_info(), ast_graph->location(selector),
                                                ptr->ast_graph->text_info(),
                                                ptr->ast_graph->location(ptr->name_index)));
                                            break;
                                        }
                                        case ClassDeclaration::MethodInsertResult::DuplicatedByExtentionMethod: {
                                            const auto* ptr = decl.find_method(selector_sym);
                                            assert(ptr); // shouldn't have told us this error if doesn't exist.
                                            ctx2.diagnostics(sc::diag::Diagnostic::duplicate_extention(
                                                ast_graph->text_info(), ast_graph->location(selector),
                                                ptr->ast_graph->text_info(),
                                                ptr->ast_graph->location(ptr->name_index)));
                                            break;
                                        }
                                        case ClassDeclaration::MethodInsertResult::DuplicatedByAccessor: {
                                            const auto* member = decl.find_member(selector_sym);
                                            assert(member);

                                            ctx2.diagnostics(sc::diag::Diagnostic::duplicate_method_name(
                                                ast_graph->text_info(), ast_graph->location(selector),
                                                decl.ast_graph()->text_info(),
                                                decl.ast_graph()->location(member->name_index)));
                                            break;
                                        }
                                        }
                                    }
                                }
                            } //
                        );
                    };

                    boost::asio::post(pool, on_decl);
                    boost::asio::post(pool, on_meta_decl);
                }
            }
        }

        pool.join(); // ext is destroyed after this scope.
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
        } else {
            for (auto& diags : check_intrinsic_constraint(constraint, ctx, *decl)) {
                ctx.diagnostics(std::move(diags));
                intrinsic_constraints_met = false;
                decl->poison();
            }
        }
    }


    // It is not safe to sort if these have not been satisfied.
    if (!intrinsic_constraints_met || common_missing) {
        ctx.class_declarations.poison_all();
        return ctx.diagnostics.consume();
    }


    // Step 2: Check hierarchy makes sense.

    // Chains don't intersect, this is thread safe.
    {
        boost::asio::thread_pool pool { num_threads };
        const auto cycles =
            ctx.inheritance_edges.cycle_check({ *ctx.common_classes.root_object }, m_topological_ordering);
        for (const auto& chain : cycles) {
            const auto process = [&]() {
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
                                const auto loc =
                                    child_ptr->super() ? child_ptr->super()->second : child_ptr->name_index();
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
                        ctx.diagnostics(
                            sc::diag::Diagnostic::self_inheritance(graph.text_info(), graph.location(super_loc)));
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
            };

            boost::asio::post(pool, process);
        }

        pool.join();
    }

    // Step 3: build class member layout
    // Must be done in order, cannot multithread.
    using MemberEntry = std::tuple<SCMember, std::shared_ptr<const ast::ASTGraph>, ast::Index>;
    std::unordered_map<SymSCClassName, std::vector<MemberEntry>, std::hash<Symbol>> class_member_layout;

    for (SymSCClassName c : m_topological_ordering) {
        const auto& decl = *ctx.class_declarations.get_unsafe(c);
        std::vector<MemberEntry> members;
        if (decl.super()) {
            // copy from parent.
            members = class_member_layout.at(decl.super()->first);
        }

        std::unordered_set<SymSCIdentifier, std::hash<Symbol>> names;
        for (const auto& p : members) {
            names.insert(std::get<0>(p).name);
        }


        for (auto s : decl.member_order()) {
            const auto& mem = decl.find_member(s);
            if (names.count(mem->name)) {
                // Duplicate with base.
                const auto fnd = std::find_if(members.begin(), members.end(), [n = mem->name](const MemberEntry& m) {
                    return n == std::get<0>(m).name;
                });
                assert(fnd != members.end());

                ctx.diagnostics(diag::Diagnostic::duplicate_member_in_base(
                    std::get<1>(*fnd)->text_info(), std::get<1>(*fnd)->location(std::get<2>(*fnd)),
                    decl.ast_graph()->text_info(), decl.ast_graph()->location(mem->name_index)));

                // This avoids pushing the variable and the code should revert to the old behaviour.
                continue;
            }
            SCMember ref { mem->name, mem->name_index, decl.name() };
            if (mem->expr_index) {
                ref.expr = mem->expr_index->first;
                ref.expr_i = mem->expr_index->second;
                ref.expr_type_info = *mem->type_info;
            }
            ref.accessor = mem->accessor;
            // ref.is_const = mem->
            members.push_back({ std::move(ref), decl.ast_graph(), mem->name_index });
        }
        class_member_layout.insert({ decl.name(), std::move(members) });
    }

    // Step 4: Compile all ast class nodes.

    // There is exactly one thread that accesses each class declaration here.
    // We do not insert or remove any.
    // No need to lock when mutating (poisoning).
    {
        boost::asio::thread_pool pool { num_threads };
        for (auto class_it = ctx.class_declarations.unsafe_begin(); class_it != ctx.class_declarations.unsafe_end();
             ++class_it) {
            const auto process = [&, class_it]() {
                std::shared_ptr<IRGraph> class_ir = std::make_shared<IRGraph>();
                auto& [name, decl] = *class_it;

                // Shared across threads, so must not insert. Poisoned classes have no layout.
                const auto make_member_list = [&class_ir, &class_member_layout, decl_graph = decl.ast_graph(),
                                               decl_name_i = decl.name_index()](SymSCClassName c) {
                    const auto list_i = class_ir->create(SCMemberList {}, { decl_graph, decl_name_i });
                    if (const auto fnd = class_member_layout.find(c); fnd != class_member_layout.end()) {
                        for (const auto& [m, g, i] : fnd->second) {
                            const auto member = class_ir->create(SCMember { m }, { g, i });
                            class_ir->append(list_i, member);
                        }
                    }
                    return list_i;
                };

                const auto member_list_i = make_member_list(name);

                // The meta class's members are compiled into its own graph, so the class's methods get a copy here
                // to resolve class variables against.
                std::optional<SCMemberList_I> meta_member_list_i;
                if (const auto meta = decl.meta_class())
                    meta_member_list_i = make_member_list(*meta);


                const auto method_list_i = class_ir->create(SCMethodList { }, { decl.ast_graph(), decl.name_index() });

                SCClass class_node { name };
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

                const auto class_i = class_ir->create(std::move(class_node), { decl.ast_graph(), decl.index() },
                                                      member_list_i, method_list_i);

                class_ir->add_root(class_i);

                for (auto meth_it = decl.unsafe_methods_begin(); meth_it != decl.unsafe_methods_end(); ++meth_it) {
                    const auto r = ast2ir::Method { *class_ir, meth_it->second.ast_graph, ctx, meta_member_list_i,
                                                    member_list_i }(meth_it->second.ast_node_index);
                    class_ir->append(method_list_i, r.ir_node);
                }

                {
                    std::scoped_lock lk { m_classes_lock };
                    assert(m_classes.count(name) == 0);
                    m_classes.insert({ name, class_ir });
                }
            };

            boost::asio::post(pool, process);
        }

        pool.join();
    }


    return ctx.diagnostics.consume();
}
}
