// Copyright Jordan Henderson 2026
#include "consteval/consteval.hpp"

#include "sc_parser/ast.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/details/intrinsic_constraint.hpp"
#include "sc_sema/ir.hpp"
#include "sc_source/normalise_source.hpp"
#include "sc_source/text_info.hpp"

#include <cassert>

namespace sc::const_eval {

namespace {

constexpr const char* prelude = R"(//# VERSION 3.16
AbstractObjectExperimental {}
Object : AbstractObjectExperimental {
    classvar dependantsDictionary, currentEnvironment, topEnvironment, uniqueMethods;
}
Class {}
String {}
True {}
False {}
Nil {}
Float {}
Integer {}
Char {}
Symbol {}
Ref {}
Array {}
Event {}
Interpreter {
    var <cmdLine, filePath, lineNumber, column, context;
    var <>a, <>b, <>c, <>d, <>e, <>f, <>g, <>h, <>i, <>j;
    var <>k, <>l, <>m, <>n, <>o, <>p, <>q, <>r, <>s, <>t;
    var <>u, <>v, <>w, <>x, <>y, <>z;
    var <>codeDump, <>preProcessor;

    functionCompileContext {}
}
)";

std::shared_ptr<const ast::ASTGraph> parse(std::string source, std::optional<std::filesystem::path> path,
                                           src::TextKind kind, std::vector<diag::Diagnostic>& diagnostics) {
    auto text_info = std::make_shared<src::TextInfo>(src::NormalisedSource { std::move(source) },
                                                     src::FileCodeLocation {}, std::move(path), kind);
    auto [ast_graph, _] = ast::parse(text_info);
    if (ast_graph)
        for (const auto& d : ast_graph->diagnostics())
            diagnostics.push_back(d);
    return ast_graph;
}

}

const ir::ConstExpression* Evaluated::value() const { return m_value ? &(*this)(*m_value) : nullptr; }

const ir::ConstExpression& Evaluated::operator()(ir::ConstExprIndex i) const { return constants()(i); }

const ir::ConstantTable& Evaluated::constants() const { return ir::constant_table(*m_context); }

std::unique_ptr<const Evaluated> evaluate(std::string source, std::optional<std::filesystem::path> path) {
    std::unique_ptr<Evaluated> e { new Evaluated };
    {
        const auto ast_graph = parse(prelude, "prelude.sc", src::TextKind::ClassFile, e->m_diagnostics);
        const auto root = ast_graph ? ast_graph->root() : std::nullopt;
        assert(root && std::holds_alternative<ast::ClassOrExtensionListIndex>(*root));
        (void)e->m_sources.push_back(
            ir::SourceEntry { ast_graph, std::get<ast::ClassOrExtensionListIndex>(*root), ir::ModuleIndex::from(0) });
    }

    // As in sc_inspect, just what the compiler needs, not the class library's constraints.
    auto& symbols = e->m_symbols;
    ir::IntrinsicConstraints constraints {};
    const auto abstract_object = symbols.get_class_name("AbstractObjectExperimental");
    constraints.insert({ abstract_object, ir::IntrinsicConstraint { abstract_object, {}, {}, {} } });
    {
        const auto object = symbols.get_class_name("Object");
        constraints.insert({ object, ir::IntrinsicConstraint { object, { abstract_object }, {}, {} } });
        const auto meta_obj = symbols.get_meta_class_name(object);
        constraints.insert(
            { meta_obj,
              ir::IntrinsicConstraint { meta_obj, { symbols.get_meta_class_name(abstract_object) }, {}, {} } });
    }

    e->m_context = ir::make_context(symbols, abstract_object, e->m_modules, e->m_sources);
    ir::IRCompiler comp { *e->m_context };
    for (auto& d : comp.compile_class_library(constraints, 1))
        e->m_diagnostics.push_back(std::move(d));

    const auto ast_graph = parse(std::move(source), std::move(path), src::TextKind::ScdFile, e->m_diagnostics);
    if (!ast_graph || !ast_graph->root())
        return e;
    auto interpreted = comp.compile_interpreted(ast_graph, std::nullopt, ast::LanguageVersion::V3_16);
    for (auto& d : interpreted.diagnostics)
        e->m_diagnostics.push_back(std::move(d));
    if (!interpreted.ir)
        return e;

    // A list of regions, or a file as a function.
    const auto& ir = *interpreted.ir;
    const auto [code] = ir.children(std::get<ir::SCInterpreted_I>(*ir.root()));
    if (const auto regions = ir.as_a<ir::SCRegionList_I>(code))
        e->m_value = ir.payload(*regions).constant_result;
    else if (const auto function = ir.as_a<ir::SCFunction_I>(code))
        e->m_value = ir.payload(*function).constant_result;
    return e;
}

}
