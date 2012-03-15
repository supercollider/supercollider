install(FILES ${CMAKE_SOURCE_DIR}/COPYING DESTINATION SuperCollider)

# Add a target that invokes a cmake script that:
# -- invokes fixup_bundle to copy dependencies over
# -- invokes a custom NSIS script, because CPack would
#    not create a suitable installer for sced

add_custom_target( package
    COMMAND cmake
        "-DSC_VERSION=${PROJECT_VERSION}"
        "-DSC_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
        "-DSC_WIN_DEP_DIRS=${SC_WIN_DEP_DIRS}"
        "-DSC_ED=${SC_ED}"
        "-DNSIS_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/supercollider.nsi"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/package.cmake"
    COMMENT "Bundling the package and building the installer:"
    VERBATIM
)
