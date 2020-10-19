# we put portaudio_library in a subdirectory in order to disable flags and definitions
unset(CMAKE_C_FLAGS)
unset(CMAKE_CXX_FLAGS)
if(WIN32)
    remove_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

set(PA_BUILD_STATIC ON CACHE BOOL "PortAudio static library" FORCE)
set(PA_BUILD_SHARED OFF CACHE BOOL "PortAudio shared library" FORCE)

# disable platform suffix
set(PA_LIBNAME_ADD_SUFFIX OFF CACHE BOOL "PortAudio static library suffix" FORCE)
# disable install target
set(PA_DISABLE_INSTALL ON CACHE BOOL "Disable PortAudio install" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/portaudio_library")

if(WIN32)
    message(STATUS "PortAudio support for ASIO: ${PA_USE_ASIO}")
endif()

# when building PA ourselves, we need to specify PORTAUDIO_INCLUDE_DIRS and PORTAUDIO_LIBRARIES
set(PORTAUDIO_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/portaudio_library/include" CACHE PATH "Portaudio include directory" FORCE)
set(PORTAUDIO_LIBRARIES portaudio_static CACHE PATH "PORTAUDIO_LIBRARIES" FORCE)
