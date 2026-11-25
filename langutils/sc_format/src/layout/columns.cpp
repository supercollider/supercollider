// Copyright Jordan Henderson 2026
#include "layout/passes.hpp"

#include "text.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace sc::format {

namespace {

// How many code points text has, decoded as the lexer does, an invalid byte counting as one.
std::uint32_t count_code_points(std::string_view text) {
    std::uint32_t code_points { 0 };
    for (auto remaining = text; !remaining.empty(); ++code_points)
        remaining.remove_prefix(std::max<std::size_t>(std::get<1>(first_codepoint(remaining)), 1));
    return code_points;
}

// Pads each element of a table's rows after its comma, so the elements start in columns, and lines up comments
// ending the rows as one more column. Widths are in code points, as printed.
class ColumnAligner {
public:
    ColumnAligner(LayoutDraft& layout, const TablesToAlign& tables): m_layout(layout), m_tables(tables) {}

    void run() {
        for (const auto& table : m_tables.tables)
            align(table);
    }

private:
    struct ColumnTag;
    using ColumnIndex = util::StrongIndex<std::uint32_t, ColumnTag>;

    // A row's last cell, and how wide the row is up to that cell's comma.
    struct Row {
        TableCellIndex last;
        std::uint32_t width;
    };

    std::uint32_t printed_width_of_whitespace_before(TokenIndex token) const {
        const auto spaces = m_layout.whitespace_before[token].spaces;
        if (spaces != Whitespace::spaces_as_written)
            return spaces;
        const auto written = m_layout.written_whitespace_before(token);
        return written.find('\n') != std::string_view::npos ? 1 : count_code_points(written);
    }

    std::uint32_t printed_width(const TableCell& cell) const {
        std::uint32_t width { 0 };
        for (auto token = cell.first_token; *token <= *cell.comma; token = util::advance(token)) {
            if (token != cell.first_token)
                width += printed_width_of_whitespace_before(token);
            width += count_code_points(m_layout.token_text(token));
        }
        return width;
    }

    void align(const TableToAlign& table) {
        const auto measured = measure(table);
        const auto rows = pad_cells_to_columns(table, measured);
        align_comments_ending_rows(rows);
    }

    struct MeasuredCell {
        ColumnIndex column;
        std::uint32_t width;
    };

    struct MeasuredTable {
        std::vector<MeasuredCell> cells; // in the table's order
        util::IndexedVector<ColumnIndex, std::uint32_t> column_widths;
    };

    MeasuredTable measure(const TableToAlign& table) const {
        const auto& cells = m_tables.cells;
        MeasuredTable measured;
        ColumnIndex column { 0 };
        for (auto cell = table.begin; cell != table.end; cell = util::advance(cell)) {
            column = cells[cell].starts_row ? ColumnIndex { 0 } : util::advance(column);
            const auto width = printed_width(cells[cell]);
            measured.cells.push_back({ column, width });
            if (!measured.column_widths.contains(column))
                (void)measured.column_widths.push_back(0);
            measured.column_widths[column] = std::max(measured.column_widths[column], width);
        }
        return measured;
    }

    // Each cell but a row's last is padded to its column's width.
    std::vector<Row> pad_cells_to_columns(const TableToAlign& table, const MeasuredTable& measured) {
        const auto& cells = m_tables.cells;
        std::vector<Row> rows;
        for (auto cell = table.begin; cell != table.end; cell = util::advance(cell)) {
            const auto& [column, width] = measured.cells[*cell - *table.begin];
            if (cells[cell].starts_row)
                rows.push_back({ cell, 0 });
            auto& row = rows.back();
            row.last = cell;
            const auto next_cell = util::advance(cell);
            const bool row_end = next_cell == table.end || cells[next_cell].starts_row;
            if (row_end) {
                row.width += width;
                continue;
            }
            // The next cell, as expand_trailing_comma_list() records no table with a comment between cells on one line.
            const auto column_width = measured.column_widths[column];
            row.width += column_width + 1;
            set_spaces_before(cells[next_cell].first_token, 1 + column_width - width);
        }
        return rows;
    }

    // Comments ending the rows, as one more column.
    void align_comments_ending_rows(const std::vector<Row>& rows) {
        std::uint32_t widest_row { 0 };
        for (const auto& row : rows)
            widest_row = std::max(widest_row, row.width);
        for (const auto& row : rows) {
            const auto token_after_comma = util::advance(m_tables.cells[row.last].comma);
            const bool comment_ends_row =
                m_layout.is_comment(token_after_comma) && m_layout.whitespace_before[token_after_comma].newlines == 0;
            if (comment_ends_row)
                set_spaces_before(token_after_comma, 1 + widest_row - row.width);
        }
    }

    void set_spaces_before(TokenIndex token, std::uint32_t spaces) {
        if (spaces < Whitespace::spaces_as_written)
            m_layout.whitespace_before[token].spaces = static_cast<std::uint16_t>(spaces);
    }

    LayoutDraft& m_layout;
    const TablesToAlign& m_tables;
};

}

void align_table_columns(LayoutDraft& layout, const TablesToAlign& tables, const Style& style) {
    if (style.align_columns == Switch::On)
        ColumnAligner { layout, tables }.run();
}
}
