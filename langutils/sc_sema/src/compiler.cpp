#include "sc_parser/indexes_typed.hpp"

#include "sc_sema/compiler.hpp"
#include "sc_sema/ast_location.hpp"
#include "sc_sema/symbols.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/tables.hpp"

#include "ast2ir.hpp"

#include <exception>
#include <unordered_map>

namespace sc::ir {

// clang-format off
std::vector<Diagnostic> IRCompiler::compile_class_library( //
        std::unordered_map<SymSCClassName, IntrinsicConstraints, std::hash<Symbol>> intrinsic_constraints, //
        std::vector<GraphAndIndex> class_lib, //
        size_t num_threads 
){
    m_context.emplace(std::move(class_lib));

    ////////////////////////////////////////////////////////////////////////////////
    // This mutates the context member.
    ////////////////////////////////////////////////////////////////////////////////

    (void)intrinsic_constraints;


    // clang-format on
    (void)num_threads; // Later we should use a thread pool.

    if (m_compile_has_been_called) {
        // slowest memory order is fine here. This shouldn't happen
        assert(false);
        std::terminate();
    }
    m_compile_has_been_called = true;

    const auto abs_obj_sym = m_context->symbol_table.get<SymSCClassName>("AbstractObject");
    const auto obj_sym = m_context->symbol_table.get<SymSCClassName>("Object");
    const auto class_sym = m_context->symbol_table.get<SymSCClassName>("Class");
    const CommonClasses common_classes { abs_obj_sym, obj_sym, class_sym };

    // Step 1: Declare all classes, checking intrinsic classes are correct

    size_t i { 0 };
    for (const auto& p : m_context->ast_of_class_library()) {
        const auto& [ast_graph, root_index] = p;

        const auto classes_or_extensions = ast_graph.children(root_index);

        for (auto child { classes_or_extensions.iter() }; child; ++child) {
            if (const auto class_i = ast_graph.as_a<ast::ClassIndex>(*child)) {
                ast2ir::declare_class(ast_graph, ASTGraphIndex::from(i), *m_context, common_classes, *class_i);
            }
        }

        i += 1;
    }

    // Step 2: Check hierarchy makes sense.

    // Step 3: Compile all ast class nodes.
    // Step 4: Compile all ast class extension nodes.

    return m_context->diagnostics.consume();
}

void IRCompiler::insert_class_ir(SymSCClassName name, IRGraph&& graph, SCClass_I class_index) {
    std::scoped_lock lock { m_ir_graphs_lock };
    m_classes.insert({ name, ClassData { name, std::move(graph), class_index } });
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
}
