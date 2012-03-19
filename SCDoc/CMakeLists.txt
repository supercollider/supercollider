set(SCDOC_DIR ${CMAKE_SOURCE_DIR}/SCDoc)

set(SCDOC_SRCS
    ${SCDOC_DIR}/SCDoc.cpp
    ${SCDOC_DIR}/SCDoc.tab.cpp
    ${SCDOC_DIR}/lex.scdoc.cpp
    ${SCDOC_DIR}/SCDocPrim.cpp
)

include_directories(
    ${SCDOC_DIR}
)

