message(STATUS "    hidapi_parser" )


# message( "hidapi parser source dir is: ${hidapi_SOURCE_DIR}" )
#
# set( hidapi_parser_INCLUDE_DIRS ${hidapi_SOURCE_DIR}/hidapi_parser/)
# # set( hidapi_LIBS ${UDEV_LIBRARIES})
# set( hidapi_parser_SRCS hidapi_parser.c )
#
# message( "hidapi_parser include dirs are: ${hidapi_parser_INCLUDE_DIRS}" )

include_directories( ${hidapi_SOURCE_DIR}/hidapi/ )
add_library( hidapi_parser STATIC hidapi_parser.c )
target_link_libraries( hidapi )
