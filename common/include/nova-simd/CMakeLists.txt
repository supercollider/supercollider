cmake_minimum_required (VERSION 2.6)
project (nova-simd)

enable_testing()

if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-1000")
endif()

add_subdirectory (benchmarks)
add_subdirectory (testsuite)

