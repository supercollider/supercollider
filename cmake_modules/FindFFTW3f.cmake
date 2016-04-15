# Find single-precision (float) version of FFTW3

FIND_PATH(
    FFTW3F_INCLUDE_DIR
    NAMES fftw3.h
    HINTS $ENV{FFTW3_DIR}/include
      "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/fftw"
      "$ENV{ProgramW6432}/fftw"
      "$ENV{ProgramFiles}/fftw"
    PATHS /usr/local/include
          /usr/include
)
FIND_LIBRARY(
    FFTW3F_LIBRARY
    NAMES fftw3f fftw3f-3 libfftw3f libfftw3f-3
    HINTS $ENV{FFTW3_DIR}/lib
      "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/fftw"
      "$ENV{ProgramW6432}/fftw"
      "$ENV{ProgramFiles}/fftw"
    PATHS /usr/local/lib
          /usr/lib
)
# used by Windows only
FIND_PATH(
  FFTW3F_LIBRARY_DIR
  NAMES libfftw3f-3.dll libfftw3f.dll fftw3f-3.dll fftw3f.dll
  HINTS "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/fftw"
    "$ENV{ProgramW6432}/fftw"
    "$ENV{ProgramFiles}/fftw"
)

SET(FFTW3F_FOUND "NO")

IF( FFTW3F_INCLUDE_DIR AND FFTW3F_LIBRARY )
    SET(FFTW3F_FOUND "YES")
ENDIF()
