if(NOT READLINE_INCLUDE_DIR OR NOT READLINE_LIBRARY)
    if(APPLE)
        execute_process(COMMAND brew --prefix readline OUTPUT_VARIABLE READLINE_BREW_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
        find_path(READLINE_INCLUDE_DIR 
            NAMES readline/readline.h
            HINTS "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/readline/include" # vcpkg
                "${READLINE_BREW_PREFIX}/include" # homebrew
                "/opt/local/include" # macports
            NO_CMAKE_SYSTEM_PATH # we don't want the old readline shipped with macOS
        )
        find_library(READLINE_LIBRARY 
            NAMES readline
            HINTS "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/readline/lib"
                "${READLINE_BREW_PREFIX}/lib"
                "/opt/local/lib"
            NO_CMAKE_SYSTEM_PATH
        )
    elseif(WIN32)
        find_path(READLINE_INCLUDE_DIR
            NAMES readline/readline.h
            HINTS "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/readline/include"
                "$ENV{ProgramW6432}/GnuWin32/include"
                "$ENV{ProgramFiles}/GnuWin32/include"
        )
        find_library(READLINE_LIBRARY
            NAMES readline6 readline5 readline libreadline6 libreadline5 libreadline
            HINTS "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/readline/lib"
                "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/readline/bin"
                "$ENV{ProgramW6432}/GnuWin32/lib"
                "$ENV{ProgramW6432}/GnuWin32/bin"
                "$ENV{ProgramFiles}/GnuWin32/lib"
                "$ENV{ProgramFiles}/GnuWin32/bin"
        )
        find_path(READLINE_LIBRARY_DIR
            NAMES readline5.dll libreadline5.dll readline6.dll libreadline6.dll readline.dll libreadline.dll
            HINTS "${CMAKE_SOURCE_DIR}/../${CMAKE_LIBRARY_ARCHITECTURE}/readline/bin"
                "$ENV{ProgramW6432}/GnuWin32/bin"
                "$ENV{ProgramFiles}/GnuWin32/bin"
            PATH_SUFFIXES "bin"
        )
    else()
        find_path(READLINE_INCLUDE_DIR readline/readline.h)
        find_library(READLINE_LIBRARY NAMES readline)
    endif()
endif()

if (READLINE_INCLUDE_DIR AND READLINE_LIBRARY)
    set(READLINE_FOUND TRUE)
endif()

if (READLINE_INCLUDE_DIR AND EXISTS "${READLINE_INCLUDE_DIR}/readline/readline.h")
  file(STRINGS "${READLINE_INCLUDE_DIR}/readline/readline.h"
               READLINE_MAJOR_VERSION
       REGEX "^#define RL_VERSION_MAJOR.*$")
  file(STRINGS "${READLINE_INCLUDE_DIR}/readline/readline.h"
               READLINE_MINOR_VERSION
       REGEX "^#define RL_VERSION_MINOR.*$")

  string(REGEX REPLACE "^.*RL_VERSION_MAJOR.*([0-9]+).*$"
                       "\\1"
                       READLINE_MAJOR_VERSION
                       "${READLINE_MAJOR_VERSION}")
  string(REGEX REPLACE "^.*RL_VERSION_MINOR.*([0-9]+).*$"
                       "\\1"
                       READLINE_MINOR_VERSION
                       "${READLINE_MINOR_VERSION}")

  if(READLINE_MAJOR_VERSION)
    set(READLINE_VERSION "${READLINE_MAJOR_VERSION}.${READLINE_MINOR_VERSION}")
  else()
    set(READLINE_VERSION "(unknown)")
  endif()
endif ()

if (READLINE_FOUND)
  if (Readline_FIND_VERSION VERSION_GREATER READLINE_VERSION)
    message(STATUS "Found version ${READLINE_VERSION} of GNU Readline at ${READLINE_LIBRARY}, but version ${Readline_FIND_VERSION} required")
    unset(READLINE_FOUND)
  else()
    if (NOT READLINE_FIND_QUIETLY)
      message(STATUS "Found GNU Readline version ${READLINE_VERSION}: ${READLINE_LIBRARY}")
    endif ()
  endif ()
else ()
  if (READLINE_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find GNU Readline")
  else ()
    message(STATUS "Readline not found, building without readline support")
  endif ()
endif ()
