#pragma once

#include "sc_source/text_location.hpp"

#include <cstddef>
#include <filesystem>
#include <ostream>
#include <string_view>

namespace sc::diag {

struct Highlight {
    const std::filesystem::path* file_path; // may be null
    std::string_view txt;
    sc::src::SourceCodeRange range;
    sc::src::FileCodeLocation txt_start_in_file; // where txt starts within the file
    std::string_view description;
};

/// Prints `file:line:col`, then the surrounding source with the range marked. `line_prefix` begins every line
/// after the first, `relative_to` makes the path relative rather than a file:// URI.
std::ostream& streamHighlight(std::ostream& ss, const Highlight& highlight, bool print_source = true,
                              std::string_view line_prefix = {}, const std::filesystem::path* relative_to = nullptr);

} // namespace sc::diag
