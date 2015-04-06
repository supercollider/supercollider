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
# the installer

add_custom_target( installer
    COMMAND ${CMAKE_COMMAND}
        "-DSC_VERSION=${PROJECT_VERSION}"
        "-DSC_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
        "-DNSIS_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/supercollider.nsi"
        "-DSC_CUBE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/Resources/sc_cube.ico"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/package.cmake"
    COMMENT "Building the installer:"
    VERBATIM
)
