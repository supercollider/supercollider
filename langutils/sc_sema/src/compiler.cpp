#include "sc_parser/indexes_typed.hpp"

#include "sc_sema/compiler.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/tables.hpp"

#include "ast2ir.hpp"

#include <exception>
#include <unordered_map>

namespace sc::ir {

CommonClasses symbol_table_and_common_classes(SymbolTable& sym_table, SymSCClassName root_object) {
    const auto abs_obj_sym = sym_table.get_class_name("AbstractObject");
    const auto obj_sym = sym_table.get_class_name("Object");
    const auto class_sym = sym_table.get_class_name("Class");
    const auto string_sym = sym_table.get_class_name("String");
    const auto true_sym = sym_table.get_class_name("True");
    const auto false_sym = sym_table.get_class_name("False");
    const auto nil_sym = sym_table.get_class_name("Nil");
    const auto float_sym = sym_table.get_class_name("Float");
    const auto int_sym = sym_table.get_class_name("Integer");
    const auto char_sym = sym_table.get_class_name("Char");
    const auto sym_sym = sym_table.get_class_name("Symbol");
    const auto array_sym = sym_table.get_class_name("Array");
    const auto event_sym = sym_table.get_class_name("Event");

    return CommonClasses {
        //
        root_object,
        obj_sym, //
        class_sym, //
        string_sym, //
        true_sym, //
        false_sym, //
        nil_sym, //
        float_sym, //
        int_sym, //
        char_sym, //
        sym_sym, //
        array_sym, //
        event_sym //
    };
}

std::vector<Diagnostic> IRCompiler::compile_class_library( //
    const IntrinsicConstraints& intrinsic_constraints, //
    SymSCClassName root_object,
    std::vector<GraphAndIndex> class_lib, //
    size_t num_threads) //
{
    auto common_c = symbol_table_and_common_classes(symbol_table, root_object);

    m_context.emplace(symbol_table, std::move(common_c), std::move(class_lib));


    (void)num_threads; // Later we should use a thread pool.

    if (m_compile_has_been_called) {
        // slowest memory order is fine here. This shouldn't happen
        assert(false);
        std::terminate();
    }
    m_compile_has_been_called = true;

    // Declare classes
    size_t i { 0 };
    for (const auto& p : m_context->ast_of_class_library()) {
        const auto& [ast_graph, root_index] = p;

        const auto classes_or_extensions = ast_graph.children(root_index);

        for (auto child { classes_or_extensions.iter() }; child; ++child) {
            if (const auto class_i = ast_graph.as_a<ast::ClassIndex>(*child)) {
                if (const auto result =
                        ast2ir::DeclareClass { *m_context, ast_graph, ASTGraphIndex::from(i) }(*class_i)) {
                    const auto [c, m] = *result;

                    m_context->class_declarations.register_class_declaration(c);
                    m_context->class_declarations.register_class_declaration(m);
                }
            }
        }

        i += 1;
    }


    // Check intrinsics
    std::atomic<bool> intrinsic_constraints_met { true };
    for (const auto& [k, v] : intrinsic_constraints) {
        // We know this is okay to multithread because the constraints are a map, so we only ever access the decl one at
        // a time and we are no longer inserting any new ones.
        auto* decl = m_context->class_declarations.get_unsafe(k);
        if (!decl) {
            m_context->diagnostics(Diagnostic::missing_intrinsic_class(std::string { m_context->symbol_table(k) }));
            intrinsic_constraints_met = false;
            continue;
        }

        if (auto diags = v(*m_context, *decl, m_context->ast_of_class_library()); !diags.empty()) {
            m_context->diagnostics(std::move(diags));
            intrinsic_constraints_met = false;
        }
    }


    // Step 2: Check hierarchy makes sense.

    const auto cycles = m_context->inheritance_edges.cycle_check(m_context->common_classes.root_object);
    for (const auto& chain : cycles) {
        // TODO: create diagnostic for inheritance chain.
        for (const auto c : chain) {
            m_context->class_declarations.get_unsafe(c)->poisoned = true;
        }
    }

    // Step 3: Compile all ast class nodes.
    // Step 4: Compile all ast class extension nodes.

    return m_context->diagnostics.consume();
}

void IRCompiler::insert_class_ir(SymSCClassName name, IRGraph&& graph, SCClass_I class_index) {
    std::scoped_lock lock { m_ir_graphs_lock };
    m_classes.insert({ name,
                       IRCompiler::ClassData {
                           name,
                           {},
                           {},
                           std::move(graph),
                           class_index,
                       } });
}

void IRCompiler::poison_class(SymSCClassName name) {
    {
        std::scoped_lock lock { m_ir_graphs_lock };
        m_classes.find(name)->second.poisoned = true;
    }
    m_context->class_declarations.with_decl(name, [](ClassDeclaration& decl) {
        decl.poisoned = true;
        return;
    });
}


std::variant<ConstExpression, Diagnostic> compile_consteval(ast::ASTGraph ast_graph, ast::AnyExprIndex expr_i,
                                                            SymbolTable& sym_table) {
    auto common_c = symbol_table_and_common_classes(sym_table, sym_table.get_class_name("AbstractObject"));

    Context context { sym_table, std::move(common_c), {} };

    using R = std::variant<ConstExpression, Diagnostic>;
    auto r =
        std::visit(ast2ir::ConstEval { context, ast_graph, ASTGraphIndex { 0 } }, ast_graph.index_to_variant(expr_i));

    return std::move(r).visit([&](ast2ir::ConstEval::Success s) -> R { return { context.constant_table(s.first) }; },
                              [&](Diagnostic d) -> R { return { std::move(d) }; });
}

[[nodiscard]] std::vector<Diagnostic>
IntrinsicConstraint::operator()(Context& cxt, ClassDeclaration& decl,
                                const std::vector<GraphAndIndex>& ast_graphs) const {
    assert(decl.name.value == target);

    std::vector<Diagnostic> out;

    // Check super class
    [&]() {
        // both abscent
        if (!super_class && !decl.super)
            return;

        if (super_class && decl.super) {
            // both present and equal
            if (*super_class == *decl.super->value)
                return;

            // present, not equal
            const auto [ast_graph_i, ast_node_i] = decl.super->location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));
            decl.poisoned = true;
            return;
        }
        if (super_class && !decl.super) {
            const auto [ast_graph_i, ast_node_i] = decl.super->location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));
            decl.poisoned = true;
        } else {
            const auto [ast_graph_i, ast_node_i] = decl.super->location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));
            decl.poisoned = true;
        }
    }();

    // check slot def
    [&]() {
        if (!slot_def && !decl.slot_def)
            return;
        else if (slot_def && !decl.slot_def) {
            // Should have a slot def
            const auto [ast_graph_i, ast_node_i] = decl.name.location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));
            decl.poisoned = true;
            return;
        } else if (!slot_def && decl.slot_def) {
            // Should not have one
            const auto [ast_graph_i, ast_node_i] = decl.slot_def->location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));
            decl.poisoned = true;
            return;
        } else if (*slot_def != decl.slot_def->value) {
            // Not equal
            const auto [ast_graph_i, ast_node_i] = decl.slot_def->location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));
            decl.poisoned = true;
            return;
        }
    }();

    // Check members
    [&]() {
        // deal with empty requirement
        if (member_names.empty() && decl.members.empty())
            return;

        else if (member_names.empty() && !decl.members.empty()) {
            // Shouldn't be any.
            const auto [ast_graph_i, ast_node_i] = decl.members.front().name.location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));
            decl.poisoned = true;
            return;
        }
        // member_names is not empty.

        if (decl.members.empty()) {
            // Didn't provide any, uses class name location
            const auto [ast_graph_i, ast_node_i] = decl.name.location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));

            decl.poisoned = true;
            return;
        }

        // both have something.

        if (decl.members.size() < member_names.size()) {
            // Too few
            const auto [ast_graph_i, ast_node_i] = decl.members.back().name.location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));

            decl.poisoned = true;
            return;
        } else if (decl.members.size() > member_names.size()) {
            // Too many
            const auto [ast_graph_i, ast_node_i] = decl.members[member_names.size()].name.location;
            const auto& graph = ast_graphs[*ast_graph_i].first;
            out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));

            decl.poisoned = true;
            return;
        }


        assert(member_names.size() == decl.members.size()); // checked above.

        for (size_t i { 0 };; ++i) {
            if (i >= member_names.size())
                return;

            const auto& got = decl.members[i];
            const auto& expected = member_names[i];

            if (got.poisoned) {
                const auto [ast_graph_i, ast_node_i] = got.name.location;
                const auto& graph = ast_graphs[*ast_graph_i].first;
                out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));
                decl.poisoned = true;
                return;
            }

            if (got.expr_index) {
                //  Intrinsic classes shouldn't have defaults.
                const auto [ast_graph_i, ast_node_i] = got.expr_index->location;
                const auto& graph = ast_graphs[*ast_graph_i].first;
                out.push_back(Diagnostic::todo(graph.text_info(), graph.location(ast_node_i)));
                decl.poisoned = true;
                return;
            }
            if (got.name.value != expected) {
                //  Incorrect name
                const auto [ast_graph_i, ast_node_i] = got.name.location;
                const auto& graph = ast_graphs[*ast_graph_i].first;
                const auto expected_name = cxt.symbol_table(expected);
                out.push_back(Diagnostic::intrinsic_class_wrong_member_name(graph.text_info(),
                                                                            graph.location(ast_node_i), expected_name));
                decl.poisoned = true;
                return;
            }
        }
    }();


    return out;
}
void InheritanceEdges::insert(SymSCClassName parent, SymSCClassName child) {
    std::scoped_lock l { m_lock };
    m_sub_classes[parent].push_back(child);
}
std::vector<std::vector<SymSCClassName>> InheritanceEdges::cycle_check(SymSCClassName root) const {
    std::scoped_lock l { m_lock };
    // Because root is empty, and we have single inheritance, it is impossible to create cycles that are attached to
    // root.
    std::unordered_set<SymSCClassName, std::hash<Symbol>> not_visited, visited;

    const auto dfs = [&](auto&& self, SymSCClassName n) -> void {
        assert(visited.count(n) == 0);
        not_visited.erase(n);
        visited.insert(n);

        for (const SymSCClassName child : m_sub_classes.at(n))
            self(self, child);
    };

    for (const auto& [k, v] : m_sub_classes) {
        not_visited.insert(k);
    }

    dfs(dfs, root);

    std::vector<std::vector<SymSCClassName>> loops;

    const auto loop_maker = [&](auto&& self, SymSCClassName n, std::vector<SymSCClassName>& current_cycle) -> void {
        assert(not_visited.count(n));
        current_cycle.push_back(n);
        not_visited.erase(n);

        for (const SymSCClassName child : m_sub_classes.at(n))
            self(self, child, current_cycle);
    };

    // Effectively pulls a random class from the not_visited set and returns the cycle it is connected to.
    // Repeats until there are no more.
    while (!not_visited.empty()) {
        std::vector<SymSCClassName> cycles;
        loop_maker(loop_maker, *not_visited.begin(), cycles);
        loops.push_back(std::move(cycles));
    }

    return loops;
}
}
