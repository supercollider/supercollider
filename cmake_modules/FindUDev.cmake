# - try to find the udev library
#
# Cache Variables: (probably not for direct use in your scripts)
#  UDEV_INCLUDE_DIR
#  UDEV_SOURCE_DIR
#  UDEV_LIBRARY
#
# Non-cache variables you might use in your CMakeLists.txt:
#  UDEV_FOUND
#  UDEV_INCLUDE_DIRS
#  UDEV_LIBRARIES
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Original Author:
# 2014 Kevin M. Godby <kevin@godby.org>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

#set(UDEV_ROOT_DIR
#    "${UDEV_ROOT_DIR}"
#	CACHE
#	PATH
#    "Directory to search for udev")
#
#find_package(PkgConfig QUIET)
#if(PKG_CONFIG_FOUND)
#	pkg_check_modules(PC_LIBUDEV libudev)
#endif()
#
#find_library(UDEV_LIBRARY
#	NAMES
#	udev
#	PATHS
#	${PC_LIBUDEV_LIBRARY_DIRS}
#	${PC_LIBUDEV_LIBDIR}
#	HINTS
#	"${UDEV_ROOT_DIR}"
#	PATH_SUFFIXES
#	lib
#	)
#
#get_filename_component(_libdir "${UDEV_LIBRARY}" PATH)
#
#find_path(UDEV_INCLUDE_DIR
#	NAMES
#	libudev.h
#	PATHS
#	${PC_LIBUDEV_INCLUDE_DIRS}
#	${PC_LIBUDEV_INCLUDEDIR}
#	HINTS
#	"${_libdir}"
#	"${_libdir}/.."
#	"${UDEV_ROOT_DIR}"
#	PATH_SUFFIXES
#	include
#	)
#
#include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(UDEV
#	DEFAULT_MSG
#	UDEV_LIBRARY
#	UDEV_INCLUDE_DIR
#	)
#
#if(UDEV_FOUND)
#	list(APPEND UDEV_LIBRARIES ${UDEV_LIBRARY})
#	list(APPEND UDEV_INCLUDE_DIRS ${UDEV_INCLUDE_DIR})
#	mark_as_advanced(UDEV_ROOT_DIR)
#endif()
#
#mark_as_advanced(UDEV_INCLUDE_DIR
#	UDEV_LIBRARY)
#




FIND_PATH(
    UDEV_INCLUDE_DIR
    libudev.h
    /usr/include
    /usr/local/include
    ${UDEV_PATH_INCLUDES}
)

FIND_LIBRARY(
    UDEV_LIBRARIES
    NAMES udev libudev
    PATHS
        /usr/lib${LIB_SUFFIX}
        /usr/local/lib${LIB_SUFFIX}
        ${UDEV_PATH_LIB}
)

IF (UDEV_LIBRARIES AND UDEV_INCLUDE_DIR)
    SET(UDEV_FOUND "YES")
    execute_process(COMMAND pkg-config --atleast-version=143 libudev RESULT_VARIABLE UDEV_STABLE)
    # retvale is 0 of the condition is "true" so we need to negate the value...
    if (UDEV_STABLE)
set(UDEV_STABLE 0)
    else (UDEV_STABLE)
set(UDEV_STABLE 1)
    endif (UDEV_STABLE)
    message(STATUS "libudev stable: ${UDEV_STABLE}")
ENDIF (UDEV_LIBRARIES AND UDEV_INCLUDE_DIR)

IF (UDEV_FOUND)
    MESSAGE(STATUS "Found UDev: ${UDEV_LIBRARIES}")
    MESSAGE(STATUS " include: ${UDEV_INCLUDE_DIR}")
ELSE (UDEV_FOUND)
    MESSAGE(STATUS "UDev not found.")
    MESSAGE(STATUS "UDev: You can specify includes: -DUDEV_PATH_INCLUDES=/opt/udev/include")
    MESSAGE(STATUS " currently found includes: ${UDEV_INCLUDE_DIR}")
    MESSAGE(STATUS "UDev: You can specify libs: -DUDEV_PATH_LIB=/opt/udev/lib")
    MESSAGE(STATUS " currently found libs: ${UDEV_LIBRARIES}")
    IF (UDev_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find UDev library")
    ENDIF (UDev_FIND_REQUIRED)
ENDIF (UDEV_FOUND)

