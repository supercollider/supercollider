#----------------------------------------------------------------------------
# This file was automatically generated from the original CMakeLists.txt file
# Add a variable to hold the headers for the library
set (lib_headers
    unordered_map.hpp
    unordered_set.hpp
    unordered
)

# Add a library target to the build system
boost_library_project(
  unordered
  #  SRCDIRS 
  #  TESTDIRS 
  HEADERS ${lib_headers}
  #  DOCDIRS 
  #  DESCRIPTION 
  MODULARIZED 
  #  AUTHORS 
  #  MAINTAINERS 
)


