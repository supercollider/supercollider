##Very graciously copied from
##https://github.com/FreeRDP/FreeRDP/blob/master/cmake/FindUDev.cmake

FIND_PATH(
    UDEV_INCLUDE_DIR
    libudev.h
    /usr/include
    /usr/local/include
    ${UDEV_PATH_INCLUDES}
)

FIND_LIBRARY(
    UDEV_LIBRARIES
    NAMES udev libudev
    PATHS
        /usr/lib${LIB_SUFFIX}
        /usr/local/lib${LIB_SUFFIX}
        ${UDEV_PATH_LIB}
)

IF (UDEV_LIBRARIES AND UDEV_INCLUDE_DIR)
    SET(UDEV_FOUND "YES")
    execute_process(COMMAND pkg-config --atleast-version=143 libudev RESULT_VARIABLE UDEV_STABLE)
    # retvale is 0 of the condition is "true" so we need to negate the value...
    if (UDEV_STABLE)
set(UDEV_STABLE 0)
    else (UDEV_STABLE)
set(UDEV_STABLE 1)
    endif (UDEV_STABLE)
    message(STATUS "libudev stable: ${UDEV_STABLE}")
ENDIF (UDEV_LIBRARIES AND UDEV_INCLUDE_DIR)

IF (UDEV_FOUND)
    MESSAGE(STATUS "Found UDev: ${UDEV_LIBRARIES}")
    MESSAGE(STATUS " include: ${UDEV_INCLUDE_DIR}")
ELSE (UDEV_FOUND)
    MESSAGE(STATUS "UDev not found.")
    MESSAGE(STATUS "UDev: You can specify includes: -DUDEV_PATH_INCLUDES=/opt/udev/include")
    MESSAGE(STATUS " currently found includes: ${UDEV_INCLUDE_DIR}")
    MESSAGE(STATUS "UDev: You can specify libs: -DUDEV_PATH_LIB=/opt/udev/lib")
    MESSAGE(STATUS " currently found libs: ${UDEV_LIBRARIES}")
    IF (UDev_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find UDev library")
    ENDIF (UDev_FIND_REQUIRED)
ENDIF (UDEV_FOUND)

