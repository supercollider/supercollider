
#pragma once
#include "sc_sema/context.hpp"
#include <memory>

namespace sc::ast {
struct ASTGraph;
}

namespace sc::ir {
struct IRGraph;
}
namespace sc::ir::ast2ir {

struct DeclareClass {
    DeclareClass(Context& cx, std::shared_ptr<const ast::ASTGraph> ast, ASTGraphIndex ast_i, ModuleIndex mod):
        cxt(cx),
        ast_graph(ast),
        ast_index(ast_i),
        module(mod) {}
    struct Result {
        ClassDeclaration class_;
        ClassDeclaration meta_class_;
    };

    std::optional<Result> operator()(ast::ClassIndex);

private:
    void append_member_to_decl(ClassDeclaration&, ast::Index, ast::UntypedIndex accessor_i, ast::ReadWriteAccessor,
                               ast::NamedIdentifierIndex);
    void append_member_to_decl(ClassDeclaration&, ast::Index, ast::UntypedIndex accessor_i, ast::ReadWriteAccessor,
                               ast::DeclareVariableWithDefaultIndex);
    void append_member_to_decl(ClassDeclaration&, ast::Index, ast::UntypedIndex accessor_i, ast::ReadWriteAccessor,
                               ast::VariablePackAssignIndex);

    Context& cxt;
    std::shared_ptr<const ast::ASTGraph> ast_graph;
    ASTGraphIndex ast_index;
    ModuleIndex module;
};

void declare_class(const ast::ASTGraph&, ASTGraphIndex, Context&, const CommonClasses&, ast::ClassIndex);
}