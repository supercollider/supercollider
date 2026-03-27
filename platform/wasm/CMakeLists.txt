# other emscripten definitions are in root, scsynth and sclang cmake
# this only defines the editor build

add_custom_target(webeditor
    DEPENDS sclang scsynth
)

set(EDITOR_WASM_OUTPUT ${WASM_DIST_DIR}/editor)

set_target_properties(webeditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTOR ${EDITOR_WASM_OUTPUT}
)

set(EDITOR_WASM_FILES
    ${WASM_ASSET_DIR}/editor/index.html
    ${WASM_ASSET_DIR}/editor/init.js
    ${WASM_ASSET_DIR}/LICENSE
    ${WASM_ASSET_DIR}/dev_server.py
    ${CMAKE_SOURCE_DIR}/README_WASM.md
    $<TARGET_FILE_DIR:scsynth>/scsynth.js
    $<TARGET_FILE_DIR:scsynth>/scsynth.wasm
    $<TARGET_FILE_DIR:sclang>/sclang.js
    $<TARGET_FILE_DIR:sclang>/sclang.wasm
    $<TARGET_FILE_DIR:sclang>/sclang.data
)
add_custom_command(TARGET webeditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${EDITOR_WASM_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${EDITOR_WASM_FILES} ${EDITOR_WASM_OUTPUT}
)
