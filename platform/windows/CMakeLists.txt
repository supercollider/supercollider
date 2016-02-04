SET(CMAKE_LIBRARY_PATH "${CMAKE_PREFIX_PATH}/bin;${SNDFILE_LIBRARY_DIR};${PORTAUDIO_LIBRARY_DIRS};${FFTW3F_LIBRARY_DIR};${READLINE_LIBRARY_DIR}")

# MinGW only: This target invokes fixup_bundle to copy
# dependencies into the installation directory. Fixup_bundle
# is so slow with MinGW that we can't wait after each install:
if(MINGW)
    add_custom_target( bundle
        COMMAND ${CMAKE_COMMAND}
            "-DSC_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
            "-DSC_DEPENDENCY_DIRS=${CMAKE_LIBRARY_PATH}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/bundle.cmake"
        COMMENT "Bundling the dependencies:"
        VERBATIM
    )
    SET(BUILD_ENV "MinGW")
else(MINGW)
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
    SET(BUILD_ENV "VS")
endif(MINGW)

# This target invokes a custom NSIS script to create
# the installer, because CPack would not create a suitable
# installer for sced:
add_custom_target( installer
    COMMAND ${CMAKE_COMMAND}
        "-DSC_VERSION=${PROJECT_VERSION}"
        "-DSC_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
        "-DNSIS_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/supercollider.nsi"
        "-DSC_CUBE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/Resources/sc_cube.ico"
        "-DBUILD_DEF=${CMAKE_BUILD_TYPE}-${CMAKE_LIBRARY_ARCHITECTURE}-${BUILD_ENV}-${GIT_COMMIT_HASH}"
        "-DTARGET_ARCH=${CMAKE_LIBRARY_ARCHITECTURE}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/package.cmake"
    COMMENT "Building the installer:"
    VERBATIM
)
