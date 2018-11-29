add_subdirectory(scsynth)

if (SUPERNOVA)
  add_subdirectory(supernova)
endif()

add_library(boost_test STATIC boost_test.cpp)
if (Boost_FOUND)
  target_include_directories(boost_test PUBLIC ${Boost_INCLUDE_DIRS})
else()
  target_include_directories(boost_test PUBLIC ${CMAKE_SOURCE_DIR}/external_libraries/boost)
endif()
