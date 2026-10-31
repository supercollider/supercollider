#pragma once

#include "sc_backend/details/inlining.hpp"

#include "sc_sema/details/indexes.hpp"

#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"

#include <cstdint>
#include <limits>
#include <optional>
#include <vector>

namespace sc::diag {
class Diagnostic;
}

namespace sc::backend {

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

struct Frames;

// Works out the frames the VM creates, and which frame and slot each variable is in.
// An inlined function has no frame, its body is in the frame it is inlined into.
[[nodiscard]] Frames layout_frames(const ir::IRGraph& g, const InliningContext& inlining,
                                   ir::join<ir::SCClass_I, ir::SCInterpreted_I> root);

////////////////////////////////////////////////////////////////////////////////
// Frames
////////////////////////////////////////////////////////////////////////////////

using FrameIndex = util::StrongIndex<std::uint32_t, struct FrameIndex__>;
using OptionalFrameIndex =
    util::StrongOptionalIndex<std::uint32_t, struct FrameIndex__, std::numeric_limits<std::uint32_t>::max()>;

using FrameOwner_I = ir::join<ir::SCMethod_I, ir::SCFunction_I, ir::SCGenQualIterate_I, ir::SCGenQualDeclare_I,
                              ir::SCRoutineGenerator_I>;

using LocalUse_I = ir::join<ir::SCRead_I, ir::SCAssign_I, ir::SCPackTarget_I>;

// The most slots a frame can have, as the VM addresses them with a byte.
inline constexpr std::size_t max_frame_slots { 255 };

enum struct Variadic : std::uint8_t { None, Args, ArgsAndKwargs };

// A slot of a frame. Can exceed max_frame_slots, which is reported, so it is wider than lir::SlotIndex.
using FrameSlotIndex = util::StrongIndex<std::uint32_t, struct FrameSlotIndex__>;

// The variables of one function call. Slots are `this`, then arguments, variadic arguments, variables, and
// hoisted vars. All are optional.
struct Frame {
    FrameOwner_I owner;
    OptionalFrameIndex parent {}; // The enclosing frame, empty for a root.
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
static_assert(sizeof(Frame) == (sizeof(void*) == 8 ? 48 : 32)); // wasm is 32-bit

struct LocalSlot {
    OptionalFrameIndex frame {}; // The frame it is in, empty if it has none.
    FrameSlotIndex slot { 0 };
};
static_assert(sizeof(LocalSlot) == 8);

struct Frames {
    util::IndexedVector<FrameIndex, Frame> frames {};
    util::IndexedVector<ir::LocalIndex, LocalSlot> locations {};
    // No initialiser, as that would need Diagnostic to be complete here.
    std::vector<sc::diag::Diagnostic> diagnostics;
    // Sends that could be inlined, but aren't, as hoisting their bodies' variables would take the frame past
    // max_frame_slots. Their bodies are functions with frames of their own. Sorted.
    std::vector<ir::SCMessage_I> not_inlined {};

    // Out of line, so sc_diagnostic is only needed where the diagnostics are used.
    Frames();
    ~Frames();
    Frames(const Frames&);
    Frames& operator=(const Frames&);
    Frames(Frames&&) noexcept;
    Frames& operator=(Frames&&) noexcept;

    [[nodiscard]] std::uint32_t depth(FrameIndex from, FrameIndex to) const;
    [[nodiscard]] bool encloses(FrameIndex outer, FrameIndex inner) const;
    [[nodiscard]] bool is_not_inlined(ir::SCMessage_I m) const;
};

////////////////////////////////////////////////////////////////////////////////
// Frame cursor
////////////////////////////////////////////////////////////////////////////////

// Follows the frames through a depth first walk that enters and exits every node, so no per-node table is
// needed.
//
// We have to walk the frames once when laying them out, and again when inlining. This makes sure we always do it in the
// same order.
//
// A method, function or Routine starts its frame on entering, unless the function was inlined by the send being
// visited. A qualifier starts its frame on exiting, so its own expression is in the frame before it (confusing given
// the syntax).
class FrameCursor {
public:
    FrameCursor(const ir::IRGraph& g, const InliningContext& inlining): m_graph(&g), m_inlining(&inlining) {}

    FrameCursor(const ir::IRGraph& g, const InliningContext& inlining, const Frames& frames):
        m_graph(&g),
        m_inlining(&inlining),
        m_frames(&frames) {}

    std::optional<FrameIndex> enter(ir::NodeIndex i);
    std::optional<FrameIndex> exit(ir::NodeIndex i);
    // Call instead of walking a node's children, which are not visited.
    std::optional<FrameIndex> skip_over_children(ir::NodeIndex i);

    [[nodiscard]] OptionalFrameIndex current() const {
        return m_stack.empty() ? OptionalFrameIndex {} : OptionalFrameIndex { m_stack.back() };
    }

    [[nodiscard]] ast::LanguageVersion version() const { return m_version; }

    [[nodiscard]] std::optional<Inlined> inlining(ir::SCMessage_I m) const;

    // For layout_frames(): the frames being created, so a send whose bodies would take the current frame past
    // max_frame_slots is recorded in Frames::not_inlined rather than inlined.
    void lay_out_into(Frames& frames) { m_creating = &frames; }

private:
    FrameIndex start(ir::NodeIndex owner);
    [[nodiscard]] bool is_inlined(ir::SCFunction_I function) const;

    // Pointers, so the cursor can be moved, but they are const references really.
    const ir::IRGraph* m_graph;
    const InliningContext* m_inlining;
    const Frames* m_frames {};
    Frames* m_creating {};
    std::uint32_t m_next { 0 };
    ast::LanguageVersion m_version { ast::LanguageVersion::Legacy };

    std::vector<FrameIndex> m_stack {};
    // A generator's qualifiers' frames end when it exits.
    std::vector<std::size_t> m_stack_size_at_generator {};
    std::vector<ir::SCFunction_I> m_inlined {};
    std::vector<std::size_t> m_inlined_size_at_message {};
};
}
