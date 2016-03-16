# This target just helps creating a standalone server bundle. It depends
# on the required targets (scsynth and plugins, optionally supernova) to have
# been built in advance. In the full SC build, Qt and implicitly the IDE
# can be skipped (-DSC_QT=OFF) to speed things up.
add_custom_target(install_server_only
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX}/server"
    COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:scsynth> "${CMAKE_INSTALL_PREFIX}/server"
    COMMAND ${CMAKE_COMMAND} "-DSC_WIN_SERVER_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/server" "-DSC_WIN_DLL_DIRS=${SC_WIN_DLL_DIRS}" -P "${CMAKE_CURRENT_SOURCE_DIR}/server_bundle.cmake"
    COMMENT "Copying target server to folder install_prefix/server and checking for dependencies (MS runtimes are not copied! See README_WINDOWS.md) ..."
    DEPENDS scsynth
    VERBATIM
)


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
