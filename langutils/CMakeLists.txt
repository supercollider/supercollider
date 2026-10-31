# sclang.wasm links with shared memory, so every object needs the atomics feature -pthread enables.
if(EMSCRIPTEN)
    add_compile_options(-pthread)
endif()

add_subdirectory(sc_lexer)
add_subdirectory(sc_util)
