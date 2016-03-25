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
  set(SNDFILE_FOUND False)
  set(SNDFILE_INCLUDE_DIR "nowhere")  # for onceonly check above
  set(SNDFILE_LIBRARIES "")
  add_definitions("-DNO_LIBSNDFILE")
elseif (SNDFILE_INCLUDE_DIR AND SNDFILE_LIBRARY)
  set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
  set(SNDFILE_FOUND TRUE)

elseif (APPLE)

  find_path(SNDFILE_INCLUDE_DIR sndfile.h
    HINTS /usr/local/opt/libsndfile/include
  )

  find_library(SNDFILE_LIBRARY NAMES libsndfile.dylib
    HINTS /usr/local/opt/libsndfile/lib
  )

  if(NOT SNDFILE_INCLUDE_DIR OR NOT SNDFILE_LIBRARY)
    set(SNDFILE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../external_libraries/libsndfile/)
    set(SNDFILE_LIBRARIES ${CMAKE_CURRENT_LIST_DIR}/../platform/mac/lib/scUBlibsndfile.a)
    set(SNDFILE_FOUND TRUE)
    add_definitions("-isystem ${CMAKE_CURRENT_LIST_DIR}/../external_libraries/libsndfile")
    message(STATUS "Could not find homebrew install of libsndfile, using old bundled version instead")
  else()
    set(SNDFILE_FOUND TRUE)
    message(STATUS "Found homebrew install of libsndfile")
    set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
  endif()

  # TODO on non-apple platforms, we need to be able to test for >=1018.
  # (On apple it is known true, because we bundle a later version.)
  # I think this is not necessary anymore in 2016

  add_definitions("-DLIBSNDFILE_1018")

else()
  find_path(SNDFILE_INCLUDE_DIR sndfile.h
    HINTS
      "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/libsndfile/include"
      "$ENV{ProgramW6432}/Mega-Nerd/libsndfile/include"
      "$ENV{ProgramFiles}/Mega-Nerd/libsndfile/include"
    PATHS /usr/local/include
      /usr/include
  )

  find_library(SNDFILE_LIBRARY
    NAMES sndfile sndfile-1 libsndfile libsndfile-1
    HINTS
      "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/libsndfile/lib"
      "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/libsndfile/bin"
      "$ENV{ProgramW6432}/Mega-Nerd/libsndfile/lib"
      "$ENV{ProgramW6432}/Mega-Nerd/libsndfile/bin"
      "$ENV{ProgramFiles}/Mega-Nerd/libsndfile/lib"
      "$ENV{ProgramFiles}/Mega-Nerd/libsndfile/bin"
    PATHS /usr/local/
      /usr/lib
  )
  # used by Windows only
  find_path(SNDFILE_LIBRARY_DIR
    NAMES libsndfile.dll libsndfile-1.dll
    HINTS
      "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/libsndfile/lib"
      "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/libsndfile/bin"
      "$ENV{ProgramW6432}/Mega-Nerd/libsndfile/lib"
      "$ENV{ProgramW6432}/Mega-Nerd/libsndfile/bin"
      "$ENV{ProgramFiles}/Mega-Nerd/libsndfile/lib"
      "$ENV{ProgramFiles}/Mega-Nerd/libsndfile/bin"
  )

  # Handle the QUIETLY and REQUIRED arguments and set SNDFILE_FOUND to TRUE if
  # all listed variables are TRUE.
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Sndfile DEFAULT_MSG
    SNDFILE_LIBRARY SNDFILE_INCLUDE_DIR)

  if(SNDFILE_FOUND)
    set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
  else(SNDFILE_FOUND)
    set(SNDFILE_LIBRARIES)
  endif(SNDFILE_FOUND)

endif()
mark_as_advanced(SNDFILE_INCLUDE_DIR SNDFILE_LIBRARY)
