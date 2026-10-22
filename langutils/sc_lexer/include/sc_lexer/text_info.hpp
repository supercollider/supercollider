// Copyright Jordan Henderson 2026
#pragma once

#include "normalise_source.hpp"
#include "sc_lexer/text_location.hpp"
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>

namespace sc::lex {

struct TextInfo {
    // Throws std::length_error if the source is larger than max_source_size, as positions are 32 bit.
    TextInfo(NormalisedSource src, FileCodeLocation start_in_file, std::optional<std::filesystem::path> path,
             bool class_file):
        source(std::move(src)),
        source_start_in_file(start_in_file),
        file_path(std::move(path)),
        is_class_file(class_file) {
        if (source.as_string().size() > max_source_size)
            throw std::length_error { "source is larger than the " + std::to_string(max_source_size)
                                      + " bytes supported" };
    }

    NormalisedSource source;
    FileCodeLocation source_start_in_file;
    std::optional<std::filesystem::path> file_path; // can be nullptr;
    bool is_class_file;

    [[nodiscard]] std::string_view read(SourceCodeRange r) const noexcept {
        const char* str = source.as_string().c_str();
        assert(r.begin.absolute <= r.end.absolute);
        return { str + r.begin.absolute, r.size() };
    }
};

}
