# we put portaudio_submodule in a subdirectory in order to disable flags and definitions
unset(CMAKE_C_FLAGS)
unset(CMAKE_CXX_FLAGS)
if(WIN32)
    remove_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
endif()

set(PA_BUILD_STATIC ON CACHE BOOL "PortAudio static library" FORCE)
set(PA_BUILD_SHARED OFF CACHE BOOL "PortAudio shared library" FORCE)

# disable platform suffix
set(PA_LIBNAME_ADD_SUFFIX OFF CACHE BOOL "PortAudio static library suffix" FORCE)
# disable install target
set(PA_DISABLE_INSTALL ON CACHE BOOL "Disable PortAudio install" FORCE)

# disable obsolete APIs
if(WIN32)
    set(PA_USE_DS OFF CACHE BOOL "Disable support for DirectSound" FORCE)
    set(PA_USE_WMME OFF CACHE BOOL "Disable support for MME" FORCE)
    set(PA_USE_WDMKS OFF CACHE BOOL "Disable support for WDMKS" FORCE)
endif()

# bump up cmake version to fix compatibility with cmake 4
# FIXME: remove this after the portaudio submodule is updated above v19.7.0
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)

add_subdirectory(portaudio_submodule)

if(WIN32 AND NOT PA_USE_ASIO)
    message(STATUS "Building PortAudio WITHOUT support for ASIO")
endif()

# when building PA ourselves, we need to specify PORTAUDIO_INCLUDE_DIRS and PORTAUDIO_LIBRARIES
set(PORTAUDIO_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/portaudio_submodule/include" CACHE PATH "Portaudio include directory" FORCE)
set(PORTAUDIO_LIBRARIES portaudio_static CACHE PATH "PORTAUDIO_LIBRARIES" FORCE)
