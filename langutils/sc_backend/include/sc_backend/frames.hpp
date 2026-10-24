#pragma once

#include "sc_backend/inlining.hpp"
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

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

struct Frames;

// An inlined function has no frame, its body is in the frame it is inlined into.
[[nodiscard]] Frames layout_frames(const ir::IRGraph& g, const InliningContext& inlining,
                                   ir::join<ir::SCClass_I, ir::SCInterpreted_I> root);

////////////////////////////////////////////////////////////////////////////////
// Frames
////////////////////////////////////////////////////////////////////////////////

using FrameIndex = util::StrongIndex<std::uint32_t, struct FrameIndex__>;
using OptionalFrameIndex =
    util::StrongOptionalIndex<std::uint32_t, struct FrameIndex__, std::numeric_limits<std::uint32_t>::max()>;

// Generator qualifiers have frames, as the old compiler made them functions.
using FrameOwner_I = ir::join<ir::SCMethod_I, ir::SCFunction_I, ir::SCGenQualIterate_I, ir::SCGenQualDeclare_I,
                              ir::SCRoutineGenerator_I>;

using LocalUse_I = ir::join<ir::SCRead_I, ir::SCAssign_I, ir::SCPackTarget_I>;

inline constexpr std::size_t max_frame_slots { 255 };

enum struct Variadic : std::uint8_t { None, Args, ArgsAndKwargs };

// Can exceed max_frame_slots, which is reported, so it is wider than lir::SlotIndex.
using FrameSlotIndex = util::StrongIndex<std::uint32_t, struct FrameSlotIndex__>;

// Slots are `this`, then arguments, variadic arguments, variables, and hoisted vars. All are optional.
struct Frame {
    FrameOwner_I owner;
    OptionalFrameIndex parent {};
    // The number of enclosing frames, so the depth of one frame from another is a subtraction.
    std::uint32_t depth { 0 };
    std::uint32_t num_positional { 0 };
    bool has_this { false };
    Variadic variadic { Variadic::None };
    // Each slot's variable, none for `this`. The variables of enclosing frames it uses are in theirs.
    util::IndexedVector<FrameSlotIndex, ir::OptionalLocalIndex> slots {};

    [[nodiscard]] std::uint32_t num_variadic() const { return static_cast<std::uint32_t>(variadic); }
    [[nodiscard]] std::size_t num_slots() const { return slots.size(); }
};
static_assert(sizeof(Frame) == 48);

struct LocalSlot {
    OptionalFrameIndex frame {};
    FrameSlotIndex slot { 0 };
};
static_assert(sizeof(LocalSlot) == 8);

struct Frames {
    util::IndexedVector<FrameIndex, Frame> frames {};
    // Where each variable is.
    util::IndexedVector<ir::LocalIndex, LocalSlot> locations {};
    std::vector<sc::diag::Diagnostic> diagnostics {};

    [[nodiscard]] std::uint32_t depth(FrameIndex from, FrameIndex to) const;
    // Whether outer is inner or encloses it.
    [[nodiscard]] bool encloses(FrameIndex outer, FrameIndex inner) const;
};

////////////////////////////////////////////////////////////////////////////////
// Frame cursor
////////////////////////////////////////////////////////////////////////////////

// Follows the frames through a depth first walk that enters and exits every node, so no per-node table is needed. A
// method, function or Routine starts its frame on entering, unless the function was inlined by the send being visited.
// A qualifier starts its frame on exiting, so its own expression is in the frame before it (confusing given the
// syntax).
class FrameCursor {
public:
    // For layout_frames, which creates each frame as it starts.
    FrameCursor(const ir::IRGraph& g, const InliningContext& inlining): m_graph(&g), m_inlining(&inlining) {}
    // Checks each frame starts at its owner, which fails if the walk's order differs from layout_frames'.
    FrameCursor(const ir::IRGraph& g, const InliningContext& inlining, const Frames& frames):
        m_graph(&g),
        m_inlining(&inlining),
        m_frames(&frames) {}

    // The frame the node starts, if it does.
    std::optional<FrameIndex> enter(ir::NodeIndex i);
    std::optional<FrameIndex> exit(ir::NodeIndex i);
    void skip_over_children(ir::NodeIndex i);

    [[nodiscard]] OptionalFrameIndex current() const {
        return m_stack.empty() ? OptionalFrameIndex {} : OptionalFrameIndex { m_stack.back() };
    }

    // Inlining depends on version.
    [[nodiscard]] ast::LanguageVersion version() const { return m_version; }

private:
    FrameIndex start(ir::NodeIndex owner);
    [[nodiscard]] bool is_inlined(ir::SCFunction_I function) const;

    // Pointers, so the cursor can be moved.
    const ir::IRGraph* m_graph;
    const InliningContext* m_inlining;
    const Frames* m_frames {};
    std::uint32_t m_next { 0 };
    ast::LanguageVersion m_version { ast::LanguageVersion::Legacy };

    std::vector<FrameIndex> m_stack {};
    // A generator's qualifiers' frames end when it exits.
    std::vector<std::size_t> m_stack_size_at_generator {};
    std::vector<ir::SCFunction_I> m_inlined {};
    std::vector<std::size_t> m_inlined_size_at_message {};
};
}
