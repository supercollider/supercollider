file(TO_NATIVE_PATH ${SC_INSTALL_PREFIX} NATIVE_INSTALL_PREFIX)

if(SC_ED)
    set(DEFINE_SC_ED "/DSC_ED")
endif()

include(BundleUtilities)
fixup_bundle(
    "${SC_INSTALL_PREFIX}/SuperCollider/sclang.exe"
    ""
    "${SC_WIN_DEP_DIRS}"
)

execute_process( COMMAND makensis
    /DSC_VERSION=${SC_VERSION}
    /DSC_SRC_DIR="${NATIVE_INSTALL_PREFIX}"
    # FIXME: Does not support spaces in path:
    /DSC_DST_DIR=${NATIVE_INSTALL_PREFIX}
    ${DEFINE_SC_ED}
    ${NSIS_SCRIPT}
)
