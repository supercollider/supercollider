#include "sc_backend/frames.hpp"
#include "sc_sema/ir.hpp"
#include "sc_util/overload.hpp"

#include <algorithm>
#include <cassert>

namespace sc::backend {

////////////////////////////////////////////////////////////////////////////////
// Frames
////////////////////////////////////////////////////////////////////////////////

std::uint32_t Frames::depth(FrameIndex from, FrameIndex to) const {
    assert(encloses(to, from) && "the local's frame doesn't enclose the use");
    return frames[from].depth - frames[to].depth;
}

bool Frames::encloses(FrameIndex outer, FrameIndex inner) const {
    OptionalFrameIndex f { inner };
    while (f && *f != *outer)
        f = frames[FrameIndex { *f }].parent;
    return static_cast<bool>(f);
}

////////////////////////////////////////////////////////////////////////////////
// Frame cursor
////////////////////////////////////////////////////////////////////////////////

FrameIndex FrameCursor::start(ir::NodeIndex owner) {
    const FrameIndex index { m_next++ };
    assert(!m_frames || (m_frames->frames.contains(index) && *m_frames->frames[index].owner == *owner));
    (void)owner;
    m_stack.push_back(index);
    return index;
}

bool FrameCursor::is_inlined(ir::SCFunction_I function) const {
    return std::find(m_inlined.begin(), m_inlined.end(), function) != m_inlined.end();
}

std::optional<FrameIndex> FrameCursor::enter(ir::NodeIndex i) {
    const auto& g = *m_graph;
    if (const auto m = g.as_a<ir::SCMessage_I>(i)) {
        m_inlined_size_at_message.push_back(m_inlined.size());
        if (const auto kind = inlining(*m_inlining, g, *m, m_version); kind && *kind != Inlined::QuestionMark) {
            const auto [receiver, p, k, v, a] = g.children(*m);
            if (inlines_receiver(*kind))
                m_inlined.push_back(g.assume_a<ir::SCFunction_I>(receiver));
            // Every function argument, a switch's `{ 1 }` keys too, though only their value is used.
            const auto arguments = positional_arguments(g, *m);
            for (const auto argument : arguments.raw())
                if (const auto f = g.as_a<ir::SCFunction_I>(argument))
                    m_inlined.push_back(*f);
        }
        return std::nullopt;
    }
    if (const auto f = g.as_a<ir::SCFunction_I>(i); f && is_inlined(*f))
        return std::nullopt;

    if (const auto in = g.as_a<ir::SCInterpreted_I>(i))
        m_version = g.payload(*in).version;
    else if (const auto method = g.as_a<ir::SCMethod_I>(i))
        m_version = g.payload(*method).version;

    if (g.is_a<ir::SCMethod_I>(i) || g.is_a<ir::SCFunction_I>(i))
        return start(i);

    if (g.is_a<ir::SCRoutineGenerator_I>(i)) {
        m_stack_size_at_generator.push_back(m_stack.size());
        return start(i);
    }

    if (g.is_a<ir::SCImmediateGenerator_I>(i))
        m_stack_size_at_generator.push_back(m_stack.size());
    return std::nullopt;
}

std::optional<FrameIndex> FrameCursor::exit(ir::NodeIndex i) {
    const auto& g = *m_graph;
    if (g.is_a<ir::SCMessage_I>(i)) {
        m_inlined.erase(m_inlined.begin() + static_cast<std::ptrdiff_t>(m_inlined_size_at_message.back()),
                        m_inlined.end());
        m_inlined_size_at_message.pop_back();
        return std::nullopt;
    }
    if (const auto f = g.as_a<ir::SCFunction_I>(i); f && is_inlined(*f))
        return std::nullopt;
    if (g.is_a<ir::SCMethod_I>(i) || g.is_a<ir::SCFunction_I>(i)) {
        m_stack.pop_back();
        return std::nullopt;
    }
    if (g.is_a<ir::SCGenQualIterate_I>(i) || g.is_a<ir::SCGenQualDeclare_I>(i))
        return start(i);
    if (g.is_a<ir::SCRoutineGenerator_I>(i) || g.is_a<ir::SCImmediateGenerator_I>(i)) {
        m_stack.erase(m_stack.begin() + static_cast<std::ptrdiff_t>(m_stack_size_at_generator.back()), m_stack.end());
        m_stack_size_at_generator.pop_back();
    }
    return std::nullopt;
}

void FrameCursor::skip_over_children(ir::NodeIndex i) {
    (void)enter(i);
    if (const auto child = m_graph->first_child(i))
        m_graph->depth_first_traverse(
            ir::NodeIndex { *child },
            [&](const ir::IRGraph::Edges&, const ir::IRGraph::Variant&, const ir::SourceLocation&, ir::NodeIndex n,
                size_t) { (void)enter(n); },
            ir::IRGraph::Default {}, ir::IRGraph::Default {},
            [&](const ir::IRGraph::Edges&, const ir::IRGraph::Variant&, const ir::SourceLocation&, ir::NodeIndex n,
                size_t) { (void)exit(n); });
    (void)exit(i);
}

////////////////////////////////////////////////////////////////////////////////
// Layout
////////////////////////////////////////////////////////////////////////////////

namespace {

struct FrameCreator {
    const ir::IRGraph& g;
    FrameCursor cursor;
    Frames out {};
    bool poisoned { false };
    bool any_poisoned { false };

    FrameIndex create(FrameIndex started, OptionalFrameIndex parent, FrameOwner_I owner, bool has_this) {
        Frame f { owner };
        f.parent = parent;
        if (parent)
            f.depth = out.frames[FrameIndex { *parent }].depth + 1;
        f.has_this = has_this;
        if (has_this)
            (void)f.slots.push_back({});
        const auto index = out.frames.push_back(std::move(f));
        assert(*index == *started);
        (void)started;
        return index;
    }

    void add_arguments(Frame& f, ir::SCArgumentDeclareList_I list, std::uint32_t& count) {
        for (auto it = g.children(list).iter(); it; ++it) {
            (void)f.slots.push_back(g.payload(*it).local);
            ++count;
        }
    }

    void add_variables(Frame& f, ir::SCBody_I body) {
        for (auto it = g.children(body).iter(); it; ++it) {
            const ir::NodeIndex i { **it };
            if (const auto declare = g.as_a<ir::SCVariableDeclare_I>(i)) {
                (void)f.slots.push_back(g.payload(*declare).local);
            } else if (const auto pack = g.as_a<ir::SCPackDeclare_I>(i)) {
                const auto [_, targets] = g.children(*pack);
                for (auto t = g.children(targets).iter(); t; ++t)
                    if (const auto& target = g.payload(*t); target.declares && target.local)
                        (void)f.slots.push_back(ir::LocalIndex { *target.local });
            }
        }
    }

    template <typename FIndex> void function(FrameIndex started, OptionalFrameIndex parent, FIndex f_i, bool has_this) {
        const auto index = create(started, parent, f_i, has_this);
        Frame& f = out.frames[index];
        const auto [positional, variadic, body] = g.children(f_i);
        add_arguments(f, positional, f.num_positional);
        std::uint32_t num_variadic { 0 };
        add_arguments(f, variadic, num_variadic);
        // The grammar only allows `...args` or `...args, kwargs`.
        assert(num_variadic <= 2);
        f.variadic = static_cast<Variadic>(num_variadic);
        add_variables(f, body);

        // Every local the function owns has been found, unless its ir is incomplete. Any that weren't still get a slot.
        std::size_t owned { has_this ? 1u : 0u };
        for (const auto& local : g.locals())
            if (*local.owner == *f_i)
                ++owned;
        assert(owned == f.slots.size() || poisoned);
        if (owned != f.slots.size()) {
            const auto& found = f.slots.raw();
            for (std::uint32_t l { 0 }; l < g.locals().size(); ++l)
                if (*g.locals()[l].owner == *f_i
                    && std::find(found.begin(), found.end(), ir::OptionalLocalIndex { l }) == found.end())
                    (void)f.slots.push_back(ir::LocalIndex { l });
        }
        record_locations(index, FrameSlotIndex { 0 });
    }

    void qualifier(FrameIndex started, OptionalFrameIndex parent, FrameOwner_I owner,
                   std::initializer_list<ir::OptionalLocalIndex> locals) {
        const auto index = create(started, parent, owner, false);
        Frame& f = out.frames[index];
        for (const auto l : locals)
            if (l) {
                (void)f.slots.push_back(ir::LocalIndex { *l });
                ++f.num_positional;
            }
        record_locations(index, FrameSlotIndex { 0 });
    }

    // Records each local's frame and slot in Frames::locations, from first_unrecorded on, the ones before it already
    // are. Reports the frame once, when these locals take it over the limit.
    void record_locations(FrameIndex index, FrameSlotIndex first_unrecorded) {
        const Frame& f = out.frames[index];
        for (std::uint32_t s { *first_unrecorded }; s < f.slots.size(); ++s) {
            const FrameSlotIndex slot { s };
            const auto l = f.slots[slot];
            if (!l)
                continue;
            out.locations.grow_to_hold(ir::LocalIndex { *l });
            out.locations[ir::LocalIndex { *l }] = LocalSlot { index, slot };
        }
        if (f.num_slots() > max_frame_slots && *first_unrecorded <= max_frame_slots) {
            const auto loc = g.location(f.owner);
            out.diagnostics.push_back(sc::diag::Diagnostic::too_many_variables(
                g.text_info(loc.source), ir::IRGraph::range(loc), f.num_slots(), max_frame_slots));
        }
    }

    // An inlined function's variables are after the variables of the frame it is inlined into.
    void hoist(FrameIndex into, ir::SCFunction_I function) {
        Frame& f = out.frames[into];
        const FrameSlotIndex first_unrecorded { static_cast<std::uint32_t>(f.slots.size()) };
        const auto [p, v, body] = g.children(function);
        add_variables(f, body);
        if (f.slots.contains(first_unrecorded))
            record_locations(into, first_unrecorded);
    }

    void enter(const ir::IRGraph::Variant& payload, ir::NodeIndex i) {
        const auto parent = cursor.current();
        const auto started = cursor.enter(i);
        std::visit(util::overload {
                       [](const auto&) {},
                       [&](const ir::SCInterpreted& in) {
                           poisoned = in.poisoned;
                           any_poisoned = any_poisoned || poisoned;
                       },
                       [&](const ir::SCMethod& m) {
                           poisoned = m.poisoned;
                           any_poisoned = any_poisoned || poisoned;
                           function(*started, parent, g.assume_a<ir::SCMethod_I>(i), true);
                       },
                       [&](const ir::SCFunction&) {
                           // An inlined function has no frame.
                           if (started)
                               function(*started, parent, g.assume_a<ir::SCFunction_I>(i), false);
                           else
                               hoist(FrameIndex { *parent }, g.assume_a<ir::SCFunction_I>(i));
                       },
                       [&](const ir::SCRoutineGenerator&) {
                           create(*started, parent, g.assume_a<ir::SCRoutineGenerator_I>(i), false);
                       },
                   },
                   payload);
    }

    void exit(const ir::IRGraph::Variant& payload, ir::NodeIndex i) {
        const auto parent = cursor.current();
        const auto started = cursor.exit(i);
        std::visit(util::overload {
                       [](const auto&) {},
                       [&](const ir::SCGenQualIterate& q) {
                           qualifier(*started, parent, g.assume_a<ir::SCGenQualIterate_I>(i),
                                     { ir::OptionalLocalIndex { q.element_local }, q.index_local });
                       },
                       [&](const ir::SCGenQualDeclare& q) {
                           qualifier(*started, parent, g.assume_a<ir::SCGenQualDeclare_I>(i),
                                     { ir::OptionalLocalIndex { q.local } });
                       },
                   },
                   payload);
    }
};

}

Frames layout_frames(const ir::IRGraph& g, const InliningContext& inlining,
                     ir::join<ir::SCClass_I, ir::SCInterpreted_I> root) {
    FrameCreator creator { g, FrameCursor { g, inlining } };
    creator.out.locations.resize(g.locals().size());

    const auto on_enter = [&](const ir::IRGraph::Edges&, const ir::IRGraph::Variant& payload, const ir::SourceLocation&,
                              ir::NodeIndex i, size_t) { //
        creator.enter(payload, i); //
    };

    const auto on_exit = [&](const ir::IRGraph::Edges&, const ir::IRGraph::Variant& payload, const ir::SourceLocation&,
                             ir::NodeIndex i, size_t) { //
        creator.exit(payload, i); //
    };

    g.depth_first_traverse(root, //
                           on_enter, //
                           ir::IRGraph::Default {}, //
                           ir::IRGraph::Default {}, //
                           on_exit);

    assert(!creator.cursor.current());
    assert(creator.any_poisoned
           || std::all_of(creator.out.locations.raw().begin(), creator.out.locations.raw().end(),
                          [](const LocalSlot& l) { return static_cast<bool>(l.frame); }));
    return std::move(creator.out);
}
}
