message(STATUS "    hidapi2osc" )

include(FindPkgConfig)
# pkg_check_modules
pkg_check_modules(liblo REQUIRED liblo)

find_package(hidapi_parser REQUIRED)

link_directories(
   ${liblo_LIBRARY_DIRS}
)


##########################################################################
# hidapi2osc
##########################################################################

add_executable( hidapi2osc hidapi2osc.cpp)

target_link_libraries(hidapi2osc ${liblo_LIBRARIES} ${EXTRA_LIBS} hidapi hidapi_parser m)

install(TARGETS hidapi2osc DESTINATION bin)
