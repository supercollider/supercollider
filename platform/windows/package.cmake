file(TO_NATIVE_PATH ${SC_INSTALL_PREFIX} NATIVE_INSTALL_PREFIX)

find_program(NSIS_PROGRAM makensis)

if(NOT NSIS_PROGRAM)
    message(FATAL_ERROR "ERROR: makensis program was not found, cannot run packaging script.")
endif()

execute_process( COMMAND ${NSIS_PROGRAM}
    /DSC_VERSION=${SC_VERSION}
    /DSC_INSTALL_ROOT="${NATIVE_INSTALL_PREFIX}"
    # FIXME: Does not support spaces in path:
    /DSC_DST_DIR=${NATIVE_INSTALL_PREFIX}
    /DSC_ICON=${SC_CUBE_PATH}
    /DFILE_NAME_SUFIX=${BUILD_DEF}
    /DTARCH=${TARGET_ARCH}
    /DBUNDLE_NAME=${SC_WIN_BUNDLE_NAME}
    /DSC_SIZE=${SC_WIN_INSTALL_SIZE}
    ${NSIS_SCRIPT}
    RESULT_VARIABLE NSIS_RESULT
)

if(NSIS_RESULT)
    message(FATAL_ERROR "ERROR: makensis failed, exiting with: ${NSIS_RESULT}")
endif()
