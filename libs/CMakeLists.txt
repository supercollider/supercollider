#boost
aux_source_directory(boost/libs/thread/src/ boost_thread )
aux_source_directory(boost/libs/thread/src/pthread/ boost_thread_posix )

add_library(boost_thread STATIC ${boost_thread} ${boost_thread_posix})

aux_source_directory(boost/libs/program_options/src/ boost_program_options )
add_library(boost_program_options STATIC ${boost_program_options})

aux_source_directory(boost/libs/system/src/ boost_system )
add_library(boost_system STATIC ${boost_system})


#oscpack
add_library(oscpack STATIC "oscpack_build.cpp")

#tlsf
add_library(tlsf STATIC "tlsf/src/tlsf.c")
set_target_properties(tlsf
                      PROPERTIES COMPILE_DEFINITIONS TLSF_STATISTIC=1)
