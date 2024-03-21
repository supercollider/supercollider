message(STATUS "    hidapi_parser" )
add_library( hidapi_parser STATIC hidapi_parser.c )
target_link_libraries(hidapi_parser hidapi::hidapi m)
target_include_directories(hidapi_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
