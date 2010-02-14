cmake_minimum_required (VERSION 2.6)
project (supernova)

enable_testing()

option(COMPACT_BUILD "Enable all-in-one compilation.")

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

include_directories(source/sc/supercollider/common/Headers/plugin_interface
                    source/sc/supercollider/common/Headers/common
                    source/sc/supercollider/common/Headers/server)



if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-4096 -Wnon-virtual-dtor -Wunused -Wunused -Wstrict-overflow")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -frename-registers -funsafe-loop-optimizations -Wunsafe-loop-optimizations -fomit-frame-pointer")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

add_subdirectory (libs)
add_subdirectory (source)
add_subdirectory (testsuite)

