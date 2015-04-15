# Find single-precision (float) version of FFTW3

FIND_PATH(
    FFTW3F_INCLUDE_DIR
    NAMES fftw3.h
    HINTS $ENV{FFTW3_DIR}/include
    PATHS /usr/local/include
          /usr/include 
		  ${CMAKE_SOURCE_DIR}/../fftw
          /${MINGW_ARCH}/include
          $ENV{WD}/../../${MINGW_ARCH}/include
		  $ENV{ProgramW6432}/fftw
          $ENV{ProgramFiles}/fftw
)
FIND_LIBRARY(
    FFTW3F_LIBRARY
    NAMES fftw3f libfftw3f fftw3f-3 libfftw3f-3
    HINTS $ENV{FFTW3_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
		  ${CMAKE_SOURCE_DIR}/../fftw
          /${MINGW_ARCH}/bin
          $ENV{WD}/../../${MINGW_ARCH}/bin
		  $ENV{ProgramW6432}/fftw
          $ENV{ProgramFiles}/fftw
)
FIND_PATH(
    FFTW3F_LIBRARY_DIR
    NAMES fftw3f libfftw3f fftw3f-3 libfftw3f-3.dll
	PATHS ${CMAKE_SOURCE_DIR}/../fftw
          /${MINGW_ARCH}/bin
          $ENV{WD}/../../${MINGW_ARCH}/bin
          $ENV{ProgramW6432}/fftw
          $ENV{ProgramFiles}/fftw
)

SET(FFTW3F_FOUND "NO")

IF( FFTW3F_INCLUDE_DIR AND FFTW3F_LIBRARY )
    SET(FFTW3F_FOUND "YES")
ENDIF()
