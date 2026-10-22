add_library(sc_describe STATIC describe.cpp)
target_include_directories(sc_describe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(sc_describe PUBLIC sc_sema sc_parser sc_lexer sc_diagnostic sc_util)
set_property(TARGET sc_describe PROPERTY FOLDER LangUtils)
