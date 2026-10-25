include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

add_library(sc_source STATIC
    src/codepoint.cpp
    src/text_location.cpp
    src/source_utils.cpp
)

target_include_directories(sc_source PUBLIC include)

target_link_libraries(sc_source PRIVATE utf8proc)
set_target_compiler_strict_errors(sc_source)
set_property(TARGET sc_source PROPERTY FOLDER Libs)
