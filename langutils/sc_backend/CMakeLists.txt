include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

# Lowers sc_sema's ir to bytecode for the VM in lang/, as plain data, so it doesn't depend on the VM.
add_library(sc_backend STATIC
	src/frames.cpp
	src/lower.cpp
	src/members.cpp
)

target_include_directories(sc_backend PUBLIC include)
target_link_libraries(sc_backend PUBLIC sc_sema sc_diagnostic sc_bytecode sc_util PRIVATE sc_parser sc_lexer)
set_property(TARGET sc_backend PROPERTY FOLDER Libs)
