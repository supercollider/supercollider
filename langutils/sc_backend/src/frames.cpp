#include "sc_backend/frames.hpp"
#include "sc_sema/ir.hpp"
#include "sc_util/overload.hpp"

#include <algorithm>
#include <cassert>

namespace sc::backend {

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

FrameIndex FrameCursor::start(ir::NodeIndex owner) {
    const FrameIndex index { m_next++ };
    assert(!m_frames || (m_frames->frames.contains(index) && *m_frames->frames[index].owner == *owner));
    (void)owner;
    m_stack.push_back(index);
    return index;
}

std::optional<FrameIndex> FrameCursor::enter(ir::NodeIndex i) {
    const auto& g = *m_graph;
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

namespace {

struct FrameCreator {
    const ir::IRGraph& g;
    Frames out {};
    FrameCursor cursor { g };
    bool poisoned { false };
    bool any_poisoned { false };

    // Creates the frame the cursor has started, whose parent is the frame it was started in.
    FrameIndex create(FrameIndex started, OptionalFrameIndex parent, FrameOwner_I owner, bool has_this) {
        Frame f { owner };
        f.parent = parent;
        if (parent)
            f.depth = out.frames[FrameIndex { *parent }].depth + 1;
        f.has_this = has_this;
        const auto index = out.frames.push_back(std::move(f));
        assert(*index == *started);
        (void)started;
        return index;
    }

    void add_arguments(Frame& f, ir::SCArgumentDeclareList_I list, std::uint32_t& count) {
        for (auto it = g.children(list).iter(); it; ++it) {
            f.locals.push_back(g.payload(*it).local);
            ++count;
        }
    }

    // A body's variables, in the order declared. Only bodies declare variables, a nested function has its own.
    void add_variables(Frame& f, ir::SCBody_I body) {
        for (auto it = g.children(body).iter(); it; ++it) {
            const ir::NodeIndex i { **it };
            if (const auto declare = g.as_a<ir::SCVariableDeclare_I>(i)) {
                f.locals.push_back(g.payload(*declare).local);
            } else if (const auto pack = g.as_a<ir::SCPackDeclare_I>(i)) {
                const auto [_, targets] = g.children(*pack);
                for (auto t = g.children(targets).iter(); t; ++t)
                    if (const auto& target = g.payload(*t); target.declares && target.local)
                        f.locals.push_back(ir::LocalIndex { *target.local });
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
        std::size_t owned { 0 };
        for (const auto& local : g.locals())
            if (*local.owner == *f_i)
                ++owned;
        assert(owned == f.locals.size() || poisoned);
        if (owned != f.locals.size()) {
            for (std::uint32_t l { 0 }; l < g.locals().size(); ++l)
                if (*g.locals()[l].owner == *f_i
                    && std::find(f.locals.begin(), f.locals.end(), ir::LocalIndex { l }) == f.locals.end())
                    f.locals.push_back(ir::LocalIndex { l });
        }
        place(index);
    }

    void qualifier(FrameIndex started, OptionalFrameIndex parent, FrameOwner_I owner,
                   std::initializer_list<ir::OptionalLocalIndex> locals) {
        const auto index = create(started, parent, owner, false);
        Frame& f = out.frames[index];
        for (const auto l : locals)
            if (l) {
                f.locals.push_back(ir::LocalIndex { *l });
                ++f.num_positional;
            }
        place(index);
    }

    void place(FrameIndex index) {
        const Frame& f = out.frames[index];
        const std::uint32_t first { f.has_this ? 1u : 0u };
        for (std::uint32_t s { 0 }; s < f.locals.size(); ++s) {
            const auto l = f.locals[s];
            out.locals.grow_to_hold(l);
            out.locals[l] = LocalSlot { index, first + s };
        }
        if (f.num_slots() > max_frame_slots) {
            const auto loc = g.location(f.owner);
            out.diagnostics.push_back(sc::diag::Diagnostic::too_many_variables(
                g.text_info(loc.source), ir::IRGraph::range(loc), f.num_slots(), max_frame_slots));
        }
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
                           function(*started, parent, *g.as_a<ir::SCMethod_I>(i), true);
                       },
                       [&](const ir::SCFunction&) { function(*started, parent, *g.as_a<ir::SCFunction_I>(i), false); },
                       [&](const ir::SCRoutineGenerator&) {
                           create(*started, parent, *g.as_a<ir::SCRoutineGenerator_I>(i), false);
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
                           qualifier(*started, parent, *g.as_a<ir::SCGenQualIterate_I>(i),
                                     { ir::OptionalLocalIndex { q.element_local }, q.index_local });
                       },
                       [&](const ir::SCGenQualDeclare& q) {
                           qualifier(*started, parent, *g.as_a<ir::SCGenQualDeclare_I>(i),
                                     { ir::OptionalLocalIndex { q.local } });
                       },
                   },
                   payload);
    }
};

}

Frames layout_frames(const ir::IRGraph& g, ir::join<ir::SCClass_I, ir::SCInterpreted_I> root) {
    FrameCreator creator { g };
    creator.out.locals.resize(g.locals().size());

    const auto on_enter = [&](const ir::IRGraph::Edges&, const ir::IRGraph::Variant& payload, const ir::SourceLocation&,
                              ir::NodeIndex i, size_t) { //
        creator.enter(payload, i); //
    };

    const auto on_exit = [&](const ir::IRGraph::Edges&, const ir::IRGraph::Variant& payload, const ir::SourceLocation&,
                             ir::NodeIndex i, size_t) { //
        creator.exit(payload, i); //
    };

    g.depth_first_traverse(ir::NodeIndex { *root }, //
                           on_enter, //
                           ir::IRGraph::Default {}, //
                           ir::IRGraph::Default {}, //
                           on_exit);

    assert(!creator.cursor.current());
    assert(creator.any_poisoned
           || std::all_of(creator.out.locals.raw().begin(), creator.out.locals.raw().end(),
                          [](const LocalSlot& l) { return static_cast<bool>(l.frame); }));
    return std::move(creator.out);
}

}
