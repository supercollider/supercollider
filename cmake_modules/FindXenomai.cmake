# - Try to find xenomai
# Once done this will define
#
#  XENOMAI_FOUND - system has xenomai
#  XENOMAI_INCLUDE_DIRS - the xenomai include directory
#  XENOMAI_LIBRARIES - Link these to use xenomai
#  XENOMAI_DEFINITIONS - Compiler switches required for using xenomai
#
#  Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#  Modified for other libraries by Lasse Kärkkäinen <tronic>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (XENOMAI_LIBRARIES AND XENOMAI_INCLUDE_DIRS)
  # in cache already
  set(XENOMAI_FOUND TRUE)
else (XENOMAI_LIBRARIES AND XENOMAI_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  if (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    include(UsePkgConfig)
    pkgconfig(xenomai _XENOMAI_INCLUDEDIR _XENOMAI_LIBDIR _XENOMAI_LDFLAGS _XENOMAI_CFLAGS)
  else (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
      pkg_check_modules(_XENOMAI xenomai)
    endif (PKG_CONFIG_FOUND)
  endif (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
  find_path(XENOMAI_INCLUDE_DIR
    NAMES
      xeno_config.h
    PATHS
      ${_XENOMAI_INCLUDEDIR}
      /usr/xenomai/include
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )
  
  find_library(XENOMAI_LIBRARY
    NAMES
      xenomai
    PATHS
      ${_XENOMAI_LIBDIR}
      /usr/xenomai/lib
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (XENOMAI_LIBRARY)
    set(XENOMAI_FOUND TRUE)
    SET(XENOMAI_DEFINITIONS "${XENOMAI_DEFINITIONS} -lnative -lxenomai -lrt" )
  endif (XENOMAI_LIBRARY)

  set(XENOMAI_INCLUDE_DIRS
    ${XENOMAI_INCLUDE_DIR}
  )

  if (XENOMAI_FOUND)
    set(XENOMAI_LIBRARIES
      ${XENOMAI_LIBRARIES}
      ${XENOMAI_LIBRARY}
    )
  endif (XENOMAI_FOUND)

  if (XENOMAI_INCLUDE_DIRS AND XENOMAI_LIBRARIES)
     set(XENOMAI_FOUND TRUE)
  endif (XENOMAI_INCLUDE_DIRS AND XENOMAI_LIBRARIES)

  if (XENOMAI_FOUND)
    if (NOT XENOMAI_FIND_QUIETLY)
      message(STATUS "Found xenomai: ${XENOMAI_LIBRARY}")
    endif (NOT XENOMAI_FIND_QUIETLY)
  else (XENOMAI_FOUND)
    if (XENOMAI_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find XENOMAI")
    endif (XENOMAI_FIND_REQUIRED)
  endif (XENOMAI_FOUND)

  # show the XENOMAI_INCLUDE_DIRS and XENOMAI_LIBRARIES variables only in the advanced view
  mark_as_advanced(XENOMAI_INCLUDE_DIRS XENOMAI_LIBRARIES)

endif (XENOMAI_LIBRARIES AND XENOMAI_INCLUDE_DIRS)

