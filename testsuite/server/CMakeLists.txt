add_library(boost_test STATIC boost_test.cpp)
target_link_libraries(boost_test PUBLIC Boost::unit_test_framework)

add_subdirectory(scsynth)
if (SUPERNOVA)
  add_subdirectory(supernova)
endif()

add_subdirectory(plugins)
