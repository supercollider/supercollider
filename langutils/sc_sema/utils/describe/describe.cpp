#include "describe.hpp"

#include "sc_util/overload.hpp"

#include <string_view>

namespace sc::ir::describe {

namespace {
void write_text(std::ostream& out, std::string_view view) {
    for (const char c : view) {
        if (c == '\n')
            out << "'\\n'";
        else
            out << c;
    }
}

const char* kind_name(SCAssign::Kind k) {
    switch (k) {
    case SCAssign::Kind::Local:
        return "local";
    case SCAssign::Kind::Instance:
        return "instance";
    case SCAssign::Kind::Class:
        return "class";
    case SCAssign::Kind::Environment:
        return "environment";
    }
    return "";
}

const char* read_kind_name(SCRead::Kind k) { return kind_name(static_cast<SCAssign::Kind>(static_cast<int>(k))); }

const char* layout_name(SCClass::MemoryLayout l) {
    using Layout = SCClass::MemoryLayout;
    switch (l) {
    case Layout::NotIndexed:
        return "";
    case Layout::Slot:
        return "[slot]";
    case Layout::Double:
        return "[double]";
    case Layout::Float:
        return "[float]";
    case Layout::Int32:
        return "[int32]";
    case Layout::Int16:
        return "[int16]";
    case Layout::Int8:
        return "[int8]";
    case Layout::Char:
        return "[char]";
    case Layout::Symbol:
        return "[symbol]";
    }
    return "";
}

const char* special_name(SCSpecial::Kind k) {
    using Kind = SCSpecial::Kind;
    switch (k) {
    case Kind::This:
        return "this";
    case Kind::Super:
        return "super";
    case Kind::Process:
        return "thisProcess";
    case Kind::Thread:
        return "thisThread";
    case Kind::Method:
        return "thisMethod";
    case Kind::FunctionDef:
        return "thisFunctionDef";
    case Kind::Function:
        return "thisFunction";
    }
    return "";
}
}

void write_location(std::ostream& out, const SourceLocation& l) {
    const auto r = IRGraph::range(l);
    out << "(" << r.begin.line_number + 1 << ":" << r.begin.column + 1 << ")";
}

bool hidden(const Describe& d, const IRGraph::Edges& edges, const IRGraph::Variant& payload,
            std::optional<SymSCClassName> dumped_class) {
    // Inherited members are dumped with the class that declares them.
    if (const auto member = std::get_if<SCMember>(&payload); member && member->owning_class != dumped_class)
        return true;
    // Empty argument lists add nothing.
    const bool is_argument_list = std::holds_alternative<SCArgumentDeclareList>(payload)
        || std::holds_alternative<SCPositionalArgs>(payload) || std::holds_alternative<SCKwArgs>(payload)
        || std::holds_alternative<SCVariadicArgs>(payload);
    if (is_argument_list && !edges.first_child)
        return true;
    // A missing part only matters in a range, where it says which part is missing, e.g., `(1..)`.
    if (std::holds_alternative<SCMissing>(payload)) {
        const NodeIndex parent { *edges.parent };
        if (!d.graph.as_a<SCRange_I>(parent) && !d.graph.as_a<SCRangeGenerator_I>(parent))
            return true;
    }
    return false;
}

bool repeats_class(const Describe& d, SCClass_I c) {
    const auto& g = d.graph;
    const auto name = g.payload(c).name;
    if (!d.ctx.symbol_table.is_meta(name))
        return false;
    const auto [members, methods] = g.children(c);
    bool has_own_member = false;
    for (auto it = g.children(members).iter(); it; ++it)
        has_own_member = has_own_member || g.payload(*it).owning_class == name;
    return !has_own_member && !g.children(methods).iter();
}

void write_details(const Describe& d, std::ostream& out, const IRGraph::Variant& payload, const SourceLocation& loc,
                   NodeIndex i) {
    const auto& g = d.graph;
    const auto& symbol_table = d.ctx.symbol_table;
    const auto write_constant = [&](ConstExprIndex c) {
        d.ctx.constant_table(c).to_literal(out, symbol_table, d.ctx.constant_table);
    };
    // Where the variable is declared, shown by its source, so a read can be matched to its declaration.
    const auto write_declaration = [&](const std::optional<SourceLocation>& declaration) {
        if (!declaration)
            return;
        out << ", declared ";
        write_location(out, *declaration);
        out << ": ";
        write_text(out, g.read(*declaration));
    };

    // Only when it isn't Legacy, which almost all code is.
    const auto write_version = [&](sc::ast::LanguageVersion v, const char* before, const char* after) {
        if (v != sc::ast::LanguageVersion::Legacy)
            out << before << "version " << sc::ast::to_string(v) << after;
    };

    std::visit(sc::util::overload {
                   [](const auto&) {},
                   // The slot type, e.g., `String[char]`.
                   [&](const SCClass& c) {
                       out << ": " << symbol_table(c.name) << layout_name(c.memory_layout);
                       write_version(c.version, ", ", "");
                   },
                   [&](const SCMethod& m) {
                       out << ": ";
                       if (m.poisoned)
                           out << "poisoned: ";
                       write_version(m.version, "", ": ");
                       write_text(out, g.read(loc));
                   },
                   // Its file, as interpreted code isn't in a class.
                   [&](const SCInterpreted& in) {
                       out << ": ";
                       if (in.poisoned)
                           out << "poisoned: ";
                       write_version(in.version, "", ": ");
                       if (const auto& path = g.text_info(loc.source)->file_path)
                           out << (d.relative_to ? path->lexically_relative(*d.relative_to) : *path).string();
                   },
                   [&](const SCMessage& m) { out << ": " << symbol_table(*m.selector); },
                   [&](const SCKwArg& k) { out << ": " << symbol_table(*k.name); },
                   [&](const SCVariableDeclare& v) { out << ": " << symbol_table(*v.name); },
                   [&](const SCArgumentDeclare& a) { out << ": " << symbol_table(a.name); },
                   [&](const SCMember& m) { out << ": " << symbol_table(m.name); },
                   [&](const SCSpecial& special) { out << ": " << special_name(special.kind); },
                   [&](const SCRead& r) { out << ": " << read_kind_name(r.kind) << " " << symbol_table(r.name); },
                   [&](const SCAssign& a) { out << ": " << kind_name(a.kind) << " " << symbol_table(*a.name); },
                   [&](const SCPackTarget& t) {
                       out << ": " << kind_name(t.kind) << " " << symbol_table(*t.name)
                           << (t.rest ? " rest from " : " at ") << t.index;
                   },
                   [&](const SCCollection& c) { out << ": " << symbol_table(*c.collection_class); },
                   [&](const SCGenerator& gen) {
                       out << (gen.kind == SCGenerator::Kind::Routine ? ": routine" : ": immediate");
                   },
                   [&](const SCGenQualIterate& q) {
                       out << ": " << symbol_table(*q.element);
                       if (q.index)
                           out << ", " << symbol_table(**q.index);
                   },
                   [&](const SCGenQualDeclare& q) { out << ": " << symbol_table(*q.name); } },
               payload);

    if (const auto const_i = g.node_as_constexpr(i)) {
        out << " = ";
        write_constant(*const_i);
    }
    // A member's initial value, the member itself isn't constant.
    if (const auto member = std::get_if<SCMember>(&payload); member && member->expr) {
        out << " = ";
        write_constant(*member->expr);
    }
    // What calling a method or function returns.
    const auto constant_result =
        std::visit(sc::util::overload { [](const auto&) -> OptionalConstExprIndex { return {}; },
                                        [](const SCMethod& m) { return m.constant_result; },
                                        [](const SCFunction& f) { return f.constant_result; },
                                        [](const SCRegionList& l) { return l.constant_result; } },
                   payload);
    if (constant_result) {
        out << " -> ";
        write_constant(*constant_result);
    }
    std::visit(sc::util::overload { [](const auto&) {}, [&](const SCRead& r) { write_declaration(r.declaration); },
                                    [&](const SCAssign& a) { write_declaration(a.declaration); },
                                    [&](const SCPackTarget& t) { write_declaration(t.declaration); } },
               payload);
}

}
