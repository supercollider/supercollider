// NOTE: we must set SC_MIN_REQUIRED_API_VERSION to at least 4.3,
// otherwise the file fails to compile. We use this to test whether
// the compile time API versioning work as excepted.
// Alternatively, we can set SC_ENABLE_ALL_API_EXTENSIONS to 1,
// which we do for the "normal" runtime test.
// The definitions are set via cmake, see CMakeLists.txt.

#include "SC_APIVersion.h"

// override the version number
#undef SC_API_VERSION_MAJOR
#define SC_API_VERSION_MAJOR 4

#undef SC_API_VERSION_MINOR
#define SC_API_VERSION_MINOR 0

#include "SC_Types.h"

#include <boost/test/unit_test.hpp>

#include <cassert>

#ifdef BUILDING_SCSYNTH
#    error BUILDING_SCSYNTH should not be defined
#endif

// NOTE: use boolean members to make sure that SC_IMPLEMENTED_STRUCT_SIZE
// is not influenced by padding.
struct TestStruct {
    size_t mStructSize;
    SCBool a;
#if SC_API_EXTENSION(4, 1)
    SCBool b;
#endif
#if SC_API_EXTENSION(4, 2)
    SCBool c;
#endif
#if SC_API_EXTENSION(4, 3)
    SCBool d;
#endif
};

BOOST_AUTO_TEST_CASE(versioned_struct_test) {
    // pretend that TestStruct does not implement d
    TestStruct test;
    test.mStructSize = SC_IMPLEMENTED_STRUCT_SIZE(TestStruct, c);

    BOOST_TEST(SC_IMPLEMENTS_FIELD(&test, TestStruct, a) == true);
    BOOST_TEST(SC_IMPLEMENTS_FIELD(&test, TestStruct, b) == true);
    BOOST_TEST(SC_IMPLEMENTS_FIELD(&test, TestStruct, c) == true);
    BOOST_TEST(SC_IMPLEMENTS_FIELD(&test, TestStruct, d) == false);
}