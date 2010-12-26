#boost
aux_source_directory(boost/libs/thread/src/ boost_thread )

include_directories(boost)

if (PTHREADS_FOUND)
    aux_source_directory(boost/libs/thread/src/pthread/ boost_thread_posix )
    add_library(boost_thread STATIC ${boost_thread} ${boost_thread_posix})
    target_link_libraries(boost_thread ${PTHREADS_LIBRARY})

    include_directories(${PTHREADS_INCLUDE_DIR})
else()
    message(SEND_ERROR "we need to implement boost_thread for non-posix systems")
endif()



aux_source_directory(boost/libs/program_options/src/ boost_program_options )
add_library(boost_program_options STATIC ${boost_program_options})

aux_source_directory(boost/libs/system/src/ boost_system )
add_library(boost_system STATIC ${boost_system})

aux_source_directory(boost/libs/filesystem/v2/src boost_filesystem)
add_library(boost_filesystem STATIC ${boost_filesystem})


#oscpack
add_library(oscpack STATIC "oscpack_build.cpp")

#tlsf
add_library(tlsf STATIC "TLSF-2.4.6/src/tlsf.c")
set_target_properties(tlsf
                      PROPERTIES COMPILE_DEFINITIONS TLSF_STATISTIC=1)

if(LTO)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
endif()

