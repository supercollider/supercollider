// Copyright Jordan Henderson 2026
#pragma once

#include "sc_source/normalise_source.hpp"
#include "sc_source/text_location.hpp"

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <optional>

namespace sc::src {

// How the text is parsed. An scd file is a list of regions, which only tools see, as the VM runs interpreted code,
// including an executed scd file, as one function, as the old grammar did.
enum struct TextKind : std::uint8_t { ClassFile, ScdFile, InterpretedFunction };

struct TextInfo {
    // Throws std::length_error if the source is larger than max_source_size, as positions are 32 bit.
    TextInfo(NormalisedSource src, FileCodeLocation start_in_file, std::optional<std::filesystem::path> path,
             TextKind text_kind);

    [[nodiscard]] std::string_view read(SourceCodeRange r) const noexcept;

    NormalisedSource source;
    FileCodeLocation source_start_in_file;
    std::optional<std::filesystem::path> file_path; // empty when the text has no file;
    TextKind kind;
};

}
