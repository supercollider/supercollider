# - Find sndfile
# Find the native sndfile includes and libraries
#
#  SNDFILE_INCLUDE_DIR - where to find sndfile.h, etc.
#  SNDFILE_LIBRARIES   - List of libraries when using libsndfile.
#  SNDFILE_FOUND       - True if libsndfile found.

if(NO_LIBSNDFILE)
  #if(NOT SNDFILE_FIND_QUIETLY)
  #  message(STATUS "FindSndfile: sndfile deactivated (NO_LIBSNDFILE)")
  #endif()
  set(SNDFILE_FOUND FALSE)
  set(SNDFILE_INCLUDE_DIR "nowhere")  # for onceonly check above
  set(SNDFILE_LIBRARIES "")
  add_definitions("-DNO_LIBSNDFILE")
elseif (SNDFILE_INCLUDE_DIR AND SNDFILE_LIBRARY)
  set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
  set(SNDFILE_FOUND TRUE)
elseif (APPLE)

  find_path(SNDFILE_INCLUDE_DIR sndfile.h
    HINTS 
      /opt/homebrew
      /usr/local/opt/libsndfile
    PATH_SUFFIXES include
  )

  find_library(SNDFILE_LIBRARY NAMES libsndfile.dylib libsndfile.a
    HINTS 
      /opt/homebrew
      /usr/local/opt/libsndfile
    PATH_SUFFIXES lib
  )

  if(SNDFILE_INCLUDE_DIR AND SNDFILE_LIBRARY)
    set(SNDFILE_FOUND TRUE)
    set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
    message(STATUS "Found SndFile: ${SNDFILE_LIBRARY}")
  else()
    set(SNDFILE_LIBRARIES)
  endif()

else()
  set(SNDFILE_HINT_PATHS_WIN "")
  if(WIN32)
    file(GLOB SNDFILE_HINT_PATHS_WIN
      "$ENV{ProgramFiles}/libsndfile*"
      "$ENV{ProgramFiles\(x86\)}/libsndfile*"
      "$ENV{ProgramW6432}/libsndfile*"
      "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/libsndfile*"
      "${CMAKE_SOURCE_DIR}/../libsndfile*"
    )
  endif()
  find_path(SNDFILE_INCLUDE_DIR sndfile.h
    HINTS
      ${SNDFILE_HINT_PATHS_WIN}
    PATH_SUFFIXES 
      include
    PATHS 
      /usr/local/include
      /usr/include
  )

  find_library(SNDFILE_LIBRARY
    NAMES sndfile sndfile-1 libsndfile libsndfile-1
    HINTS
      ${SNDFILE_HINT_PATHS_WIN}
    PATH_SUFFIXES 
      lib
    PATHS 
      /usr/local/lib
      /usr/lib
  )
  # used by Windows only
  if(WIN32)
    find_path(SNDFILE_LIBRARY_DIR
      NAMES libsndfile.dll libsndfile-1.dll sndfile.dll
      HINTS
        ${SNDFILE_HINT_PATHS_WIN}
      PATH_SUFFIXES 
        bin
    )
  endif()

  # Handle the QUIETLY and REQUIRED arguments and set SNDFILE_FOUND to TRUE if
  # all listed variables are TRUE.
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Sndfile DEFAULT_MSG
    SNDFILE_LIBRARY SNDFILE_INCLUDE_DIR)

  if(SNDFILE_FOUND)
    set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
  else()
    set(SNDFILE_LIBRARIES)
  endif()
endif()

mark_as_advanced(SNDFILE_INCLUDE_DIR SNDFILE_LIBRARY SNDFILE_LIBRARY_DIR)

# check for format support
# note: this only check whehter libsndfile's _version_ supports the given format
# it does not check whether support for that format was actually compiled into the library

include(CMakePushCheckState)
cmake_push_check_state(RESET)

list(APPEND CMAKE_REQUIRED_INCLUDES ${SNDFILE_INCLUDE_DIR})

include(CheckCSourceCompiles)
check_c_source_compiles("
    # include <sndfile.h>
    int main() {return SF_FORMAT_VORBIS;}
  "
  SNDFILE_HAS_VORBIS
)

check_c_source_compiles("
    # include <sndfile.h>
    int main() {return SF_FORMAT_OPUS;}
  "
  SNDFILE_HAS_OPUS
)

check_c_source_compiles("
    # include <sndfile.h>
    int main() {return SF_FORMAT_MPEG;}
  "
  SNDFILE_HAS_MPEG
)

cmake_pop_check_state()
