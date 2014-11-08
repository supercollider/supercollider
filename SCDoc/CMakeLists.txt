set(SCDOC_DIR ${CMAKE_SOURCE_DIR}/SCDoc)

set(SCDOC_SRCS
    ${SCDOC_DIR}/SCDoc.cpp
    ${SCDOC_DIR}/SCDoc.tab.cpp
    ${SCDOC_DIR}/lex.scdoc.cpp
    ${SCDOC_DIR}/SCDocPrim.cpp
)

set(SCDOC_HDRS
    ${SCDOC_DIR}/SCDoc.h
    ${SCDOC_DIR}/SCDoc.tab.hpp
    ${SCDOC_DIR}/SCDocPrim.h
)

include_directories(
    ${SCDOC_DIR}
)

source_group("SCDoc\\src" FILES ${SCDOC_SRCS})
source_group("SCDoc\\include" FILES ${SCDOC_HDRS})
