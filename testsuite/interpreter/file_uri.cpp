#include <boost/test/unit_test.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include "CompilerContext.hpp"

static std::string fileUri(std::string_view path, PathOs os, std::optional<FileUriLocation> location = std::nullopt) {
    std::stringstream ss;
    streamFileUri(ss, path, location, os);
    return ss.str();
}

BOOST_AUTO_TEST_CASE(file_uri_encoding) {
    for (const auto os : { PathOs::Posix, PathOs::Windows }) {
        BOOST_TEST(fileUri("/home/user/a.scd", os) == "file:///home/user/a.scd");
        BOOST_TEST(fileUri("/home/a b/c d.scd", os) == "file:///home/a%20b/c%20d.scd");
        BOOST_TEST(fileUri("/odd#?%.scd", os) == "file:///odd%23%3F%25.scd");
        BOOST_TEST(fileUri("/kept-._~:.scd", os) == "file:///kept-._~:.scd");
        BOOST_TEST(fileUri("/M\xC3\xBAsica.scd", os) == "file:///M%C3%BAsica.scd");
    }
}

BOOST_AUTO_TEST_CASE(file_uri_windows) {
    const auto os = PathOs::Windows;
    BOOST_TEST(fileUri(R"(C:\Users\a b\x.scd)", os) == "file:///C:/Users/a%20b/x.scd");
    BOOST_TEST(fileUri("C:/Users/x.scd", os) == "file:///C:/Users/x.scd");
    BOOST_TEST(fileUri(R"(\\server\share\x.scd)", os) == "file://server/share/x.scd");
    BOOST_TEST(fileUri("//server/share/x.scd", os) == "file://server/share/x.scd");
    BOOST_TEST(fileUri(R"(\\?\C:\long\x.scd)", os) == "file:///C:/long/x.scd");
    BOOST_TEST(fileUri(R"(\\?\UNC\server\share\x.scd)", os) == "file://server/share/x.scd");
}

BOOST_AUTO_TEST_CASE(file_uri_posix) {
    const auto os = PathOs::Posix;
    // A backslash is an ordinary filename character here, not a separator.
    BOOST_TEST(fileUri(R"(/home/back\slash.scd)", os) == "file:///home/back%5Cslash.scd");
    BOOST_TEST(fileUri("//double/x.scd", os) == "file:////double/x.scd");
    // On POSIX this is one oddly named file, not a drive and folders.
    BOOST_TEST(fileUri(R"(C:\a\b.scd)", os) == "file:///C:%5Ca%5Cb.scd");
}

BOOST_AUTO_TEST_CASE(file_uri_defaults_to_current_os) {
    std::stringstream ss;
    streamFileUri(ss, R"(C:\a\b.scd)");
    BOOST_TEST(ss.str() == fileUri(R"(C:\a\b.scd)", currentPathOs));
}

BOOST_AUTO_TEST_CASE(file_uri_location_fragment) {
    // 0-based line and column in, 1-based fragment out.
    BOOST_TEST(fileUri("/a.scd", PathOs::Posix, FileUriLocation { 0, 0 }) == "file:///a.scd#1:1");
    BOOST_TEST(fileUri("/a.scd", PathOs::Posix, FileUriLocation { 12, 4 }) == "file:///a.scd#13:5");
    BOOST_TEST(fileUri(R"(C:\a b.scd)", PathOs::Windows, FileUriLocation { 2, 9 }) == "file:///C:/a%20b.scd#3:10");
    // The fragment is written after encoding, so its '#' and ':' stay as they are.
    BOOST_TEST(fileUri("/x#1:2.scd", PathOs::Posix, FileUriLocation { 0, 0 }) == "file:///x%231:2.scd#1:1");
}
