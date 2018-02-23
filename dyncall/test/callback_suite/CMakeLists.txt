add_executable(callback_suite env.c handler.c sigstrings.c invokers.c do_test.c main.c print.c signature_utils.c)
target_link_libraries(callback_suite dyncallback_s)

