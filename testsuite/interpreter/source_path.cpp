#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <string>

#include "CompilerContext.hpp"
#include "SC_Codecvt.hpp"

namespace fs = std::filesystem;

// The expected result, built the same way on every platform: <cwd>/relative, in UTF-8.
static std::string inCurrentPath(const std::string& relativeUtf8) {
    return SC_Codecvt::path_to_utf8_str(
        (fs::current_path() / SC_Codecvt::utf8_str_to_path(relativeUtf8)).lexically_normal());
}

BOOST_AUTO_TEST_CASE(source_path_absolute_is_kept) {
    const auto cwd = SC_Codecvt::path_to_utf8_str(fs::current_path() / "a.scd");
    BOOST_TEST(!absoluteSourcePath(cwd).has_value());
}

BOOST_AUTO_TEST_CASE(source_path_relative_is_resolved) {
    BOOST_TEST(absoluteSourcePath("a.scd").value() == inCurrentPath("a.scd"));
    BOOST_TEST(absoluteSourcePath("sub/../a.scd").value() == inCurrentPath("a.scd"));
}

BOOST_AUTO_TEST_CASE(source_path_utf8_round_trips) {
    // On Windows this goes through UTF-16 and back.
    const std::string name = "d\xC3\xA9j\xC3\xA0/\xE6\x97\xA5\xE6\x9C\xAC.scd"; // déjà/日本.scd
    const auto result = absoluteSourcePath(name).value();
    BOOST_TEST(result == inCurrentPath(name));
    BOOST_TEST(result.find("d\xC3\xA9j\xC3\xA0") != std::string::npos);
    BOOST_TEST(result.find("\xE6\x97\xA5\xE6\x9C\xAC.scd") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(source_path_tilde_is_expanded) {
    const auto result = absoluteSourcePath("~/a.scd").value();
    BOOST_TEST(SC_Codecvt::utf8_str_to_path(result).is_absolute());
    BOOST_TEST(result.find('~') == std::string::npos);
}

BOOST_AUTO_TEST_CASE(source_path_invalid_utf8_does_not_throw) {
    BOOST_CHECK_NO_THROW((void)absoluteSourcePath("\xFF\xFE.scd"));
}

#ifdef _WIN32
BOOST_AUTO_TEST_CASE(source_path_windows) {
    // Forward slashes, as the IDE sends them, are already absolute and kept as given.
    BOOST_TEST(!absoluteSourcePath("C:/Users/a.scd").has_value());
    BOOST_TEST(!absoluteSourcePath(R"(C:\Users\a.scd)").has_value());
    BOOST_TEST(!absoluteSourcePath(R"(\\server\share\a.scd)").has_value());

    // A root without a drive is not absolute on Windows; it takes the current drive.
    const auto rooted = absoluteSourcePath(R"(\a.scd)").value();
    BOOST_TEST(SC_Codecvt::utf8_str_to_path(rooted).has_root_name());

    // Without a valid conversion to UTF-16 there is nothing to resolve.
    BOOST_TEST(!absoluteSourcePath("\xFF\xFE.scd").has_value());
}
#endif
