#include "boost/asio/post.hpp"
#include "boost/asio/thread_pool.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"

#include "ast2ir/method.hpp"
#include "ast2ir/region.hpp"
#include "sc_parser/ast.hpp"

#include "ast2ir/declare_classes.hpp"

#include "sc_sema/compiler.hpp"
#include "sc_sema/source_location.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/ir.hpp"
#include "ast2ir/ast2ir_base.hpp"

#include "sc_parser/ast_untyped.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>


namespace sc::ir {


std::vector<sc::diag::Diagnostic> IRCompiler::compile_class_library( //
    const IntrinsicConstraints& intrinsic_constraints, //
    size_t num_threads) //
{
    num_threads = std::max<size_t>(1, num_threads);

    // The files' classes and extensions in a fixed order, by module, then file, then position, as when two define the
    // same class or method, which is used depends on it. The classes are declared on several threads, so the first
    // definition of each is found beforehand.
    struct Defined {
        const SourceEntry* entry;
        ast::Index index;
        std::string path;
        sc::lex::SourceCodeLocation begin;
    };
    std::vector<Defined> classes;
    std::vector<Defined> extensions;
    for (const auto& opt : ctx.class_library_ast) {
        if (!opt)
            continue;
        const auto& g = *opt->graph;
        const auto& path = g.text_info()->file_path;
        for (auto child = g.children(opt->index).iter(); child; ++child) {
            const ast::Index i { **child };
            const Defined defined { &*opt, i, path ? path->string() : std::string {}, g.location(i).begin };
            if (g.as_a<ast::ClassIndex>(i))
                classes.push_back(defined);
            else if (g.as_a<ast::ClassExtensionIndex>(i))
                extensions.push_back(defined);
        }
    }
    const auto source_order = [](const Defined& l, const Defined& r) {
        const auto l_module = *l.entry->module;
        const auto r_module = *r.entry->module;
        return std::tie(l_module, l.path, l.begin) < std::tie(r_module, r.path, r.begin);
    };
    std::sort(classes.begin(), classes.end(), source_order);
    std::sort(extensions.begin(), extensions.end(), source_order);
    for (const auto& defined : classes) {
        const auto& g = defined.entry->graph;
        const auto [class_name_i, backing_slot, super_name_i, members, methods] =
            g->children(*g->as_a<ast::ClassIndex>(defined.index));
        ctx.class_names.define(ctx.symbol_table.get_class_name(g->string(class_name_i)), g, class_name_i);
    }

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


    // Adds the extension methods to their classes, in source order, as when two define the same method, which is used
    // depends on it, see ClassDeclaration::insert. Inserting is cheap, so this isn't threaded.
    {
        for (const auto& extension : extensions) {
            const auto& ast_graph = extension.entry->graph;
            const auto extension_i = *ast_graph->as_a<ast::ClassExtensionIndex>(extension.index);
            const auto [class_name_i, method_list_i] = ast_graph->children(extension_i);
            const auto version = ast_graph->payload(extension_i).version;
            const auto class_name = ctx.symbol_table.get_class_name(ast_graph->string(class_name_i));
            const auto meta_class_name = ctx.symbol_table.get_meta_class_name(ast_graph->string(class_name_i));

            const auto insert = [&](SymSCClassName name, auto method_i) {
                // An extension of a class that isn't defined is skipped.
                auto* decl = ctx.class_declarations.get_unsafe(name);
                if (!decl)
                    return;
                const auto [selector, args, maybe_prim, body] = ast_graph->children(method_i);
                const auto r = decl->insert(ClassDeclaration::Method {
                    ast_graph, //
                    extension.entry->module, //
                    method_i, //
                    ctx.symbol_table.get_selector(ast_graph->string(selector)), //
                    ast::Index::from(*selector), //
                    version, //
                    true, //
                });
                if (!r.previous)
                    return;
                const auto& previous = *r.previous;
                const auto previous_t = previous.ast_graph->text_info();
                const auto previous_l = previous.ast_graph->location(previous.name_index);
                const auto here_t = ast_graph->text_info();
                const auto here_l = ast_graph->location(selector);
                // The last location is the method the diagnostic is about: the poisoned one that was kept for the
                // errors, the one used for the warning.
                switch (r.result) {
                case ClassDeclaration::MethodInsertResult::Ok:
                    break;
                case ClassDeclaration::MethodInsertResult::DuplicateInModule:
                    ctx.diagnostics(sc::diag::Diagnostic::duplicate_extention(here_t, here_l, previous_t, previous_l));
                    break;
                case ClassDeclaration::MethodInsertResult::OverwrittenBetweenModules:
                    ctx.diagnostics(
                        sc::diag::Diagnostic::extension_overwritten(previous_t, previous_l, here_t, here_l));
                    break;
                case ClassDeclaration::MethodInsertResult::ConflictBetweenModules:
                    ctx.diagnostics(sc::diag::Diagnostic::extension_conflict(here_t, here_l, previous_t, previous_l));
                    break;
                }
            };
            for (auto method_it = ast_graph->children(method_list_i).iter(); method_it; ++method_it) {
                if (const auto method_i = ast_graph->as_a<ast::MethodIndex>(*method_it))
                    insert(class_name, *method_i);
                else if (const auto class_method_i = ast_graph->as_a<ast::ClassMethodIndex>(*method_it))
                    insert(meta_class_name, *class_method_i);
            }
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

    // Reported in order, rather than threaded, so the diagnostics are too, there are only ever a few.
    {
        const auto cycles = ctx.inheritance_edges.cycle_check({ *ctx.common_classes.root_object },
                                                              m_topological_ordering, ctx.symbol_table);
        for (const auto& chain : cycles) {
            {
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
                } else if (!ctx.symbol_table.is_meta(chain[0])) {
                    // A loop. The meta classes form the same loop, which isn't reported again.
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
        }
    }

    // A class inheriting from a poisoned class is poisoned too, e.g., from one with an invalid slot type,
    // `Foo[bad] {}`, as is the meta class of a poisoned class. Nothing more is reported, the cause has been already.
    // The topological ordering has parents before their children, so this reaches every descendant.
    for (auto it = ctx.class_declarations.unsafe_begin(); it != ctx.class_declarations.unsafe_end(); ++it)
        if (it->second.poisoned())
            if (const auto meta = it->second.meta_class())
                if (auto* meta_decl = ctx.class_declarations.get_unsafe(*meta))
                    meta_decl->poison();
    for (const auto c : m_topological_ordering) {
        const auto* decl = ctx.class_declarations.get_unsafe(c);
        if (!decl || !decl->poisoned())
            continue;
        for (const auto child : ctx.inheritance_edges.get_unsafe(c))
            if (auto* child_decl = ctx.class_declarations.get_unsafe(child))
                child_decl->poison();
    }

    // The hierarchy is valid, so method lookups can be done.
    ctx.known_methods.check(ctx.class_declarations, ctx.common_classes, ctx.common_symbols);

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
            SCMember ref { mem->name, decl.name() };
            if (mem->expr_index) {
                ref.expr = mem->expr_index->first;
                ref.expr_type_info = *mem->type_info;
            }
            ref.accessor = mem->accessor;
            ref.is_const = mem->is_const;
            members.push_back({ std::move(ref), decl.ast_graph(), mem->name_index });
        }
        class_member_layout.insert({ decl.name(), std::move(members) });
    }

    // Step 4: Compile all ast class nodes.
    // This is done in two passes so a class's methods can refer to its meta class's graph. In the first, every class
    // ir graph is created along with its members, in the second the methods are compiled.
    // In the first pass, we read declarations and create ir graphs.
    // During the second, only one ir graph is accessed per thread.

    // There is exactly one thread that accesses each class declaration here.

    struct ClassBuild {
        std::shared_ptr<IRGraph> ir;
        SCMethodList_I method_list;
        ast2ir::Scope members;
    };
    // Every class gets an entry before the passes start, so the threads only write their own entry.
    std::unordered_map<SymSCClassName, std::optional<ClassBuild>, std::hash<Symbol>> class_builds;
    for (auto class_it = ctx.class_declarations.unsafe_begin(); class_it != ctx.class_declarations.unsafe_end();
         ++class_it)
        class_builds[class_it->first];

    // Step 4a: Create the class graphs and their members.
    {
        boost::asio::thread_pool pool { num_threads };
        for (auto class_it = ctx.class_declarations.unsafe_begin(); class_it != ctx.class_declarations.unsafe_end();
             ++class_it) {
            const auto process = [&, class_it]() {
                auto& [name, decl] = *class_it;
                // A poisoned class, e.g., one whose superclass isn't defined, has been reported already. Compiling it
                // would only report what follows from that, e.g., every member being undefined.
                if (decl.poisoned())
                    return;
                std::shared_ptr<IRGraph> class_ir = std::make_shared<IRGraph>();

                const auto member_list_i = class_ir->create(
                    SCMemberList {}, ast2ir::location_of(*class_ir, *decl.ast_graph(), decl.name_index()));
                if (const auto fnd = class_member_layout.find(name); fnd != class_member_layout.end()) {
                    for (const auto& [m, g, i] : fnd->second) {
                        const auto member = class_ir->create(SCMember { m }, ast2ir::location_of(*class_ir, *g, i));
                        class_ir->append(member_list_i, member);
                    }
                }

                const auto method_list_i = class_ir->create(
                    SCMethodList {}, ast2ir::location_of(*class_ir, *decl.ast_graph(), decl.name_index()));

                SCClass class_node { name };
                if (auto slot = decl.slot_def()) {
                    const auto [n, i] = *slot;
                    const auto& s = ctx.common_symbols;
                    using Layout = SCClass::MemoryLayout;
                    if (n == s.slot) {
                        class_node.memory_layout = Layout::Slot;
                    } else if (n == s.double_) {
                        class_node.memory_layout = Layout::Double;
                    } else if (n == s.float_) {
                        class_node.memory_layout = Layout::Float;
                    } else if (n == s.int32) {
                        class_node.memory_layout = Layout::Int32;
                    } else if (n == s.int16) {
                        class_node.memory_layout = Layout::Int16;
                    } else if (n == s.int8) {
                        class_node.memory_layout = Layout::Int8;
                    } else if (n == s.char_) {
                        class_node.memory_layout = Layout::Char;
                    } else if (n == s.symbol) {
                        class_node.memory_layout = Layout::Symbol;
                    } else {
                        assert(false); // should be guarded in the declaration phase.
                        decl.poison();
                    }
                }
                class_node.meta_class = decl.meta_class();
                class_node.version = decl.version();

                const auto class_i = class_ir->create(std::move(class_node),
                                                      ast2ir::location_of(*class_ir, *decl.ast_graph(), decl.index()),
                                                      member_list_i, method_list_i);

                class_ir->add_root(class_i);

                auto members = ast2ir::member_scope(class_ir, member_list_i);
                class_builds.at(name).emplace(ClassBuild { std::move(class_ir), method_list_i, std::move(members) });
            };

            boost::asio::post(pool, process);
        }

        pool.join();
    }

    // Step 4b: Compile the methods.
    {
        boost::asio::thread_pool pool { num_threads };
        for (auto class_it = ctx.class_declarations.unsafe_begin(); class_it != ctx.class_declarations.unsafe_end();
             ++class_it) {
            const auto process = [&, class_it]() {
                auto& [name, decl] = *class_it;
                // Poisoned classes weren't built, see 4a.
                auto& maybe_build = class_builds.at(name);
                if (!maybe_build)
                    return;
                ClassBuild& build = *maybe_build;

                const ast2ir::Scope* meta_members = nullptr;
                if (const auto meta = decl.meta_class()) {
                    if (const auto fnd = class_builds.find(*meta); fnd != class_builds.end() && fnd->second)
                        meta_members = &fnd->second->members;
                }

                // Compiled in the order they are written, by file then position, so the IR and the diagnostics
                // don't depend on how the methods are hashed. Extensions can be in other files.
                std::vector<const ClassDeclaration::Method*> methods;
                for (auto meth_it = decl.unsafe_methods_begin(); meth_it != decl.unsafe_methods_end(); ++meth_it)
                    methods.push_back(&meth_it->second);
                const auto source_order = [](const ClassDeclaration::Method* m) {
                    const auto& path = get_text_info(*m->ast_graph)->file_path;
                    return std::tuple { path ? path->string() : std::string {},
                                        get_location(*m->ast_graph, m->ast_node_index).begin };
                };
                std::sort(methods.begin(), methods.end(),
                          [&](const auto* l, const auto* r) { return source_order(l) < source_order(r); });

                for (const auto* method : methods) {
                    const auto r = ast2ir::Method {
                        *build.ir, method->ast_graph, ctx, method->version, meta_members, build.members
                    }(method->ast_node_index);
                    // Still compiled, so its own diagnostics are reported, e.g., an extension that clashes. Like any
                    // poisoned method, its result isn't known.
                    if (method->poisoned) {
                        build.ir->payload(r.ir_node).poisoned = true;
                        build.ir->payload(r.ir_node).constant_result = {};
                    }
                    build.ir->append(build.method_list, r.ir_node);
                }

                {
                    std::scoped_lock lk { m_classes_lock };
                    assert(m_classes.count(name) == 0);
                    m_classes.insert({ name, build.ir });
                }
            };

            boost::asio::post(pool, process);
        }

        pool.join();
    }


    return ctx.diagnostics.consume();
}

IRCompiler::Interpreted IRCompiler::compile_interpreted(std::shared_ptr<const ast::ASTGraph> ast_graph,
                                                        ast::LanguageVersion version) {
    assert(m_compile_has_been_called);
    const auto root = ast_graph->root();
    assert(root && !std::holds_alternative<ast::ClassOrExtensionListIndex>(*root));
    const auto root_i = std::visit([](auto i) { return ast::Index { *i }; }, *root);

    // The Interpreter's graphs, if it and functionCompileContext compiled.
    const auto interpreter = ctx.common_symbols.interpreter;
    const auto* decl = ctx.class_declarations.get_unsafe(interpreter);
    std::shared_ptr<const IRGraph> class_ir;
    std::shared_ptr<const IRGraph> meta_ir;
    if (decl && !decl->poisoned() && decl->find_method(ctx.common_symbols.function_compile_context)) {
        std::scoped_lock lk { m_classes_lock };
        if (const auto fnd = m_classes.find(interpreter); fnd != m_classes.end())
            class_ir = fnd->second;
        if (const auto meta = decl->meta_class())
            if (const auto fnd = m_classes.find(*meta); fnd != m_classes.end())
                meta_ir = fnd->second;
    }
    if (!class_ir) {
        ctx.diagnostics(sc::diag::Diagnostic::missing_interpreter_context(get_text_info(*ast_graph),
                                                                          get_location(*ast_graph, root_i)));
        return { nullptr, ctx.diagnostics.consume() };
    }

    const auto members = ast2ir::member_scope(class_ir);
    const auto meta_members = meta_ir ? std::optional { ast2ir::member_scope(meta_ir) } : std::nullopt;
    auto ir = std::make_shared<IRGraph>();
    ast2ir::Interpreted interpreted { *ir, ast_graph, ctx, version, meta_members ? &*meta_members : nullptr, members };
    const auto r = std::visit(
        util::overload { [&](ast::RegionListIndex i) { return interpreted(i); },
                         [&](ast::FileAsFunctionIndex i) { return interpreted(i); },
                         [&](ast::ClassOrExtensionListIndex) -> ast2ir::Return<SCInterpreted_I> { std::abort(); } },
        *root);
    ir->add_root(r.ir_node);
    return { std::move(ir), ctx.diagnostics.consume() };
}
}
