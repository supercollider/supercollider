#include "sc_source/text_info.hpp"
#include <stdexcept>
#include <string>

namespace sc::src {

TextInfo::TextInfo(NormalisedSource src, FileCodeLocation start_in_file, std::optional<std::filesystem::path> path,
                   TextKind text_kind):
    source(std::move(src)),
    source_start_in_file(start_in_file),
    file_path(std::move(path)),
    kind(text_kind) {
    if (source.as_string().size() > max_source_size)
        throw std::length_error { "source is larger than the " + std::to_string(max_source_size) + " bytes supported" };
}

[[nodiscard]] std::string_view TextInfo::read(SourceCodeRange r) const noexcept {
    const char* str = source.as_string().c_str();
    assert(r.begin.absolute <= r.end.absolute);
    return { str + r.begin.absolute, r.size() };
}

}