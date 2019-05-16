# Custom targets for formatting commands
get_filename_component(SC_FORMAT_SCRIPT_PATH clang-format.py ABSOLUTE)
add_custom_target(lint "${SC_FORMAT_SCRIPT_PATH}" lint)
add_custom_target(format "${SC_FORMAT_SCRIPT_PATH}" format)
add_custom_target(lintall "${SC_FORMAT_SCRIPT_PATH}" lintall)
add_custom_target(formatall "${SC_FORMAT_SCRIPT_PATH}" formatall)

# Convenience scripts in build dir
set(SC_LINT_FORMAT_HELPER_SCRIPT_COMMAND lint)
configure_file(lint_or_format.py.in "${CMAKE_BINARY_DIR}/lint.py")
set(SC_LINT_FORMAT_HELPER_SCRIPT_COMMAND format)
configure_file(lint_or_format.py.in "${CMAKE_BINARY_DIR}/format.py")
