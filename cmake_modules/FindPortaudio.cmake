# - Try to find Portaudio
# Once done this will define
#
#  PORTAUDIO_FOUND - system has Portaudio
#  PORTAUDIO_INCLUDE_DIRS - the Portaudio include directory
#  PORTAUDIO_LIBRARIES - Link these to use Portaudio
#  PORTAUDIO_DEFINITIONS - Compiler switches required for using Portaudio
#  PORTAUDIO_VERSION - Portaudio version
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  # in cache already
  set(PORTAUDIO_FOUND TRUE)
else (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  if (NOT WIN32 OR MSYS)
   include(FindPkgConfig)
   pkg_check_modules(PORTAUDIO2 portaudio-2.0)
  endif (NOT WIN32 OR MSYS)

  if (PORTAUDIO2_FOUND)
    set(PORTAUDIO_INCLUDE_DIRS
      ${PORTAUDIO2_INCLUDE_DIRS}
    )
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      set(PORTAUDIO_LIBRARIES "${PORTAUDIO2_LIBRARY_DIRS}/lib${PORTAUDIO2_LIBRARIES}.dylib")
    else (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR MSYS)
      set(PORTAUDIO_LIBRARIES
        ${PORTAUDIO2_LIBRARIES}
      )
    endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR MSYS)
    set(PORTAUDIO_VERSION
      19
    )
    set(PORTAUDIO_FOUND TRUE)
  else (PORTAUDIO2_FOUND)
    find_path(PORTAUDIO_INCLUDE_DIR
      NAMES
        portaudio.h
      PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        "/${MINGW_ARCH}/include"
        "$ENV{WD}/../../${MINGW_ARCH}/include"
        "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/portaudio/include"
    )

    if(NOT MSVC)
        find_library(PORTAUDIO_LIBRARY
          NAMES
            portaudio libportaudio "libportaudio_${CMAKE_LIBRARY_ARCHITECTURE}" "libportaudio_static_${CMAKE_LIBRARY_ARCHITECTURE}"
          PATHS
            /usr/lib
            /usr/local/lib
            /opt/local/lib
            /sw/lib
            "/${MINGW_ARCH}/lib"
            "$ENV{WD}/../../${MINGW_ARCH}/lib"
            "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/portaudio"
        )

        find_path(PORTAUDIO_LIBRARY_DIR
          NAMES
            portaudio.dll libportaudio.dll "libportaudio_${CMAKE_LIBRARY_ARCHITECTURE}.dll"
          PATHS
            "/${MINGW_ARCH}/bin"
            "$ENV{WD}/../../${MINGW_ARCH}/bin"
            "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/portaudio"
        )

    else(NOT MSVC)
        find_library(PORTAUDIO_LIBRARY_DEBUG
          NAMES
            "portaudio_${CMAKE_LIBRARY_ARCHITECTURE}" "portaudio_static_${CMAKE_LIBRARY_ARCHITECTURE}"
          PATHS
            "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/portaudio/Debug"
        )

        find_library(PORTAUDIO_LIBRARY_RELEASE
          NAMES
            "portaudio_${CMAKE_LIBRARY_ARCHITECTURE}" "portaudio_static_${CMAKE_LIBRARY_ARCHITECTURE}"
          PATHS
            "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/portaudio/Release"
        )

        find_path(PORTAUDIO_LIBRARY_DIR_DEBUG
          NAMES
            "portaudio_${CMAKE_LIBRARY_ARCHITECTURE}.dll"
          PATHS
            "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/portaudio/Debug"
        )

        find_path(PORTAUDIO_LIBRARY_DIR_RELEASE
          NAMES
            "portaudio_${CMAKE_LIBRARY_ARCHITECTURE}.dll"
          PATHS
            "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/portaudio/Release"
        )
    endif(NOT MSVC)

    set(PORTAUDIO_INCLUDE_DIRS
      ${PORTAUDIO_INCLUDE_DIR}
    )
    set(PORTAUDIO_LIBRARIES
      ${PORTAUDIO_LIBRARY}
    )

    set(PORTAUDIO_LIBRARY_DIRS
      ${PORTAUDIO_LIBRARY_DIR}
    )
    if (WIN32)
      set(PORTAUDIO_VERSION 19)
    else (WIN32)
      set(PORTAUDIO_VERSION 18)
    endif (WIN32)

    if (PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)
      set(PORTAUDIO_FOUND TRUE)
    endif ()

    if (PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARY_DEBUG AND PORTAUDIO_LIBRARY_RELEASE)
      set(PORTAUDIO_FOUND_RELEASE_DEBUG TRUE)
    endif()

    if (PORTAUDIO_FOUND)
      if (NOT Portaudio_FIND_QUIETLY)
        message(STATUS "Found Portaudio: ${PORTAUDIO_LIBRARIES}")
      endif (NOT Portaudio_FIND_QUIETLY)
    else (PORTAUDIO_FOUND)
      if (Portaudio_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find Portaudio")
      endif (Portaudio_FIND_REQUIRED)
    endif (PORTAUDIO_FOUND)
  endif (PORTAUDIO2_FOUND)


  # show the PORTAUDIO_INCLUDE_DIRS and PORTAUDIO_LIBRARIES variables only in the advanced view
  mark_as_advanced(PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)

endif (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
