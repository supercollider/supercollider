# SCDoc's parser, without SCDocPrim.cpp, which needs the VM.
include(${CMAKE_SOURCE_DIR}/SCDoc/CMakeLists.txt)
add_executable(sc_check_docs
    main.cpp
    ${SCDOC_DIR}/SCDoc.cpp
    ${SCDOC_DIR}/SCDoc.tab.cpp
    ${SCDOC_DIR}/lex.scdoc.cpp
)
if(WIN32)
    target_sources(sc_check_docs PRIVATE ${CMAKE_SOURCE_DIR}/common/SC_Codecvt.cpp)
endif()
target_include_directories(sc_check_docs PRIVATE ${SCDOC_DIR} ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(sc_check_docs PRIVATE sc_compiler Boost::thread)
set_property(TARGET sc_check_docs PROPERTY FOLDER LangUtils)

# Every code block in HelpSource and every scd file in examples compiles, see README.md.
add_test(NAME sc_doc.compile_all
    COMMAND sc_check_docs --library ${CMAKE_SOURCE_DIR}/SCClassLibrary --relative-to ${CMAKE_SOURCE_DIR}
        --exclude PG_Cookbook --exclude Understanding-Errors
        ${CMAKE_SOURCE_DIR}/HelpSource ${CMAKE_SOURCE_DIR}/examples
)
