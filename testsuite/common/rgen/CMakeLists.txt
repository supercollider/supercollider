cmake_minimum_required(VERSION 3.12)

# This project lives outside the main source tree and must be built separately.
# See the accompanying README.md for more details and build instructions.

project(rgen_test_data)

set(SC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../.." CACHE PATH "Path to SC source code")

include(${SC_PATH}/SCVersion.txt)

configure_file(${SC_PATH}/common/SC_Version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/SC_Version.hpp)

set(CMAKE_CXX_STANDARD 17)

# This is only needed by the generate_test_data target below.
add_executable(rgen_generate EXCLUDE_FROM_ALL ${CMAKE_CURRENT_LIST_DIR}/rgen_generate.cpp)
# Unfortunately, the SC version is not available as a preprocessor define
# so we have to pass it explicitly. See rgen_generate.cpp for details.
target_compile_definitions(rgen_generate PRIVATE
    SC_VERSION_MAJOR=${SC_VERSION_MAJOR}
    SC_VERSION_MINOR=${SC_VERSION_MINOR}
    SC_VERSION_PATCH=${SC_VERSION_PATCH}
)
target_include_directories(rgen_generate PRIVATE
    ${SC_PATH}/include/common
    ${SC_PATH}/include/plugin_interface
    ${CMAKE_CURRENT_BINARY_DIR} # for SC_Version.hpp
)

set(output_file rgen_output_${SC_VERSION_MAJOR}_${SC_VERSION_MINOR}_${SC_VERSION_PATCH}.hpp)

add_custom_target(generate_test_data
    COMMAND rgen_generate ${CMAKE_CURRENT_SOURCE_DIR}/${output_file}
    DEPENDS rgen_generate
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
    VERBATIM
)
