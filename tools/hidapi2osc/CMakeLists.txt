message(STATUS "    hidapi2osc" )

include(FindPkgConfig)
# pkg_check_modules
pkg_check_modules(liblo REQUIRED liblo)

# add_subdirectory( ${hidapi_source} )

# message( "osc: hidapi include dirs are: ${hidapi_INCLUDE_DIRS}" )
# message( "osc: hidapi parser include dirs are: ${hidapi_parser_INCLUDE_DIRS}" )

include_directories(
  ${CMAKE_BINARY_DIR}
  ${liblo_INCLUDE_DIRS}
  ${hidapi_SOURCE_DIR}/hidapi/
  ${hidapi_SOURCE_DIR}/hidapi_parser/
)

link_directories(
   ${liblo_LIBRARY_DIRS}
)


##########################################################################
# sdl2osc
##########################################################################

set(hidapi2osc_SRCS
  hidapi2osc.cpp
)

add_executable( hidapi2osc ${hidapi2osc_SRCS} )

target_link_libraries(hidapi2osc ${liblo_LIBRARIES} ${EXTRA_LIBS} hidapi hidapi_parser m)

install(TARGETS hidapi2osc DESTINATION bin)
