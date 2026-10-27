#pragma once

#include "sc_source/text_location.hpp"
#include <cstddef>
#include <ostream>
#include <string>

struct DiagnosticHighlight {
    const char* filepath;
    const char* txt;
    size_t txt_len;
    sc::src::SourceCodeRange range_in_source;
    size_t source_loc_lin, source_loc_column;
    std::string description;
};

std::ostream& streamSourceCodeWithHighlight(std::ostream& ss, const DiagnosticHighlight& highlight,
                                            bool printSource = true, const char* newlinePrefixPtr = "",
                                            size_t newlinePrefixSz = 0);
