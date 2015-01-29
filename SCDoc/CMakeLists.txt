set(SCDOC_DIR ${CMAKE_SOURCE_DIR}/SCDoc)

set(SCDOC_SRCS
    ${SCDOC_DIR}/SCDoc.cpp
    ${SCDOC_DIR}/SCDoc.h
    ${SCDOC_DIR}/SCDoc.tab.cpp
    ${SCDOC_DIR}/lex.scdoc.cpp
    ${SCDOC_DIR}/SCDocPrim.cpp
    ${SCDOC_DIR}/SCDocPrim.h
)

if(CMAKE_COMPILER_IS_CLANG)
  set_source_files_properties(
    ${SCDOC_DIR}/lex.scdoc.cpp
    PROPERTIES COMPILE_FLAGS "-Wno-deprecated-register -Wno-null-conversion")
endif()

include_directories(
    ${SCDOC_DIR}
)

source_group("SCDoc\\src" FILES ${SCDOC_SRCS})
source_group("SCDoc\\include" FILES ${SCDOC_HDRS})
