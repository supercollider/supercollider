cmake_minimum_required (VERSION 2.6)
project (nova-server)

enable_testing()

# preprocessor paths
include_directories(source)
include_directories(libs
                    libs/boost
                    libs/nova-dsp
                    libs/nova-simd
                    libs/nova-tt
                    libs/oscpack
                    libs/pri_queue
                    libs/tlsf/src
                    )

if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-4096 -Wnon-virtual-dtor -Wunused -Wunused -Wstrict-overflow")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -frename-registers -funsafe-loop-optimizations -Wunsafe-loop-optimizations -fomit-frame-pointer")
endif(CMAKE_COMPILER_IS_GNUCXX)


add_subdirectory (libs)
add_subdirectory (source)
add_subdirectory (testsuite)

