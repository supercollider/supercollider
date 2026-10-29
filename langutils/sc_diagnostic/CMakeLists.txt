include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

add_library(sc_diagnostic STATIC 
    src/highlight.cpp
    src/impl.cpp
)

# todo: enable warnings

target_include_directories(sc_diagnostic PUBLIC include)
target_link_libraries(sc_diagnostic PUBLIC sc_source PRIVATE sc_lexer sc_util)
set_property(TARGET sc_diagnostic PROPERTY FOLDER Libs)

if(ENABLE_TESTSUITE)
    add_subdirectory(test)
endif()
