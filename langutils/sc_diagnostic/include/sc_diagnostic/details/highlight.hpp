#pragma once

#include "sc_diagnostic/style.hpp"
#include "sc_source/text_location.hpp"

#include <filesystem>
#include <ostream>
#include <string_view>
#include <vector>

namespace sc::diag {

struct Highlight {
    const std::filesystem::path* file_path; // may be null
    std::string_view txt;
    sc::src::SourceCodeRange range;
    sc::src::FileCodeLocation txt_start_in_file; // where txt starts within the file
    std::string_view description;
};

// Prints `file:line:col` of the final highlight, then the surrounding source with every range marked.
// Highlights in the same text that are close together share one listing. `line_prefix` begins every line after the
// first, `relative_to` makes the path relative rather than a file:// URI. The marks of the final highlight, the true
// location, are written with `true_location_role` (`Error` or `Warning`), the others as `Secondary`.
std::ostream& streamHighlights(std::ostream& ss, const std::vector<Highlight>& highlights, bool print_source = true,
                               std::string_view line_prefix = {}, const std::filesystem::path* relative_to = nullptr,
                               const Style& style = plain_style(), Role true_location_role = Role::Error);

std::ostream& streamHighlight(std::ostream& ss, const Highlight& highlight, bool print_source = true,
                              std::string_view line_prefix = {}, const std::filesystem::path* relative_to = nullptr,
                              const Style& style = plain_style(), Role true_location_role = Role::Error);

} // namespace sc::diag
