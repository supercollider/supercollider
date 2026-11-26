#pragma once

#include <ostream>
#include <string>
#include <vector>

// plain text of the help documents for the JS full-text search index
struct SCDocTextSource {
    std::string path; // e.g. "Classes/SinOsc"
    std::string fullPath; // the .schelp file
    std::string title;
    std::string summary;
    bool isExtension;
    std::vector<std::string> additions; // .ext.schelp files
};

/**
 * @brief Parses the given vector of structs and writes their content into JSON.
 *
 * Writes \code doctext = [...] \endcode, with one entry per section and method.
 * Each entry has the anchor that \c SCDocHTMLRenderer uses, so search results can link to that section.
 * Methods from additions are included.
 * \c copymethod:: is not added, since the copied text is already indexed at its source.
 *
 * \returns the number of entries written
 */
int scdocExportSearchIndex(const std::vector<SCDocTextSource>& docs, std::ostream& out);
