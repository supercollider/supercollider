set(test_source_files
    api_version_test.cpp
    shared_lib_test.cpp
    versioned_struct_test.cpp
)

set_source_files_properties(c_api_test.c PROPERTIES LANGUAGE C)

set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_SHARED_MODULE_SUFFIX ".scx")

add_library(plugin_api_test_common INTERFACE)
target_include_directories(plugin_api_test_common INTERFACE
    ${CMAKE_SOURCE_DIR}/include/common
    ${CMAKE_SOURCE_DIR}/include/plugin_interface
)

# c_api_test_plugin.scx (for now only test whether it compiles)
add_library(c_api_test_plugin MODULE plugins/c_api_test_plugin.c)
target_link_libraries(c_api_test_plugin PRIVATE plugin_api_test_common)
set_source_files_properties(c_api_test_plugin.c PROPERTIES LANGUAGE C)
set_property(TARGET c_api_test_plugin PROPERTY C_STANDARD 99)

# shared_lib_test_plugin.scx
add_library(shared_lib_test_plugin MODULE plugins/shared_lib_test_plugin.cpp)
target_link_libraries(shared_lib_test_plugin PRIVATE plugin_api_test_common)
# helper library
add_library(testlib SHARED plugins/testlib.cpp)
# The toplevel CMakeLists.txt sets CMAKE_CXX_VISIBILITY_PRESET to 'hidden' for all targets...
set_target_properties(testlib PROPERTIES CXX_VISIBILITY_PRESET default)
target_link_libraries(shared_lib_test_plugin PRIVATE testlib)

# test_plugin_v3.scx
add_library(test_plugin_v3 MODULE plugins/test_plugin_v3.cpp)
target_link_libraries(test_plugin_v3 PRIVATE plugin_api_test_common)

# test_plugin_v3_supernova.scx
add_library(test_plugin_v3_supernova MODULE plugins/test_plugin_v3.cpp)
target_link_libraries(test_plugin_v3_supernova PRIVATE plugin_api_test_common)
target_compile_definitions(test_plugin_v3_supernova PRIVATE SUPERNOVA)

# test_plugin_v5_0.scx
add_library(test_plugin_v5_0 MODULE plugins/test_plugin_v4.cpp)
target_link_libraries(test_plugin_v5_0 PRIVATE plugin_api_test_common)
target_compile_definitions(test_plugin_v5_0 PRIVATE
    TEST_API_VERSION_MAJOR=5 TEST_API_VERSION_MINOR=0
)

# test_plugin_v5_0_supernova.scx
add_library(test_plugin_v5_0_supernova MODULE plugins/test_plugin_v4.cpp)
target_link_libraries(test_plugin_v5_0_supernova PRIVATE plugin_api_test_common)
target_compile_definitions(test_plugin_v5_0_supernova PRIVATE
    TEST_API_VERSION_MAJOR=5 TEST_API_VERSION_MINOR=0 SUPERNOVA
)

# test_plugin_v4_5.scx
add_library(test_plugin_v4_5 MODULE plugins/test_plugin_v4.cpp)
target_link_libraries(test_plugin_v4_5 PRIVATE plugin_api_test_common)
target_compile_definitions(test_plugin_v4_5 PRIVATE
    TEST_API_VERSION_MAJOR=4 TEST_API_VERSION_MINOR=5
)

# test_plugin_v4_5_supernova.scx
add_library(test_plugin_v4_5_supernova MODULE plugins/test_plugin_v4.cpp)
target_link_libraries(test_plugin_v4_5_supernova PRIVATE plugin_api_test_common)
target_compile_definitions(test_plugin_v4_5_supernova PRIVATE
    TEST_API_VERSION_MAJOR=4 TEST_API_VERSION_MINOR=5 SUPERNOVA
)

# test_plugin_v4_5_min_v4_1.scx
add_library(test_plugin_v4_5_min_v4_1 MODULE plugins/test_plugin_v4.cpp)
target_link_libraries(test_plugin_v4_5_min_v4_1 PRIVATE plugin_api_test_common)
target_compile_definitions(test_plugin_v4_5_min_v4_1 PRIVATE
    TEST_API_VERSION_MAJOR=4 TEST_API_VERSION_MINOR=5
    "-DSC_MIN_REQUIRED_API_VERSION=SC_MAKE_API_VERSION(4, 1)"
)

# test_plugin_v4_5_min_v4_2.scx
add_library(test_plugin_v4_5_min_v4_2 MODULE plugins/test_plugin_v4.cpp)
target_link_libraries(test_plugin_v4_5_min_v4_2 PRIVATE plugin_api_test_common)
target_compile_definitions(test_plugin_v4_5_min_v4_2 PRIVATE
    TEST_API_VERSION_MAJOR=4 TEST_API_VERSION_MINOR=5
    "-DSC_MIN_REQUIRED_API_VERSION=SC_MAKE_API_VERSION(4, 2)"
)

# test_plugin_v4_5_rel_path.scx
add_library(test_plugin_v4_5_rel_path MODULE plugins/test_plugin_v4.cpp)
target_link_libraries(test_plugin_v4_5_rel_path PRIVATE plugin_api_test_common)
target_compile_definitions(test_plugin_v4_5_rel_path PRIVATE
    TEST_API_VERSION_MAJOR=4 TEST_API_VERSION_MINOR=5
    SC_PLUGIN_RESOURCE_DIR="./data"
)

# test_plugin_v4_5_abs_path.scx
add_library(test_plugin_v4_5_abs_path MODULE plugins/test_plugin_v4.cpp)
target_link_libraries(test_plugin_v4_5_abs_path PRIVATE plugin_api_test_common)
target_compile_definitions(test_plugin_v4_5_abs_path PRIVATE
    TEST_API_VERSION_MAJOR=4 TEST_API_VERSION_MINOR=5
    SC_PLUGIN_RESOURCE_DIR="$<TARGET_FILE_DIR:test_plugin_v4_5_abs_path>/data"
)

set(api_version_test_plugins
    test_plugin_v3
    test_plugin_v3_supernova
    test_plugin_v5_0
    test_plugin_v5_0_supernova
    test_plugin_v4_5
    test_plugin_v4_5_supernova
    test_plugin_v4_5_min_v4_1
    test_plugin_v4_5_min_v4_2
    test_plugin_v4_5_rel_path
    test_plugin_v4_5_abs_path
)

# put all plugins in a subdirectory. this way we can also check that plugins
# can load helper libraries in the same directory (which is *not* the directory
# of the main application).
# NOTE: only include the current CONFIG for multi-config generators!
if(CMAKE_CONFIGURATION_TYPES)
    set(plugin_dir "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/plugins")
else()
    set(plugin_dir "${CMAKE_CURRENT_BINARY_DIR}/plugins")
endif()

set_target_properties(
    ${api_version_test_plugins}
    c_api_test_plugin
    shared_lib_test_plugin
    testlib
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${plugin_dir}"
    LIBRARY_OUTPUT_DIRECTORY "${plugin_dir}"
    ARCHIVE_OUTPUT_DIRECTORY "${plugin_dir}"
    FOLDER Test/plugin_api/plugins
)

# the actual tests
foreach(test_file ${test_source_files})
    string(REPLACE .cpp "" test_name ${test_file} )
    add_executable(${test_name}
        ${test_file}
        # for plugin loading
        ${CMAKE_SOURCE_DIR}/common/SC_Codecvt.cpp
        ${CMAKE_SOURCE_DIR}/common/SC_ServerPluginModule.cpp
    )
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/common)
    target_link_libraries(${test_name} PRIVATE plugin_api_test_common boost_test)

    add_test(
        NAME ${test_name}_run
        COMMAND ${test_name}
        # set the working directory to the target file dir so the tests themselves
        # can use relative file paths, e.g. "./plugins/<name>.scx"
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${test_name}>
    )

    set_property(TARGET ${test_name} PROPERTY FOLDER Test/plugin_api)
endforeach()

# set dependencies
add_dependencies(api_version_test ${api_version_test_plugins})
add_dependencies(shared_lib_test shared_lib_test_plugin testlib)

# api_version_test assumes API version 4.1, so we need to overwrite the SC API version
target_compile_definitions(api_version_test PRIVATE
    TEST_API_VERSION_MAJOR=4 TEST_API_VERSION_MINOR=1
)

# This is needed so that versioned_struct_test.cpp compiles, which implicitly tests
# that the SC_ENABLE_ALL_API_EXTENSIONS option works as intended.
target_compile_definitions(versioned_struct_test PRIVATE SC_ENABLE_ALL_API_EXTENSIONS=1)

# here we test that versioned_struct_test.cpp only compiles if we set the appropriate
# min. required API version.
foreach(min_version 0x0400 0x0401 0x0402 0x0403)
    set(name versioned_struct_test_${min_version})
    # exclude from "all" target!
    add_executable(${name} EXCLUDE_FROM_ALL versioned_struct_test.cpp)
    # also exclude from default build (e.g. "Build solution" in Visual Studio)
    set_target_properties(${name} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)

    target_link_libraries(${name} PRIVATE plugin_api_test_common boost_test)
    target_compile_definitions(${name} PRIVATE SC_MIN_REQUIRED_API_VERSION=${min_version})

    add_test(
        NAME ${name}_run
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${name} --config $<CONFIG>
    )

    # all min. versions except 0x0403 should fail to compile!
    if (NOT ${min_version} STREQUAL 0x0403)
        set_tests_properties(${name}_run PROPERTIES WILL_FAIL TRUE)
    endif()

    # the resource lock prevents concurrent cmake invocations in case the ctests run in parallel.
    set_tests_properties(${name}_run PROPERTIES RESOURCE_LOCK plugin_api_tests)
endforeach()
