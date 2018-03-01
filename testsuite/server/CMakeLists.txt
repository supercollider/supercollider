add_subdirectory(scsynth)

if (SUPERNOVA)
  add_subdirectory(supernova)
endif()

add_library(boost_test STATIC boost_test.cpp)
target_include_directories(boost_test PUBLIC ${CMAKE_SOURCE_DIR}/external_libraries/boost)
