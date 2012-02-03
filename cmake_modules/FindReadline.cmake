if(NOT READLINE_INCLUDE_DIR OR NOT READLINE_LIBRARY)
    find_path(READLINE_INCLUDE_DIR readline/readline.h)
    find_library(READLINE_LIBRARY NAMES readline)
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
  endif ()
endif ()
