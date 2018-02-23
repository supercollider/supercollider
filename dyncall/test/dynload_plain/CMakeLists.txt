add_executable(dynload_plain dynload_plain.c)
target_link_libraries(dynload_plain dynload_s ${CMAKE_DL_LIBS})
if(${UNIX})
exec_program("((ls /lib*/libc.so* || ls /usr/lib/libc.so*) | (sort -V -r || sort -t . -n -k 2)) 2>/dev/null | head -1" OUTPUT_VARIABLE DEF_C_DYLIB)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEF_C_DYLIB=\\\"${DEF_C_DYLIB}\\\"")
endif()

