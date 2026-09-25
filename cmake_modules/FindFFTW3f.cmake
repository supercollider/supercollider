# Find single-precision (float) version of FFTW3

set(FFTW_HINT_PATHS_WIN "")
if(WIN32)
  file(GLOB FFTW_HINT_PATHS_WIN 
    "$ENV{ProgramFiles}/fftw*"
    "$ENV{ProgramFiles\(x86\)}/fftw*"
    "$ENV{ProgramW6432}/fftw*"
    "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/fftw*"
    "${CMAKE_SOURCE_DIR}/../fftw*"
  )
endif()

if(MINGW) # enable linking to dlls directly on mingw
    list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
endif()

find_path(
    FFTW3F_INCLUDE_DIR
    NAMES fftw3.h
    HINTS
      "$ENV{FFTW3_DIR}"
      ${FFTW_HINT_PATHS_WIN}
    PATH_SUFFIXES include
    PATHS /usr/local/include
          /usr/include
)
find_library(
    FFTW3F_LIBRARY
    NAMES fftw3f fftw3f-3 libfftw3f libfftw3f-3
    HINTS
      "$ENV{FFTW3_DIR}"
      ${FFTW_HINT_PATHS_WIN}
    PATH_SUFFIXES lib
    PATHS /usr/local/lib
          /usr/lib
)
# used by Windows only
if(WIN32)
  find_path(
    FFTW3F_LIBRARY_DIR
    NAMES libfftw3f-3.dll libfftw3f.dll fftw3f-3.dll fftw3f.dll
    HINTS
      "$ENV{FFTW3_DIR}"
      ${FFTW_HINT_PATHS_WIN}
    PATH_SUFFIXES bin
  )
endif()

SET(FFTW3F_FOUND "NO")

IF( FFTW3F_INCLUDE_DIR AND FFTW3F_LIBRARY )
    SET(FFTW3F_FOUND "YES")
ENDIF()
