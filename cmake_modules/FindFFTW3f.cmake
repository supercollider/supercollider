# Find single-precision (float) version of FFTW3

FIND_PATH(
    FFTW3F_INCLUDE_DIR
    NAMES fftw3.h
    HINTS $ENV{FFTW3_DIR}/include
    PATHS /usr/local/include
          /usr/include
          "/${MINGW_ARCH}/include"
          "$ENV{WD}/../../${MINGW_ARCH}/include"
          "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/fftw"
          "$ENV{ProgramW6432}/fftw"
          "$ENV{ProgramFiles}/fftw"
)
FIND_LIBRARY(
    FFTW3F_LIBRARY
    NAMES fftw3f libfftw3f fftw3f-3 libfftw3f-3
    HINTS $ENV{FFTW3_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
          "/${MINGW_ARCH}/bin"
          "$ENV{WD}/../../${MINGW_ARCH}/bin"
          "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/fftw"
          "$ENV{ProgramW6432}/fftw"
          "$ENV{ProgramFiles}/fftw"
)
# used for Windows only
FIND_PATH(
    FFTW3F_LIBRARY_DIR
    NAMES fftw3f.dll libfftw3f.dll fftw3f-3.dll libfftw3f-3.dll
    PATHS "/${MINGW_ARCH}/bin"
          "$ENV{WD}/../../${MINGW_ARCH}/bin"
          "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/fftw"
          "$ENV{ProgramW6432}/fftw"
          "$ENV{ProgramFiles}/fftw"
)

SET(FFTW3F_FOUND "NO")

IF( FFTW3F_INCLUDE_DIR AND FFTW3F_LIBRARY )
    SET(FFTW3F_FOUND "YES")
ENDIF()
