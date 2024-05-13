add_library(boost_test STATIC boost_test.cpp)
target_include_directories(boost_test PUBLIC ${boost_include_dirs})

add_subdirectory(scsynth)
if (SUPERNOVA)
  add_subdirectory(supernova)
endif()

