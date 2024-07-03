# tries to find qt via brew

if(APPLE)
    if(NOT DEFINED QT5_PREFIX_PATH)
        execute_process(
            COMMAND brew --prefix qt@5
            OUTPUT_VARIABLE QT5_PREFIX_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(QT5_PREFIX_PATH)
            message(STATUS "Found qt@5 installation via homebrew at ${QT5_PREFIX_PATH}")
        else()
            message(WARNING "Could not find qt@5 via homebrew and no qt5 path specified.")
        endif()
    endif()

    if(QT5_PREFIX_PATH)
        set(CMAKE_PREFIX_PATH ${QT5_PREFIX_PATH} CACHE PATH "Path to Qt5")
        message(STATUS "Using qt5 prefix path: ${QT5_PREFIX_PATH}")
    else()
        message(WARNING "qt5 could not be found - consider providing a QT5_DIR environment variable or installing qt@5 via homebrew.")
    endif()
endif()
