add_library(sharedlib SHARED sharedlib.c)
target_link_libraries(sharedlib dynload_s)
