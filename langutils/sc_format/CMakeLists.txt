include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

add_library(sc_format STATIC
    src/format.cpp
    src/layout/columns.cpp
    src/layout/layout_draft.cpp
    src/layout/line_breaks.cpp
    src/layout/print.cpp
    src/layout/spacing.cpp
    src/layout/syntax_marker.cpp
    src/layout/verbatim.cpp
    src/parse.cpp
    src/rewrite/control_prefix.cpp
    src/rewrite/parameter_lists.cpp
    src/rewrite/rewrite.cpp
    src/rewrite/same_ast.cpp
    src/rewrite/semicolons.cpp
    src/rewrite/trailing_closures.cpp
    src/style.cpp
    src/syntax.cpp
    src/verbatim_lines.cpp
)
target_include_directories(sc_format PUBLIC include)
target_include_directories(sc_format PRIVATE src)
target_link_libraries(sc_format PRIVATE sc_consteval sc_parser sc_lexer sc_source)
set_property(TARGET sc_format PROPERTY FOLDER Libs)

# The command, installed next to sclang, as sclang is. The IDE links the library.
if(NOT EMSCRIPTEN)
    add_executable(sc_format_cli cli/main.cpp)
    set_target_properties(sc_format_cli PROPERTIES OUTPUT_NAME sc_format)
    target_link_libraries(sc_format_cli PRIVATE sc_format sc_source Boost::program_options)
    # The bundled Boost isn't an imported target, so its headers would get the strict warnings below.
    get_target_property(boost_program_options_includes Boost::program_options INTERFACE_INCLUDE_DIRECTORIES)
    if(boost_program_options_includes)
        target_include_directories(sc_format_cli SYSTEM PRIVATE ${boost_program_options_includes})
    endif()
    if(WIN32)
        target_sources(sc_format_cli PRIVATE ${CMAKE_SOURCE_DIR}/common/SC_Codecvt.cpp)
        target_include_directories(sc_format_cli PRIVATE ${CMAKE_SOURCE_DIR}/common)
    endif()
    set_target_compiler_strict_errors(sc_format_cli)
    # GCC warns inside Boost's program_options, where a value is stored in a vector, after inlining it into main.cpp,
    # so marking its headers as system headers doesn't hide it.
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(sc_format_cli PRIVATE -Wno-null-dereference)
    endif()
    set_property(TARGET sc_format_cli PROPERTY FOLDER LangUtils)

    if(APPLE)
        if(scappbindir)
            add_custom_command(TARGET sc_format_cli POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/${scappbindir}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:sc_format_cli>
                        ${CMAKE_INSTALL_PREFIX}/${scappbindir}
            )
        endif()
    elseif(WIN32)
        install(TARGETS sc_format_cli
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
            DESTINATION "${SC_WIN_BUNDLE_NAME}"
        )
    else()
        install(TARGETS sc_format_cli
            RUNTIME DESTINATION "bin"
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
    endif()
endif()

if(ENABLE_TESTSUITE)
    add_subdirectory(tests)
endif()
