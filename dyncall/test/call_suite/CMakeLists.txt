add_executable(call_suite globals.c invoke.c cases.c main.c)
target_link_libraries(call_suite dyncall_s)


