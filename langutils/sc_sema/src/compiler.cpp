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

    SymbolTable sym_table{};
    const auto abs_obj_sym = sym_table.get<SymSCClassName>("AbstractObject");
    const auto obj_sym = sym_table.get<SymSCClassName>("Object");
    const auto class_sym = sym_table.get<SymSCClassName>("Class");
    const auto string_sym = sym_table.get<SymSCClassName>("String");
    const auto true_sym = sym_table.get<SymSCClassName>("True");
    const auto false_sym = sym_table.get<SymSCClassName>("False");
    const auto nil_sym = sym_table.get<SymSCClassName>("Nil");
    const auto float_sym = sym_table.get<SymSCClassName>("Float");
    const auto int_sym = sym_table.get<SymSCClassName>("Integer");
    const auto char_sym = sym_table.get<SymSCClassName>("Char");
    const auto sym_sym = sym_table.get<SymSCClassName>("Symbol");
    const auto array_sym = sym_table.get<SymSCClassName>("Array");

    CommonClasses common_classes {//
         abs_obj_sym,//
          obj_sym, //
          class_sym, //
          string_sym, //
          true_sym, //
          false_sym, //
          nil_sym,//
          float_sym,//
          int_sym, //
          char_sym, //
          sym_sym, //
          array_sym //
    };
    m_context.emplace(std::move(sym_table),common_classes, std::move(class_lib));


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


    // Step 1: Declare all classes, checking intrinsic classes are correct

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

    // Step 2: Check hierarchy makes sense.

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
}
