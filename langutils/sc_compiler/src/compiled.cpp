#include "sc_compiler/sc_compiler.hpp"

#include <algorithm>
#include <cassert>
#include <variant>

namespace sc::compiler {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

ir::OptionalSymSCSymbol function_name(ClassLibrary& library, const ir::IRGraph& g, backend::FrameOwner_I owner) {
    if (const auto method = g.as_a<ir::SCMethod_I>(owner))
        return ir::SymSCSymbol::from(g.payload(*method).selector.value);
    if (const auto function = g.as_a<ir::SCFunction_I>(owner))
        return g.payload(*function).name;
    return {};
}

// A computed value that is a literal, e.g., `a = (false)` or `b(#[1])`, but not a class name, which is read.
ir::OptionalConstExprIndex literal_value(const ClassLibrary& library, const ir::IRGraph& g, ir::NodeIndex value) {
    const auto collection = g.as_a<ir::SCCollection_I>(value);
    if (!g.is_a<ir::SCLiteral_I>(value) && !(collection && g.payload(*collection).literal))
        return {};
    const auto constant = g.node_as_constexpr(value);
    if (!constant || library.ctx.constant_table(*constant).get_if<ir::SymSCClassName>())
        return {};
    return *constant;
}

// A method's or function's own arguments and variables, not those hoisted into its frame from an inlined function,
// which are set each time it runs. A bracketed literal is computed when nil is passed, but the frame holds it too, as
// the old compiler had it, e.g., for SynthDef's controls.
template <typename Function>
void set_prototypes(const ClassLibrary& library, const ir::IRGraph& g, const backend::Frames& frames,
                    backend::FrameIndex frame, Function f,
                    util::IndexedVector<backend::FrameSlotIndex, CompiledSlot>& slots) {
    const auto set = [&](auto declare) {
        const auto& d = g.payload(declare);
        const auto& location = frames.locations[d.local];
        assert(location.frame && *location.frame == *frame);
        (void)frame;
        const auto [value] = g.children(declare);
        slots[location.slot].prototype =
            d.prototype ? d.prototype : literal_value(library, g, ir::NodeIndex { *value });
    };
    const auto [positional, variadic, body] = g.children(f);
    for (const auto list : { positional, variadic })
        for (auto it = g.children(list).iter(); it; ++it)
            set(*it);
    for (auto it = g.children(body).iter(); it; ++it)
        if (const auto declare = g.as_a<ir::SCVariableDeclare_I>(*it))
            set(*declare);
}

util::IndexedVector<backend::FrameSlotIndex, CompiledSlot> compiled_slots(const ClassLibrary& library,
                                                                          const ir::IRGraph& g,
                                                                          const backend::Frames& frames,
                                                                          backend::FrameIndex index) {
    const auto& frame = frames.frames[index];
    util::IndexedVector<backend::FrameSlotIndex, CompiledSlot> slots;
    for (const auto local : frame.slots.raw())
        (void)slots.push_back(
            { local ? g.local(ir::LocalIndex { *local }).name : library.ctx.common_symbols.this_, {} });
    if (const auto method = g.as_a<ir::SCMethod_I>(frame.owner))
        set_prototypes(library, g, frames, index, *method, slots);
    else if (const auto function = g.as_a<ir::SCFunction_I>(frame.owner))
        set_prototypes(library, g, frames, index, *function, slots);
    return slots;
}

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

CompiledClass compiled_class(const ClassLibrary& library, const ir::IRGraph& g, ir::SCClass_I class_i) {
    const auto& c = g.payload(class_i);
    CompiledClass out { c.name, {}, c.memory_layout, g.location(class_i), {}, {}, {}, {} };
    if (c.superclass)
        out.superclass = *c.superclass;
    const bool is_meta = library.symbols.is_meta(c.name);
    if (is_meta)
        out.first_class_variable = library.backend->members.class_variable_base(c.name);
    const auto [members, _] = g.children(class_i);
    for (auto it = g.children(members).iter(); it; ++it) {
        const auto& m = g.payload(*it);
        const CompiledMember member { m.name, m.expr };
        if (!is_meta)
            out.instance_variables.push_back(member);
        else if (m.owning_class != c.name)
            continue;
        else if (m.is_const)
            out.constants.push_back(member);
        else
            out.class_variables.push_back(member);
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Dump
////////////////////////////////////////////////////////////////////////////////

// As sc_inspect's dump writes them.
void write_location(std::ostream& out, const ir::SourceLocation& l) {
    out << "(" << l.range.begin.line_number + 1 << ":" << l.range.begin.column + 1 << ")";
}

void write_value(std::ostream& out, ir::OptionalConstExprIndex value, const ClassLibrary& library) {
    if (!value)
        return;
    out << " = ";
    const auto& constants = library.ctx.constant_table;
    constants(ir::ConstExprIndex { *value }).to_literal(out, library.symbols, constants);
}

void write_members(std::ostream& out, const char* title, const std::vector<CompiledMember>& members,
                   const ClassLibrary& library) {
    if (members.empty())
        return;
    out << "|  " << title << ":";
    for (const auto& m : members) {
        out << " " << library.symbols(m.name);
        write_value(out, m.value, library);
        out << ",";
    }
    out << "\n";
}

constexpr const char* memory_layout_name(ir::SCClass::MemoryLayout layout) {
    using Layout = ir::SCClass::MemoryLayout;
    switch (layout) {
    case Layout::NotIndexed:
        return "not indexed";
    case Layout::Slot:
        return "slot";
    case Layout::Double:
        return "double";
    case Layout::Float:
        return "float";
    case Layout::Int32:
        return "int32";
    case Layout::Int16:
        return "int16";
    case Layout::Int8:
        return "int8";
    case Layout::Char:
        return "char";
    case Layout::Symbol:
        return "symbol";
    }
    return "";
}

void write_slots(std::ostream& out, const char* title, const CompiledFunction& f, backend::FrameSlotIndex first,
                 backend::FrameSlotIndex last, const ClassLibrary& library) {
    if (first == last)
        return;
    out << "|  |  " << title << ":";
    for (const auto s : util::range(first, last)) {
        out << " " << library.symbols(f.slots[s].name);
        write_value(out, f.slots[s].prototype, library);
        out << ",";
    }
    out << "\n";
}

void write_function(std::ostream& out, backend::lir::FunctionIndex index, const CompiledFunction& f, bool emitted_root,
                    const ClassLibrary& library) {
    out << "|  function " << *index << " ";
    write_location(out, f.location);
    if (f.name)
        out << ": " << library.symbols(ir::SymSCSymbol { *f.name });
    if (f.parent)
        out << ", in function " << *f.parent;
    out << "\n";

    const backend::FrameSlotIndex first { 0 };
    const auto positional = util::advance(first, (f.has_this ? 1u : 0u) + f.num_positional);
    const auto arguments = util::advance(positional, static_cast<std::uint32_t>(f.variadic));
    write_slots(out, "arguments", f, first, positional, library);
    write_slots(out, "variadic", f, positional, arguments, library);
    write_slots(out, "variables", f, arguments, f.slots.next_index(), library);

    const auto& emitted = f.emitted;
    out << "|  |  ";
    if (!emitted_root)
        out << "not emitted";
    else if (emitted.thrown_away)
        out << "thrown away";
    else if (!std::holds_alternative<std::monostate>(emitted.shortcut))
        backend::write_shortcut(out, emitted.shortcut, library.symbols, library.ctx.constant_table);
    else
        out << "code: " << emitted.code.codes.size() << " bytes, " << emitted.selectors.size() << " selectors, "
            << emitted.constants.size() << " constants";
    out << "\n";
}
}

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

CompiledGraph finish(ClassLibrary& library, EmittedGraph&& emitted) {
    const auto& g = *emitted.graph;
    const auto& frames = emitted.frames;
    const auto& lowered = emitted.lowered;
    CompiledGraph out;
    out.sources = g.sources();
    const auto class_i = g.as_a<ir::SCClass_I>(emitted.root);
    if (class_i)
        out.layout = compiled_class(library, g, *class_i);

    util::IndexedVector<backend::FrameIndex, backend::lir::OptionalFunctionIndex> function_of_frame;
    function_of_frame.resize(frames.frames.size());
    for (const auto index : lowered.functions.indices())
        function_of_frame[lowered.functions[index].frame] = index;

    out.functions.raw().reserve(lowered.functions.size());
    for (const auto index : lowered.functions.indices()) {
        const auto frame_index = lowered.functions[index].frame;
        const auto& frame = frames.frames[frame_index];
        (void)out.functions.push_back({
            g.location(frame.owner),
            function_name(library, g, frame.owner),
            frame.parent ? function_of_frame[backend::FrameIndex { *frame.parent }]
                         : backend::lir::OptionalFunctionIndex {},
            frame.has_this,
            frame.num_positional,
            frame.variadic,
            compiled_slots(library, g, frames, frame_index),
            std::move(emitted.emitted.functions[index]),
        });
    }

    for (const auto root : emitted.emitted.roots) {
        const auto owner = frames.frames[lowered.functions[root].frame].owner;
        if (const auto method = g.as_a<ir::SCMethod_I>(owner)) {
            const auto& m = g.payload(*method);
            out.methods.push_back({ m.selector.value, root, {} });
            if (m.primitive)
                out.methods.back().primitive = m.primitive->value;
        } else {
            out.interpreted.push_back(root);
        }
    }

    for (const auto& accessor : emitted.emitted.accessors)
        out.accessors.push_back({ accessor.selector, g.location(accessor.member), accessor.shortcut });

    out.diagnostics = std::move(emitted.frames.diagnostics);
    for (auto& d : emitted.emitted.diagnostics)
        out.diagnostics.push_back(std::move(d));
    return out;
}

void write_compiled(std::ostream& out, const CompiledGraph& compiled, const ClassLibrary& library) {
    const auto& symbols = library.symbols;
    if (const auto& layout = compiled.layout) {
        out << symbols(layout->name) << "\n|  class ";
        write_location(out, layout->location);
        if (layout->superclass)
            out << ": superclass " << symbols(ir::SymSCClassName { *layout->superclass }) << ",";
        out << " " << memory_layout_name(layout->memory_layout) << "\n";
        write_members(out, "instance variables", layout->instance_variables, library);
        if (layout->first_class_variable && !layout->class_variables.empty())
            out << "|  class variables from " << **layout->first_class_variable << "\n";
        write_members(out, "class variables", layout->class_variables, library);
        write_members(out, "constants", layout->constants, library);
    } else {
        out << "Interpreted\n";
    }
    for (const auto& accessor : compiled.accessors) {
        out << "|  accessor " << symbols(accessor.selector) << " ";
        write_location(out, accessor.location);
        out << ": ";
        backend::write_shortcut(out, accessor.shortcut, symbols, library.ctx.constant_table);
        out << "\n";
    }
    for (const auto& method : compiled.methods) {
        out << "|  method " << symbols(method.selector) << ": function " << *method.function;
        if (method.primitive)
            out << ", primitive " << symbols(ir::SymSCPrimitive { *method.primitive });
        out << "\n";
    }
    if (!compiled.interpreted.empty()) {
        out << "|  interpreted:";
        for (const auto f : compiled.interpreted)
            out << " function " << *f << ",";
        out << "\n";
    }
    // A function's root is emitted when a method or the interpreted code is.
    const auto& methods = compiled.methods;
    const auto& interpreted = compiled.interpreted;
    for (const auto index : compiled.functions.indices()) {
        auto root = index;
        while (const auto parent = compiled.functions[root].parent)
            root = backend::lir::FunctionIndex { *parent };
        const bool emitted_root =
            std::any_of(methods.begin(), methods.end(), [&](const CompiledMethod& m) { return m.function == root; })
            || std::find(interpreted.begin(), interpreted.end(), root) != interpreted.end();
        write_function(out, index, compiled.functions[index], emitted_root, library);
    }
}
}
