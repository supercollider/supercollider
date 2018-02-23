add_executable(nm nm.c)
target_link_libraries(nm dynload_s ${CMAKE_DL_LIBS})

