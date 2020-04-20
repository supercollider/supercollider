# This makes sclang/scide work with a Qt installation at a fixed, non-system location.
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(QT_IDE_COMPONENTS
    Concurrent
    Core
    LinguistTools
    # OpenGL
    PrintSupport
    Qml
    Quick
    Sql
    Widgets
)

set(QT_IDE_LIBRARIES
    Qt5::Concurrent
    Qt5::Core
    # Qt5::OpenGL
    Qt5::PrintSupport
    Qt5::Qml
    Qt5::Quick
    Qt5::Sql
)

if(SC_USE_QTWEBENGINE)
    list(APPEND QT_IDE_COMPONENTS
        WebChannel
        WebEngine
        WebEngineWidgets
        WebSockets
    )
    list(APPEND QT_IDE_LIBRARIES
        Qt5::WebChannel
        Qt5::WebEngineWidgets
        Qt5::WebSockets
    )
endif()

find_package(
    Qt5 ${REQUIRED_QT_VERSION}
    COMPONENTS
    ${QT_IDE_COMPONENTS}
    REQUIRED
)

if(${CMAKE_COMPILER_IS_GNUCXX})
    add_definitions(-Wreorder)
endif()

if (APPLE)
  include (${CMAKE_SOURCE_DIR}/cmake_modules/MacAppFolder.cmake)
  set(ide_name ${scappbundlename})
else()
  set(ide_name scide)
endif()

if(APPLE)
    if(NOT SC_SYMLINK_CLASSLIB)
        foreach(file ${ClassLibrary} ${HelpSource})
            string(REPLACE ${CMAKE_SOURCE_DIR} "" PathInBundle ${file} )
            get_filename_component(PathInBundle "${PathInBundle}" DIRECTORY)
            set_source_files_properties(${file} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${PathInBundle})
        endforeach()
    endif()

    foreach(file ${Sounds})
        string(REPLACE ${CMAKE_SOURCE_DIR} "" PathInBundle ${file} )
        get_filename_component(PathInBundle "${PathInBundle}" DIRECTORY)
        set_source_files_properties(${file} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${PathInBundle})
    endforeach()

    set( Icons ../../icons/sc_ide.icns)
    set_source_files_properties(../../icons/sc_ide.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set(AdditionalBundleSources ${ClassLibrary} ${HelpSource} ${BundleResources} ${Examples} ${Sounds} ${Icons})
endif()

set ( ide_moc_hdr
    core/main.hpp
    core/sig_mux.hpp
    core/sc_process.hpp
    core/sc_server.hpp
    core/doc_manager.hpp
    core/settings/manager.hpp
    core/session_manager.hpp
    widgets/cmd_line.hpp
    widgets/doc_list.hpp
    widgets/documents_dialog.hpp
    widgets/editor_box.hpp
    widgets/find_replace_tool.hpp
    widgets/goto_line_tool.hpp
    widgets/lookup_dialog.hpp
    widgets/main_window.hpp
    widgets/multi_editor.hpp
    widgets/popup_text_input.hpp
    widgets/post_window.hpp
    widgets/session_switch_dialog.hpp
    widgets/sessions_dialog.hpp
    widgets/tool_box.hpp
    widgets/audio_status_box.hpp
    widgets/lang_status_box.hpp
    widgets/code_editor/editor.hpp
    widgets/code_editor/sc_editor.hpp
    widgets/code_editor/highlighter.hpp
    widgets/code_editor/line_indicator.hpp
    widgets/code_editor/overlay.hpp
    widgets/code_editor/autocompleter.hpp
    widgets/code_editor/completion_menu.hpp
    widgets/settings/dialog.hpp
    widgets/settings/general_page.hpp
    widgets/settings/sclang_page.hpp
    widgets/settings/editor_page.hpp
    widgets/settings/shortcuts_page.hpp
    widgets/util/path_chooser_widget.hpp
    widgets/util/text_format_list_widget.hpp
    widgets/util/key_sequence_edit.hpp
    widgets/util/popup_widget.hpp
    widgets/util/color_widget.hpp
    widgets/util/docklet.hpp
    widgets/util/volume_widget.hpp
)

file (GLOB_RECURSE all_hdr *hpp)

set ( ide_src
    core/main.cpp
    core/app_palette.cpp
    core/sig_mux.cpp
    core/doc_manager.cpp
    core/sc_process.cpp
    core/sc_server.cpp
    core/sc_introspection.cpp
    core/sc_lexer.cpp
    core/settings/serialization.cpp
    core/settings/manager.cpp
    core/settings/theme.cpp
    core/settings/default_themes.cpp
    core/session_manager.cpp
    core/util/color.cpp
    core/util/standard_dirs.cpp
    core/util/scdoc_log.cpp
    widgets/cmd_line.cpp
    widgets/doc_list.cpp
    widgets/documents_dialog.cpp
    widgets/editor_box.cpp
    widgets/find_replace_tool.cpp
    widgets/lookup_dialog.cpp
    widgets/main_window.cpp
    widgets/multi_editor.cpp
    widgets/post_window.cpp
    widgets/session_switch_dialog.cpp
    widgets/audio_status_box.cpp
    widgets/lang_status_box.cpp
    widgets/code_editor/editor.cpp
    widgets/code_editor/sc_editor.cpp
    widgets/code_editor/highlighter.cpp
    widgets/code_editor/line_indicator.cpp
    widgets/code_editor/overlay.cpp
    widgets/code_editor/autocompleter.cpp
    widgets/code_editor/completion_menu.cpp
    widgets/settings/dialog.cpp
    widgets/settings/general_page.cpp
    widgets/settings/sclang_page.cpp
    widgets/settings/editor_page.cpp
    widgets/settings/shortcuts_page.cpp
    widgets/util/gui_utilities.cpp
    widgets/util/text_format_list_widget.cpp
    widgets/util/popup_widget.cpp
    widgets/util/docklet.cpp
    widgets/util/volume_widget.cpp
    widgets/util/status_box.cpp
    widgets/style/style.cpp

    ${CMAKE_SOURCE_DIR}/common/SC_TextUtils.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_macos.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_win.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_unix.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_iphone.cpp

    ${CMAKE_SOURCE_DIR}/editors/sc-ide/primitives/localsocket_utils.cpp
    ${CMAKE_SOURCE_DIR}/editors/sc-ide/primitives/localsocket_utils.hpp

    ${CMAKE_SOURCE_DIR}/SCDoc/SCDoc.cpp
    ${CMAKE_SOURCE_DIR}/SCDoc/SCDoc.tab.cpp
    ${CMAKE_SOURCE_DIR}/SCDoc/lex.scdoc.cpp
)

set(ide_webengine_moc_hdrs
    widgets/help_browser.hpp
    widgets/util/WebSocketTransport.hpp
    widgets/util/WebSocketClientWrapper.hpp
    widgets/util/IDEWebChannelWrapper.hpp
    ${CMAKE_SOURCE_DIR}/QtCollider/widgets/web_page.hpp
)
set(ide_webengine_src
    widgets/help_browser.cpp
    widgets/util/WebSocketTransport.cpp
    ${CMAKE_SOURCE_DIR}/QtCollider/widgets/web_page.cpp
)

if(SC_USE_QTWEBENGINE)
    list(APPEND ide_moc_hdr ${ide_webengine_moc_hdrs})
    list(APPEND ide_src ${ide_webengine_src})
endif()

set( ide_forms
    forms/settings_dialog.ui
    forms/settings_general.ui
    forms/settings_sclang.ui
    forms/settings_editor.ui
    forms/settings_shortcuts.ui
)

# QtCollider shared sources:
list(APPEND ide_moc_hdr
    ${CMAKE_SOURCE_DIR}/QtCollider/layouts/stack_layout.hpp
)
list(APPEND ide_src
    ${CMAKE_SOURCE_DIR}/QtCollider/layouts/stack_layout.cpp
)

if(APPLE)
	set_property(SOURCE ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_macos.cpp PROPERTY COMPILE_FLAGS -xobjective-c++)
endif()

if(APPLE)
	list(APPEND ide_src
        ${CMAKE_SOURCE_DIR}/QtCollider/hacks/hacks_mac.mm
    )
elseif(WIN32)
    list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/common)
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/include/plugin_interface)
include_directories(${YAMLCPP_INCLUDE_DIR})
# For QtCollider headers:
include_directories(${CMAKE_SOURCE_DIR})
# Needed for auto-generated forms headers:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets/util)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include(${CMAKE_SOURCE_DIR}/SCDoc/CMakeLists.txt)

# Translation files
set( translation_src
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide_de.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide_es.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide_fr.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide_ja.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide_pt.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide_ru.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide_sl.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide_sv.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide_zh.ts
)

# Translation file for source code native language, used only to handle singular/plural forms
set( native_translation_src ${CMAKE_CURRENT_SOURCE_DIR}/translations/scide.ts )

# A handy target to update translation source files
add_custom_target( update_ide_translations
    COMMAND lupdate ${CMAKE_CURRENT_SOURCE_DIR} -ts ${translation_src}
            -no-obsolete
    COMMAND lupdate ${CMAKE_CURRENT_SOURCE_DIR} -ts ${native_translation_src}
            -no-obsolete -pluralonly
)

qt5_wrap_cpp( ide_moc_src ${ide_moc_hdr} )
qt5_wrap_ui( ide_forms_src ${ide_forms} )
qt5_add_resources( ide_rcc resources.qrc )
qt5_add_translation( translations ${translation_src} ${native_translation_src} )

set(ide_sources ${ide_src} ${all_hdr} )

set(ide_rc_sources ${ide_moc_src} ${ide_forms_src} ${ide_rcc})

if(APPLE)
    foreach(file ${translations})
        set_source_files_properties(${file} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/translations)
    endforeach()
endif()

# This sets up the exe icon for windows.
if(WIN32)
 set(RES_FILES ${CMAKE_SOURCE_DIR}/platform/windows/Resources/scide.rc)
 set(CMAKE_RC_COMPILER_INIT windres)
 ENABLE_LANGUAGE(RC)
 SET(CMAKE_RC_COMPILE_OBJECT
 "<CMAKE_RC_COMPILER> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(WIN32)


# final builds of the IDE seem to be broken atm
if(0 AND FINAL_BUILD)
  CREATE_FINAL_FILE(scide_final.cpp ${ide_sources})
  add_library( libscide STATIC scide_final.cpp ${ide_rc_sources})
else()
  add_library( libscide STATIC ${ide_sources} ${ide_rc_sources})
endif()

# Enable targets that depend on libscide to find its headers and their contents.
# The existing code uses a mix of paths, such as:
#  "settings/manager.hpp"
#  "util/standard_dirs.hpp"
#  "../primitives/localsocket_utils.hpp"
# So we need add all the sub-directories to the search path:
target_include_directories(libscide PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/core/settings
    ${CMAKE_CURRENT_SOURCE_DIR}/core/util
    ${CMAKE_CURRENT_SOURCE_DIR}/forms
    ${CMAKE_CURRENT_SOURCE_DIR}/primitives
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/code_editor
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/settings
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/style
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/util
)

add_executable( SuperCollider MACOSX_BUNDLE core/main_function.cpp ${RES_FILES} ${AdditionalBundleSources} ${translations})

set_target_properties(libscide PROPERTIES PREFIX "")

if(APPLE)
    set_target_properties(SuperCollider PROPERTIES OUTPUT_NAME "${ide_name}")
else()
    set_target_properties(SuperCollider PROPERTIES OUTPUT_NAME "scide")
endif()

target_link_libraries( libscide PUBLIC
    ${QT_IDE_LIBRARIES}
    ${YAMLCPP_LIBRARY}
    oscpack
)

target_link_libraries( SuperCollider libscide)

target_link_libraries( libscide PUBLIC boost_system_lib boost_filesystem_lib)
include_directories(${boost_include_dirs})

if(APPLE)
    target_link_libraries( libscide PUBLIC "-framework CoreServices -framework Foundation")
    target_link_libraries( libscide PUBLIC "-framework Cocoa" )
elseif(WIN32)
    target_link_libraries( libscide PUBLIC wsock32 )
    # The following prevents a Windows console from showing up
    # when the executable is started:
    set_target_properties( SuperCollider PROPERTIES WIN32_EXECUTABLE TRUE )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(libscide PUBLIC rt)

    find_package(X11 REQUIRED)

    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(libscide PUBLIC ${X11_X11_LIB})
endif()

if(PTHREADS_FOUND)
    target_link_libraries(libscide PUBLIC ${PTHREADS_LIBRARIES})
endif()

if(LTO)
    set_property(TARGET SuperCollider
                 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

    set_property(TARGET SuperCollider
                 APPEND PROPERTY LINK_FLAGS "-flto -flto-report -fwhole-program")
endif()

if(SC_USE_QTWEBENGINE)
    message(STATUS "IDE: Building with QtWebEngine")
    target_compile_definitions(libscide PUBLIC SC_USE_QTWEBENGINE)
endif()

# Installation

if(APPLE)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Install" CACHE PATH "Install path prefix" FORCE)
    endif()

    # set how it shows up in the Info.plist file
    SET(MACOSX_BUNDLE_ICON_FILE ../../icons/sc_ide.icns)
    # ensure copyright in plist file always says current year as end date
    string(TIMESTAMP CURRENT_YEAR "%Y")

    install(TARGETS SuperCollider
          DESTINATION ${scappbundlename})

    # for processing into the plist:
    set_property(TARGET SuperCollider
	    PROPERTY MACOSX_BUNDLE_BUNDLE_NAME ${scappbundlename})

elseif(WIN32)

    if(NOT MSVC)
        set_target_properties(SuperCollider PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<CONFIG>")
    endif(NOT MSVC)

    if(MSYS)
      add_custom_command(TARGET SuperCollider
        POST_BUILD
        COMMAND "${CMAKE_SOURCE_DIR}/platform/windows/junctions.sh" "create" "${CMAKE_SOURCE_DIR}" "$<TARGET_FILE_DIR:SuperCollider>"
        COMMENT "Creating links to SCClassLibrary, HelpSource, examples and sounds"
      )
    else()
      add_custom_command(TARGET SuperCollider
        PRE_BUILD
        COMMAND cmd /C "\"\"${CMAKE_SOURCE_DIR}/platform/windows/junctions.bat\" create \"$<TARGET_FILE_DIR:SuperCollider>\" \"${CMAKE_SOURCE_DIR}\"\""
        COMMENT "Creating links to SCClassLibrary, HelpSource, examples and sounds"
      )
    endif()

    foreach(translation ${translations})
      add_custom_command(TARGET SuperCollider
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SuperCollider>/translations
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${translation} $<TARGET_FILE_DIR:SuperCollider>/translations
      )
    endforeach()

    install(TARGETS SuperCollider
        DESTINATION "${SC_WIN_BUNDLE_NAME}"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )
    install(FILES ${translations}
        DESTINATION "${SC_WIN_BUNDLE_NAME}/translations"
    )

    set(SC_WIN_DLL_DIRS)
    if(SNDFILE_LIBRARY_DIR)
      list(APPEND SC_WIN_DLL_DIRS "${SNDFILE_LIBRARY_DIR}")
    endif(SNDFILE_LIBRARY_DIR)
    if(FFTW3F_LIBRARY_DIR)
      list(APPEND SC_WIN_DLL_DIRS "${FFTW3F_LIBRARY_DIR}")
    endif(FFTW3F_LIBRARY_DIR)
    if(READLINE_LIBRARY_DIR)
      list(APPEND SC_WIN_DLL_DIRS "${READLINE_LIBRARY_DIR}")
    endif(READLINE_LIBRARY_DIR)
    if(QT_BIN_PATH)
      list(APPEND SC_WIN_DLL_DIRS "${QT_BIN_PATH}")
    endif(QT_BIN_PATH)

    # run fixup_bundle prior to deployment to get FFTW, libsndfile, MSVC, and Qt libs
    # will run windeployqt afterward to auto-deploy QtWebEngine dependencies
    install(CODE
        "
            include(BundleUtilities)
            fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${SC_WIN_BUNDLE_NAME}/sclang.exe\" \"\" \"${SC_WIN_DLL_DIRS}\")
        "
    )


else()
  install(TARGETS SuperCollider
      RUNTIME DESTINATION "bin"
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
  install(FILES ${translations}
      DESTINATION "share/SuperCollider/translations")
endif()

# mac/windows: find the Qt deploy utility.
# See https://doc.qt.io/qt-5/windows-deployment.html for windeployqt
# See https://doc.qt.io/qt-5/osx-deployment.html for macdeployqt
if(APPLE OR WIN32)
    if(APPLE)
        find_program(DEPLOY_PROG macdeployqt PATHS ${CMAKE_PREFIX_PATH} PATH_SUFFIXES bin)
        # only the IDE executable is scanned normally, so force scanning of sclang as well
        # to get QtWebEngine, libsndfile, and other libs. also force scanning of QtWebEngineProcess
        # because macdeployqt is a fickle beast.
        set(CONTENTS_DIR ${CMAKE_INSTALL_PREFIX}/SuperCollider/SuperCollider.app/Contents)
        set(DEPLOY_CMD "\"${DEPLOY_PROG}\"
                \"${CMAKE_INSTALL_PREFIX}/SuperCollider/SuperCollider.app\"
                -verbose=1
                -executable=${CONTENTS_DIR}/MacOS/sclang
                -executable=${CONTENTS_DIR}/Resources/scsynth
                -executable=${CONTENTS_DIR}/Resources/supernova
                -executable=${CONTENTS_DIR}/Resources/plugins/DiskIO_UGens.scx
                -executable=${CONTENTS_DIR}/Resources/plugins/DiskIO_UGens_supernova.scx
                -executable=${CONTENTS_DIR}/Frameworks/QtWebEngineCore.framework/Versions/5/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess
            ")
        set(VERIFY_CMD "include(BundleUtilities)")
        if(SUPERNOVA)
            get_target_property(LINK_FFTW libsupernova SUPERNOVA_FFTW)
            if(LINK_FFTW)
                message(STATUS "macdeployqt: will fix linking FFTW for supernova")
                # For supernova, libfftw3f.3.dylib has a further dependency (libgcc_s) which is copied in,
                # but libfftw3f is not fixed up automatically when supernova is deployed.
                string(APPEND VERIFY_CMD "
                message(STATUS \"Fixing up bad path in libfftw3f.3.dylib manually due to faulty logic in macdeployqt\")
                execute_process(COMMAND install_name_tool -change /usr/local/lib/gcc/9/libgcc_s.1.dylib
                    @loader_path/libgcc_s.1.dylib ${CONTENTS_DIR}/Frameworks/libfftw3f.3.dylib)")
            endif()
        endif()
        string(APPEND VERIFY_CMD "
            message(STATUS \"Verifying app\")
            verify_app(\"${CMAKE_INSTALL_PREFIX}/SuperCollider/SuperCollider.app\")")
    else() # WIN32
        find_program(DEPLOY_PROG windeployqt PATHS ${CMAKE_PREFIX_PATH})
        set(DEPLOY_CMD "\"${DEPLOY_PROG}\"
                --verbose 1 --no-compiler-runtime
                \"${CMAKE_INSTALL_PREFIX}/${SC_WIN_BUNDLE_NAME}/sclang.exe\"
            ")
        set(VERIFY_CMD "")
    endif()

    if(NOT DEPLOY_PROG)
        message(FATAL_ERROR "Could not find macdeployqt executable. Please make sure it is in PATH or CMAKE_PREFIX_PATH/bin")
    endif()

    message(STATUS "Found Qt deploy utility: ${DEPLOY_PROG}")
    install(CODE
        "
            message(STATUS \"Deploying additional Qt resources and libraries\")
            execute_process(COMMAND ${DEPLOY_CMD}
                RESULT_VARIABLE RES
                OUTPUT_VARIABLE OUT
                ERROR_VARIABLE ERR
                )
            if(OUT)
                message(STATUS \"Deploy result: \${OUT}\")
            endif()
            if(RES)
                message(FATAL_ERROR \"Deploy failed with error: \${ERR}\")
            endif()
            ${VERIFY_CMD}
        "
    )
endif()

if (LINUX)
  install( PROGRAMS SuperColliderIDE.desktop  DESTINATION "share/applications")
endif()
