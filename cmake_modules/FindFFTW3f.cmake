# Find single-precision (float) version of FFTW3

FIND_PATH(
    FFTW3F_INCLUDE_DIR
    NAMES fftw3.h
    HINTS $ENV{FFTW3_DIR}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    FFTW3F_LIBRARY
    NAMES fftw3f libfftw3f
    HINTS $ENV{FFTW3_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
)

SET(FFTW3F_FOUND "NO")

IF( FFTW3F_INCLUDE_DIR AND FFTW3F_LIBRARY )
    SET(FFTW3F_FOUND "YES")
ENDIF()
