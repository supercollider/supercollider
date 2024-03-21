message(STATUS "    hidparsertest" )

include_directories(
  ${CMAKE_BINARY_DIR}
)

add_executable( hidparsertest hidparsertest.c )

target_link_libraries(hidparsertest hidapi::hidapi hidapi_parser ${EXTRA_LIBS} m)

install(TARGETS hidparsertest DESTINATION bin)
