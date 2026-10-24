#pragma once

#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/ir_index.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include <cstdint>
#include <limits>
#include <optional>
#include <vector>

namespace sc::backend {

using FrameIndex = util::StrongIndex<std::uint32_t, struct FrameIndex__>;
using OptionalFrameIndex =
    util::StrongOptionalIndex<std::uint32_t, struct FrameIndex__, std::numeric_limits<std::uint32_t>::max()>;

using FrameOwner_I = ir::join<ir::SCMethod_I, ir::SCFunction_I, ir::SCGenQualIterate_I, ir::SCGenQualDeclare_I,
                              ir::SCRoutineGenerator_I>;

// Things that touch locals.
using LocalUse_I = ir::join<ir::SCRead_I, ir::SCAssign_I, ir::SCPackTarget_I>;

inline constexpr std::size_t max_frame_slots { 255 };

enum struct Variadic : std::uint8_t { None, Args, ArgsAndKwargs };

// Functions and methods store their variables, arguments, and 'this', in a frame.
// We need to assign indices.
// Generator expression also create frames, but only because they create functions.
struct Frame {
    FrameOwner_I owner;
    OptionalFrameIndex parent {};
    // How many frames enclose it, so the depth of one frame from another is a subtraction, see Frames::depth.
    std::uint32_t depth { 0 };
    std::uint32_t num_positional { 0 };
    bool has_this { false };
    Variadic variadic { Variadic::None };

    // Here are the locals we provide storage for and the layout. Everything is optional.
    // Layout:
    //      This
    //      arguments,
    //      variadic argument,
    //      kw variadic argument,
    //      varibles.
    std::vector<ir::LocalIndex> locals {};

    [[nodiscard]] std::uint32_t num_variadic() const { return static_cast<std::uint32_t>(variadic); }
    [[nodiscard]] std::size_t num_slots() const { return locals.size() + (has_this ? 1 : 0); }
};

// A slot in a frame, `this` first in a method's. Layout can go past the VM's limit, which is reported, see
// max_frame_slots, so this is wider than the slot the VM can address, lir::SlotIndex.
using FrameSlotIndex = util::StrongIndex<std::uint32_t, struct FrameSlotIndex__>;

struct LocalSlot {
    OptionalFrameIndex frame {};
    FrameSlotIndex slot { 0 };
};
static_assert(sizeof(LocalSlot) == 8);

// The frames of a graph's methods and functions, before any inlining, so every function is its own frame.
// These are the frames of a single ir graph, in other words, everything that is lexically in scope.
struct Frames {
    util::IndexedVector<FrameIndex, Frame> frames {};

    util::IndexedVector<ir::LocalIndex, LocalSlot> locals {};

    // A frame with more slots than the VM supports.
    std::vector<sc::diag::Diagnostic> diagnostics {};

    [[nodiscard]] std::uint32_t depth(FrameIndex from, FrameIndex to) const;
    // Whether outer is inner or one of the frames enclosing it.
    [[nodiscard]] bool encloses(FrameIndex outer, FrameIndex inner) const;
};


// Does a depth first search on the ir graph, numbering the frames it encounters.
// A method, function or Routine starts its frame when it is entered. A qualifier starts its frame when it is exited, so
// its own expression is in the frame before it, and the qualifiers after it and the generator's value are in its frame.
class FrameCursor {
public:
    // For layout_frames, which creates each frame as it starts.
    explicit FrameCursor(const ir::IRGraph& g): m_graph(&g) {}
    // Follows frames already laid out, checking each starts at its owner, which fails if the walk's order differs.
    FrameCursor(const ir::IRGraph& g, const Frames& frames): m_graph(&g), m_frames(&frames) {}

    // The frame the node starts, if it does.
    std::optional<FrameIndex> enter(ir::NodeIndex i);
    std::optional<FrameIndex> exit(ir::NodeIndex i);

    // Enters and exits the node, skips the children.
    void skip_over_children(ir::NodeIndex i);

    [[nodiscard]] OptionalFrameIndex current() const {
        return m_stack.empty() ? OptionalFrameIndex {} : OptionalFrameIndex { m_stack.back() };
    }

private:
    FrameIndex start(ir::NodeIndex owner);

    // Always valid, a reference without having to deal with the nasty move issues.
    const ir::IRGraph* m_graph;
    const Frames* m_frames {};
    std::uint32_t m_next { 0 };

    // Encountered frame stack.
    std::vector<FrameIndex> m_stack {};

    // The stack's size when each generator being visited was entered, its qualifiers' frames end when it exits.
    std::vector<std::size_t> m_stack_size_at_generator {};
};

// The frames of everything under root, a class's methods or interpreted code.
[[nodiscard]] Frames layout_frames(const ir::IRGraph& g, ir::join<ir::SCClass_I, ir::SCInterpreted_I> root);

}
