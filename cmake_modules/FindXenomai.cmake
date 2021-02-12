# - Try to find xenomai
# Once done this will define
#
#    XENOMAI_FOUND - system has xenomai
#    XENOMAI_INCLUDE_DIRS - the xenomai include directory
#    XENOMAI_LIBRARIES - Link these to use xenomai
#    XENOMAI_DEFINITIONS - Compiler switches required for using xenomai
#
#    Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#    Modified for other libraries by Lasse Kärkkäinen <tronic>
#
#    Redistribution and use is allowed according to the terms of the New
#    BSD license.
#    For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if(XENOMAI_LIBRARIES AND XENOMAI_INCLUDE_DIRS AND XENOMAI_DEFINITIONS)
    # in cache already
    set(XENOMAI_FOUND TRUE)
else()
    # Xenomai comes with its own ...-config program to get cflags and ldflags
    if(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM)
        # ifcross compiling, we want to find this program only from the sysroot
        set(CACHED ${CMAKE_FIND_ROOT_PATH_MODE_PROGRAM})
        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
    endif()
    find_program(XENOMAI_XENO_CONFIG
        NAMES
            xeno-config
        PATHS
            ${_XENOMAI_INCLUDEDIR}
            /usr/xenomai/bin
            /usr/local/bin
            /usr/bin
    )
    if(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM)
        # restore the previous value
        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ${CACHED})
    endif()

    if(XENOMAI_XENO_CONFIG)
        set(XENOMAI_FOUND TRUE)
        execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --skin=posix --cflags OUTPUT_VARIABLE XENOMAI_CFLAGS)
        string(STRIP "${XENOMAI_CFLAGS}" XENOMAI_CFLAGS)
        # use grep to separate out defines and includes
        execute_process(
            COMMAND bash -c "A= ; for a in ${XENOMAI_CFLAGS}; do echo $a | grep -q \"\\-D.*\" && A=\"$A $a\"; done; echo $A"
            OUTPUT_VARIABLE XENOMAI_DEFINITIONS)
        string(STRIP "${XENOMAI_DEFINITIONS}" XENOMAI_DEFINITIONS)

        execute_process(
            COMMAND bash -c "A= ; for a in ${XENOMAI_CFLAGS}; do echo $a | grep -q \"\\-I.*\" && A=\"$A `echo $a|sed s/-I//`;\"; done; echo $A"
            OUTPUT_VARIABLE XENOMAI_INCLUDE_DIRS)
        string(STRIP "${XENOMAI_INCLUDE_DIRS}" XENOMAI_INCLUDE_DIRS)

        execute_process(
            COMMAND ${XENOMAI_XENO_CONFIG} --skin=posix --ldflags --no-auto-init
            COMMAND sed "s/-Wl,@.*wrappers//g"
            OUTPUT_VARIABLE XENOMAI_LIBRARIES)
        string(STRIP "${XENOMAI_LIBRARIES}" XENOMAI_LIBRARIES)
    endif()

    if(XENOMAI_FOUND)
        if(NOT XENOMAI_FIND_QUIETLY)
            execute_process(COMMAND ${XENOMAI_XENO_CONFIG} --prefix OUTPUT_VARIABLE XENOMAI_PREFIX)
            message(STATUS "Found xenomai: ${XENOMAI_PREFIX}")
            message(STATUS "XENOMAI_LIBRARIES: ${XENOMAI_LIBRARIES}")
            message(STATUS "XENOMAI_INCLUDE_DIRS: ${XENOMAI_INCLUDE_DIRS}")
            message(STATUS "XENOMAI_DEFINITIONS: ${XENOMAI_DEFINITIONS}")
        endif()
    elseif(XENOMAI_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find XENOMAI")
    endif()

    # show the XENOMAI_ variables only in the advanced view
    mark_as_advanced(XENOMAI_LIBRARIES XENOMAI_INCLUDE_DIRS XENOMAI_DEFINITIONS)

endif()
