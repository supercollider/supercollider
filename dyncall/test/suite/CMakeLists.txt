add_executable(suite main.c case.c)
if(WIN32)
else()
list(APPEND AUX_LIBS m)
endif()
target_link_libraries(suite dyncall_s ${AUX_LIBS})


