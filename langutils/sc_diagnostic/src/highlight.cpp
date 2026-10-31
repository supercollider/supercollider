#include "sc_diagnostic/highlight.hpp"
#include "sc_source/codepoint.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/strong_index.hpp"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <string>

namespace sc::diag {

namespace {

// Lines kept above and below the marked ones, and how near two highlights must be to share a listing.
constexpr std::uint32_t context_before { 2 };
constexpr std::uint32_t context_after { 3 };
constexpr std::uint32_t group_distance { 4 };
// A line that is marked, or next to a marked one, is always printed; runs of at least this many others are elided.
constexpr std::uint32_t min_elided_run { 2 };

constexpr const char* annotation_margin = "      ┆ ";

// Line numbers, zero based and relative to the text.
using LineIndex = sc::util::StrongIndex<std::uint32_t, struct LineTag>;
// Columns of the gutter drawn for multi line annotations.
using SlotIndex = sc::util::StrongIndex<std::uint32_t, struct SlotTag>;
// Position in the highlights the caller gave, and which distinct text a highlight is in.
using InputIndex = sc::util::StrongIndex<std::uint32_t, struct InputTag>;
using TextIndex = sc::util::StrongIndex<std::uint32_t, struct TextTag>;

// Byte offsets into the text or a line stay plain, as they do in SourceCodeLocation.
struct Annotation {
    LineIndex first_line { 0 }, last_line { 0 };
    std::size_t begin_byte { 0 }; // in first_line
    std::size_t end_byte { 0 }; // exclusive, in last_line
    std::string_view description;
    SlotIndex slot { 0 }; // multi line only

    [[nodiscard]] bool multi_line() const { return first_line != last_line; }
};

struct Group {
    const Highlight* primary { nullptr };
    InputIndex primary_input_order { 0 }; // the caller's position of the primary
    std::string_view txt;
    sc::util::IndexedVector<LineIndex, std::size_t> line_starts;
    std::vector<Annotation> annotations;
    LineIndex first_line { 0 }, last_line { 0 }; // covered by the annotations
    std::uint32_t slot_count { 0 };

    [[nodiscard]] LineIndex last_source_line() const { return sc::util::retreat(line_starts.next_index()); }

    [[nodiscard]] std::string_view line(LineIndex i) const {
        const auto begin = line_starts[i];
        const auto next = sc::util::advance(i);
        const auto end = line_starts.contains(next) ? line_starts[next] - 1 : txt.size();
        return txt.substr(begin, end - begin);
    }
    [[nodiscard]] LineIndex line_of(std::size_t absolute) const {
        const auto& starts = line_starts.raw();
        const auto it = std::upper_bound(starts.begin(), starts.end(), absolute);
        return LineIndex::from(it - starts.begin() - 1);
    }
};

LineIndex back(LineIndex l, std::uint32_t n) { return *l > n ? sc::util::retreat(l, n) : LineIndex { 0 }; }

struct Step {
    sc::src::CodePoint cp;
    std::size_t size;
};

Step step(std::string_view line, std::size_t at) {
    const auto [cp, size] = sc::src::utf8_sequence_to_codepoint(line.data() + at, line.size() - at);
    return { cp, size == 0 ? std::size_t { 1 } : std::size_t { size } };
}

// The whitespace taking the same space as the bytes in [from, to), keeping tabs.
std::string whitespace(std::string_view line, std::size_t from, std::size_t to) {
    std::string out;
    to = std::min(to, line.size());
    for (auto at = from; at < to;) {
        const auto [cp, size] = step(line, at);
        out += cp == sc::src::invalid_utf8_flag ? " " : sc::src::codepoint_as_whitespace(cp);
        at += size;
    }
    return out;
}

// Columns taken by the bytes in [from, to), at least one.
std::size_t display_width(std::string_view line, std::size_t from, std::size_t to) {
    std::size_t w { 0 };
    to = std::min(to, line.size());
    for (auto at = from; at < to;) {
        const auto [cp, size] = step(line, at);
        w += cp == sc::src::invalid_utf8_flag ? 1 : sc::src::codepoint_width(cp);
        at += size;
    }
    return std::max<std::size_t>(w, 1);
}

std::size_t start_of_last_codepoint(std::string_view line, std::size_t end) {
    end = std::min(end, line.size());
    std::size_t last { 0 };
    for (std::size_t at = 0; at < end;) {
        last = at;
        at += step(line, at).size;
    }
    return last;
}

// The byte after the codepoint at `start`.
std::size_t after_codepoint(std::string_view line, std::size_t start) {
    return start < line.size() ? start + step(line, start).size : start;
}

bool same_text(const Highlight& a, const Highlight& b) {
    const auto same_path = a.file_path == b.file_path || (a.file_path && b.file_path && *a.file_path == *b.file_path);
    return a.txt.data() == b.txt.data() && a.txt.size() == b.txt.size() && same_path
        && a.txt_start_in_file == b.txt_start_in_file;
}

void find_line_starts(Group& g) {
    g.line_starts.push_back(0);
    for (std::size_t i = 0; i < g.txt.size(); ++i)
        if (g.txt[i] == '\n')
            g.line_starts.push_back(i + 1);
}

Annotation resolve(const Group& g, const Highlight& h) {
    const auto begin_abs = std::min<std::size_t>(h.range.begin.absolute, g.txt.size());
    const auto end_abs = std::max<std::size_t>(std::min<std::size_t>(h.range.end.absolute, g.txt.size()), begin_abs);

    Annotation a {};
    a.description = h.description;
    a.first_line = g.line_of(begin_abs);
    a.begin_byte = begin_abs - g.line_starts[a.first_line];
    a.last_line = g.line_of(end_abs);
    a.end_byte = end_abs - g.line_starts[a.last_line];

    // A range finishing at the start of a line, e.g., after a trailing newline, ends on the line before.
    if (a.last_line != a.first_line && a.end_byte == 0) {
        a.last_line = sc::util::retreat(a.last_line);
        a.end_byte = g.line(a.last_line).size();
    }
    return a;
}

void stream_location(std::ostream& ss, const Highlight& h, const std::filesystem::path* relative_to) {
    const auto starting_line = h.txt_start_in_file.line_number + h.range.begin.line_number;
    // Where the range starts, counting from zero. The text's start column only applies to its first line.
    const auto start_column = (h.range.begin.line_number == 0 ? h.txt_start_in_file.column : 0) + h.range.begin.column;

    if (h.file_path && relative_to) {
        const auto relative =
            std::filesystem::absolute(*h.file_path).lexically_relative(std::filesystem::absolute(*relative_to));
        // Empty when there is no relative path, e.g., on another drive on Windows.
        ss << (relative.empty() ? std::filesystem::absolute(*h.file_path) : relative).generic_string() << ':'
           << starting_line + 1 << ':' << start_column + 1 << '\n';
    } else if (h.file_path) {
        const auto abs = std::filesystem::absolute(*h.file_path).generic_string();
        // A URI path starts with a slash, which a Windows drive letter doesn't, i.e., `file:///C:/...`.
        ss << "file://" << (abs.front() == '/' ? "" : "/") << abs << ':' << starting_line + 1 << ':' << start_column + 1
           << '\n';
    } else
        ss << "unknown file:" << starting_line + 1 << ':' << start_column + 1 << '\n';
}

// Gives every multi line annotation a gutter column, so ones that overlap don't share.
void assign_slots(Group& g) {
    sc::util::IndexedVector<SlotIndex, LineIndex> busy_until; // per slot, the last line it is used on
    std::vector<Annotation*> multi;
    for (auto& a : g.annotations)
        if (a.multi_line())
            multi.push_back(&a);
    std::stable_sort(multi.begin(), multi.end(),
                     [](const Annotation* l, const Annotation* r) { return *l->first_line < *r->first_line; });
    for (auto* a : multi) {
        bool placed { false };
        for (const auto slot : busy_until.indices()) {
            if (*busy_until[slot] < *a->first_line) {
                busy_until[slot] = a->last_line;
                a->slot = slot;
                placed = true;
                break;
            }
        }
        if (!placed)
            a->slot = busy_until.push_back(a->last_line);
    }
    g.slot_count = static_cast<std::uint32_t>(busy_until.size());
}

std::string gutter_cells(const Group& g, const std::function<const char*(const Annotation&)>& cell_of) {
    sc::util::IndexedVector<SlotIndex, const char*> cells;
    cells.resize(g.slot_count);
    for (auto& c : cells.raw())
        c = "  ";
    for (const auto& a : g.annotations)
        if (a.multi_line())
            if (const auto* c = cell_of(a))
                cells[a.slot] = c;
    std::string out;
    for (const auto* c : cells.raw())
        out += c;
    return out;
}

// Gutter columns of multi line annotations, for a source line, the rows below it, and the lines elided between.
enum struct GutterAt { SourceLine, RowsBelow };

std::string gutter(const Group& g, LineIndex line, GutterAt at) {
    return gutter_cells(g, [&](const Annotation& a) -> const char* {
        if (at == GutterAt::SourceLine) {
            if (a.first_line == line)
                return "╭ ";
            if (*a.first_line < *line && *line <= *a.last_line)
                return "│ ";
        } else if (*a.first_line <= *line && *line < *a.last_line)
            return "│ ";
        return nullptr;
    });
}

std::string gutter_across(const Group& g, LineIndex from, LineIndex to) {
    return gutter_cells(g, [&](const Annotation& a) -> const char* {
        return *a.first_line < *from && *to < *a.last_line ? "│ " : nullptr;
    });
}

void stream_single_line_rows(std::ostream& ss, const Group& g, LineIndex line_no, std::string_view line,
                             std::string_view line_prefix) {
    struct Mark {
        const Annotation* annotation;
        std::size_t start; // byte in the line
    };
    std::vector<Mark> marks;
    for (const auto& a : g.annotations)
        if (!a.multi_line() && a.first_line == line_no)
            marks.push_back({ &a, a.begin_byte });
    if (marks.empty())
        return;
    std::stable_sort(marks.begin(), marks.end(), [](const Mark& l, const Mark& r) { return l.start < r.start; });

    const auto margin = std::string { line_prefix } + annotation_margin + gutter(g, line_no, GutterAt::RowsBelow);

    // Carets for all of them, the last one with its message.
    ss << margin;
    std::size_t cursor { 0 };
    for (auto& m : marks) {
        m.start = std::max(m.start, cursor);
        const auto end = std::max(m.annotation->end_byte, m.start);
        ss << whitespace(line, cursor, m.start) << std::string(display_width(line, m.start, end), '^');
        cursor = std::max(end, m.start + 1);
    }
    ss << ' ' << marks.back().annotation->description << '\n';

    // Every other message hangs below its caret, the rightmost first.
    for (auto hanging = std::next(marks.rbegin()); hanging != marks.rend(); ++hanging) {
        ss << margin;
        std::size_t at { 0 };
        for (auto m = marks.begin(); m != hanging.base(); ++m) {
            ss << whitespace(line, at, m->start) << (std::next(m) == hanging.base() ? "└ " : "│");
            at = after_codepoint(line, m->start);
        }
        ss << hanging->annotation->description << '\n';
    }
}

void stream_end_rows(std::ostream& ss, const Group& g, LineIndex line_no, std::string_view line,
                     std::string_view line_prefix) {
    std::vector<const Annotation*> ends;
    for (const auto& a : g.annotations)
        if (a.multi_line() && a.last_line == line_no)
            ends.push_back(&a);
    std::stable_sort(ends.begin(), ends.end(),
                     [](const Annotation* l, const Annotation* r) { return *l->slot > *r->slot; });

    for (const auto* a : ends) {
        ss << line_prefix << annotation_margin;
        for (const auto s : sc::util::range(SlotIndex { 0 }, a->slot)) {
            const auto open = std::any_of(g.annotations.begin(), g.annotations.end(), [&](const Annotation& o) {
                return o.multi_line() && o.slot == s && *o.first_line <= *line_no && *line_no < *o.last_line;
            });
            ss << (open ? "│ " : "  ");
        }
        const auto last = start_of_last_codepoint(line, a->end_byte);
        const auto last_width = display_width(line, last, after_codepoint(line, last));
        const auto dashes = (g.slot_count - *a->slot) * 2 - 1 + (last == 0 ? 0 : display_width(line, 0, last));
        ss << "╰";
        for (std::size_t i = 0; i < dashes; ++i)
            ss << "─";
        ss << std::string(last_width, '^') << ' ' << a->description << '\n';
    }
}

void stream_group(std::ostream& ss, Group& g, std::string_view line_prefix) {
    assign_slots(g);

    // Lines that are always shown: the marked ones plus context.
    sc::util::IndexedVector<LineIndex, char> forced;
    forced.resize(g.line_starts.size());
    const auto force = [&](LineIndex from, LineIndex to) {
        const auto end = std::min<std::size_t>(*to + std::size_t { 1 }, forced.size());
        for (const auto l : sc::util::range(from, LineIndex::from(end)))
            forced[l] = 1;
    };
    for (const auto& a : g.annotations) {
        if (a.multi_line()) {
            force(back(a.first_line, context_before), sc::util::advance(a.first_line));
            force(sc::util::retreat(a.last_line), sc::util::advance(a.last_line, context_after));
        } else
            force(back(a.first_line, context_before), sc::util::advance(a.first_line, context_after));
    }

    const auto lo = back(g.first_line, context_before);
    const auto hi = LineIndex::from(std::min<std::size_t>(*g.last_line + context_after, *g.last_source_line()));

    for (auto l = lo; *l <= *hi; l = sc::util::advance(l)) {
        // A trailing newline starts an empty line that isn't really there, unless something points at it.
        if (l == g.last_source_line() && g.line(l).empty() && *l > *g.last_line)
            break;

        if (!forced[l]) {
            auto run_end = l;
            while (*run_end < *hi && !forced[sc::util::advance(run_end)])
                run_end = sc::util::advance(run_end);
            if (*run_end - *l + 1 >= min_elided_run) {
                ss << line_prefix << "  ... │ " << gutter_across(g, l, run_end) << '\n';
                l = run_end;
                continue;
            }
        }

        const auto line = g.line(l);
        const auto number = *l + 1 + g.primary->txt_start_in_file.line_number;
        ss << line_prefix << std::setfill(' ') << std::setw(5) << number << " │ " << gutter(g, l, GutterAt::SourceLine)
           << line << '\n';
        stream_single_line_rows(ss, g, l, line, line_prefix);
        stream_end_rows(ss, g, l, line, line_prefix);
    }
}

} // namespace

std::ostream& streamHighlights(std::ostream& ss, const std::vector<Highlight>& highlights, bool print_source,
                               std::string_view line_prefix, const std::filesystem::path* relative_to) {
    // The caller's order is kept, as the final highlight is the true location, so only a copy is sorted: by text, in
    // order of first appearance, then by position. No text is its own text, as it can't be shown with any other.
    struct Entry {
        const Highlight* highlight;
        InputIndex input_order;
        TextIndex text_id;
    };
    std::vector<Entry> entries;
    std::vector<const Highlight*> texts;
    for (const auto& h : highlights) {
        auto text = std::find_if(texts.begin(), texts.end(),
                                 [&](const Highlight* t) { return !h.txt.empty() && same_text(*t, h); });
        if (text == texts.end()) {
            texts.push_back(&h);
            text = std::prev(texts.end());
        }
        entries.push_back({ &h, InputIndex::from(entries.size()), TextIndex::from(text - texts.begin()) });
    }
    std::stable_sort(entries.begin(), entries.end(), [](const Entry& l, const Entry& r) {
        return l.text_id != r.text_id ? *l.text_id < *r.text_id
                                      : l.highlight->range.begin.absolute < r.highlight->range.begin.absolute;
    });

    std::vector<Group> groups;
    for (const auto& entry : entries) {
        const auto& h = *entry.highlight;
        if (h.txt.empty()) {
            groups.emplace_back();
            groups.back().primary = &h;
            continue;
        }
        Group* target { nullptr };
        Annotation resolved {};
        // Only the groups of this text can match, and the sort put them last.
        if (!groups.empty() && !groups.back().txt.empty() && same_text(*groups.back().primary, h)) {
            resolved = resolve(groups.back(), h);
            if (*resolved.first_line <= *groups.back().last_line + group_distance)
                target = &groups.back();
        }
        if (!target) {
            groups.emplace_back();
            target = &groups.back();
            target->txt = h.txt;
            find_line_starts(*target);
            resolved = resolve(*target, h);
            target->first_line = resolved.first_line;
            target->last_line = resolved.last_line;
        }
        if (!target->primary || *entry.input_order >= *target->primary_input_order) {
            target->primary = &h;
            target->primary_input_order = entry.input_order;
        }
        if (*resolved.last_line > *target->last_line)
            target->last_line = resolved.last_line;
        target->annotations.push_back(resolved);
    }

    bool first { true };
    for (auto& g : groups) {
        if (!first)
            ss << '\n';
        first = false;
        stream_location(ss, *g.primary, relative_to);
        if (g.txt.empty())
            ss << line_prefix << g.primary->description; // nothing to show but where it is
        else if (print_source)
            stream_group(ss, g, line_prefix);
    }
    return ss;
}

std::ostream& streamHighlight(std::ostream& ss, const Highlight& highlight, bool print_source,
                              std::string_view line_prefix, const std::filesystem::path* relative_to) {
    return streamHighlights(ss, { highlight }, print_source, line_prefix, relative_to);
}

} // namespace sc::diag
