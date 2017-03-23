# This target invokes a custom NSIS script to create
# the installer, because CPack would not create a suitable
# installer for sced:

if(MSVC)
  SET(BUILD_ENV "VS")
  # size for win 3.7 release build
  SET(SC_WIN_INSTALL_SIZE "102298")
elseif(MSYS)
  SET(BUILD_ENV "MSYS")
  # size for win 3.7 release build
  SET(SC_WIN_INSTALL_SIZE "")
elseif(MINGW)
  set(BUILD_ENV "MinGW")
  # size for win 3.7 release build
  SET(SC_WIN_INSTALL_SIZE "121856")
else()
  set(BUILD_ENV "NN")
  SET(SC_WIN_INSTALL_SIZE "")
endif(MSVC)

add_custom_target( installer
    COMMAND ${CMAKE_COMMAND}
        "-DSC_VERSION=${PROJECT_VERSION}"
        "-DSC_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
        "-DNSIS_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/supercollider.nsi"
        "-DSC_CUBE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/Resources/sc_cube.ico"
        "-DBUILD_DEF=$<CONFIG>-${CMAKE_LIBRARY_ARCHITECTURE}-${BUILD_ENV}-${GIT_COMMIT_HASH}"
        "-DTARGET_ARCH=${CMAKE_LIBRARY_ARCHITECTURE}"
        "-DSC_WIN_BUNDLE_NAME=${SC_WIN_BUNDLE_NAME}"
        "-DSC_WIN_INSTALL_SIZE=${SC_WIN_INSTALL_SIZE}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/package.cmake"
    COMMENT "Building the installer..."
    VERBATIM
)
