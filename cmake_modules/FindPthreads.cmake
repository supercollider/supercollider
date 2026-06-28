# - Find the Pthreads library
# This module searches for the Pthreads library (including the
# pthreads-win32 port).
#
# This module defines these variables:
#
#  PTHREADS_FOUND
#      True if the Pthreads library was found
#  PTHREADS_LIBRARY
#      The location of the Pthreads library
#  PTHREADS_INCLUDE_DIR
#      The include directory of the Pthreads library
#  PTHREADS_DEFINITIONS
#      Preprocessor definitions to define (HAVE_PTHREAD_H is a fairly common
#      one)
#
# This module responds to the PTHREADS_EXCEPTION_SCHEME
# variable on Win32 to allow the user to control the
# library linked against.  The Pthreads-win32 port
# provides the ability to link against a version of the
# library with exception handling.  IT IS NOT RECOMMENDED
# THAT YOU CHANGE PTHREADS_EXCEPTION_SCHEME TO ANYTHING OTHER THAN
# "C" because most POSIX thread implementations do not support stack
# unwinding.
#
#  PTHREADS_EXCEPTION_SCHEME
#      C  = no exceptions (default)
#         (NOTE: This is the default scheme on most POSIX thread
#          implementations and what you should probably be using)
#      CE = C++ Exception Handling
#      SE = Structure Exception Handling (MSVC only)
#

#
# Define a default exception scheme to link against
# and validate user choice.
#

if(PTHREADS_INCLUDE_DIR AND PTHREADS_LIBRARY)
    set(PTHREADS_DEFINITIONS -DHAVE_PTHREAD_H)
    set(PTHREADS_INCLUDE_DIRS ${PTHREADS_INCLUDE_DIR})
    set(PTHREADS_LIBRARIES ${PTHREADS_LIBRARY})
    set(FOUND_PTHREADS TRUE)
endif()

if(NOT DEFINED PTHREADS_EXCEPTION_SCHEME)
    # Assign default if needed
    set(PTHREADS_EXCEPTION_SCHEME "C")
else(NOT DEFINED PTHREADS_EXCEPTION_SCHEME)
    # Validate
    if(
        NOT PTHREADS_EXCEPTION_SCHEME STREQUAL "C"
        AND NOT PTHREADS_EXCEPTION_SCHEME STREQUAL "CE"
        AND NOT PTHREADS_EXCEPTION_SCHEME STREQUAL "SE"
    )
        message(
            FATAL_ERROR
            "See documentation for FindPthreads.cmake, only C, CE, and SE modes are allowed"
        )
    endif(
        NOT PTHREADS_EXCEPTION_SCHEME STREQUAL "C"
        AND NOT PTHREADS_EXCEPTION_SCHEME STREQUAL "CE"
        AND NOT PTHREADS_EXCEPTION_SCHEME STREQUAL "SE"
    )

    if(NOT MSVC AND PTHREADS_EXCEPTION_SCHEME STREQUAL "SE")
        message(
            FATAL_ERROR
            "Structured Exception Handling is only allowed for MSVC"
        )
    endif(NOT MSVC AND PTHREADS_EXCEPTION_SCHEME STREQUAL "SE")
endif(NOT DEFINED PTHREADS_EXCEPTION_SCHEME)

#
# Find the header file
#
find_path(PTHREADS_INCLUDE_DIR pthread.h)

#
# Find the library
#
set(names)
if(MSVC)
    set(names pthreadV${PTHREADS_EXCEPTION_SCHEME}2 pthread)
elseif(MINGW)
    set(names pthreadG${PTHREADS_EXCEPTION_SCHEME}2 pthread)
else(MSVC) # Unix / Cygwin / Apple / Etc.
    set(names pthread)
endif(MSVC)

find_library(PTHREADS_LIBRARY NAMES ${names} DOC "The Portable Threads Library")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Pthreads
    DEFAULT_MSG
    PTHREADS_LIBRARY
    PTHREADS_INCLUDE_DIR
)

if(PTHREADS_INCLUDE_DIR AND PTHREADS_LIBRARY)
    set(PTHREADS_DEFINITIONS -DHAVE_PTHREAD_H)
    set(PTHREADS_INCLUDE_DIRS ${PTHREADS_INCLUDE_DIR})
    set(PTHREADS_LIBRARIES ${PTHREADS_LIBRARY})
endif(PTHREADS_INCLUDE_DIR AND PTHREADS_LIBRARY)

mark_as_advanced(PTHREADS_INCLUDE_DIR)
mark_as_advanced(PTHREADS_LIBRARY)
