install(FILES ${CMAKE_SOURCE_DIR}/platform/windows/Resources/sc_cube.ico
    DESTINATION .
)

SET(CMAKE_LIBRARY_PATH "${CMAKE_PREFIX_PATH}/bin;${SNDFILE_LIBRARY_DIR};${PORTAUDIO_LIBRARY_DIR};${FFTW3F_LIBRARY_DIR};${READLINE_LIBRARY_DIR}")

install(CODE "
	include(BundleUtilities)
	fixup_bundle(
		\"${CMAKE_INSTALL_PREFIX}/SuperCollider/sclang.exe\"
		\"\"
		\"${CMAKE_LIBRARY_PATH}\" )
	"
    COMMENT "Bundling the dependencies:"
    VERBATIM
)

# This target invokes a custom NSIS script to create
# the installer, because CPack would not create a suitable
# installer for sced:
add_custom_target( installer
    COMMAND ${CMAKE_COMMAND}
        "-DSC_VERSION=${PROJECT_VERSION}"
        "-DSC_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
        "-DSC_ED=${SC_ED}"
        "-DNSIS_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/supercollider.nsi"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/package.cmake"
    COMMENT "Building the installer:"
    VERBATIM
)
