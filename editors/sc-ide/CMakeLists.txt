cmake_minimum_required( VERSION 2.8.11 )

# This makes sclang/scide work with a Qt installation at a fixed, non-system location.
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(Qt5Core)
find_package(Qt5Concurrent)
find_package(Qt5LinguistTools)
find_package(Qt5OpenGL)
find_package(Qt5Positioning)
find_package(Qt5PrintSupport)
find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5Sensors)
find_package(Qt5Sql)
find_package(Qt5WebKitWidgets)
find_package(Qt5Widgets)

set(QT_IDE_LIBRARIES
    Qt5::Core Qt5::Concurrent Qt5::WebKitWidgets Qt5::OpenGL Qt5::PrintSupport Qt5::Sensors Qt5::Quick Qt5::Qml Qt5::Sql Qt5::Positioning)

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

    set( Icons ../../icons/SCcube.icns)
    set_source_files_properties(../../icons/SCcube.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

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
    widgets/help_browser.hpp
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

    ${CMAKE_SOURCE_DIR}/QtCollider/widgets/web_page.hpp
    ${CMAKE_SOURCE_DIR}/editors/sc-ide/primitives/localsocket_utils.cpp
    ${CMAKE_SOURCE_DIR}/editors/sc-ide/primitives/localsocket_utils.hpp
)

file (GLOB_RECURSE all_hdr *hpp)

set ( ide_src
    core/main.cpp
    core/sig_mux.cpp
    core/doc_manager.cpp
    core/sc_process.cpp
    core/sc_server.cpp
    core/sc_introspection.cpp
    core/sc_lexer.cpp
    core/settings/serialization.cpp
    core/settings/manager.cpp
    core/settings/theme.cpp
    core/session_manager.cpp
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
    widgets/help_browser.cpp
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

    ${CMAKE_SOURCE_DIR}/QtCollider/widgets/web_page.cpp

    ${CMAKE_SOURCE_DIR}/common/SC_TextUtils.cpp
    ${CMAKE_SOURCE_DIR}/editors/sc-ide/primitives/localsocket_utils.cpp
    ${CMAKE_SOURCE_DIR}/editors/sc-ide/primitives/localsocket_utils.hpp

    ${CMAKE_SOURCE_DIR}/SCDoc/SCDoc.cpp
    ${CMAKE_SOURCE_DIR}/SCDoc/SCDoc.tab.cpp
    ${CMAKE_SOURCE_DIR}/SCDoc/lex.scdoc.cpp
)

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

list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_DirUtils.cpp)
if(APPLE)
	set_property(SOURCE ${CMAKE_SOURCE_DIR}/common/SC_DirUtils.cpp PROPERTY COMPILE_FLAGS -xobjective-c++)
endif()

if(APPLE)
	list(APPEND ide_src
        ${CMAKE_SOURCE_DIR}/common/SC_StandAloneInfo_Darwin.cpp
        ${CMAKE_SOURCE_DIR}/QtCollider/hacks/hacks_mac.mm
    )
elseif(WIN32)
    list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/common)
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/include/plugin_interface)
include_directories(${YAMLCPP_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external_libraries/boost)
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

set(ide_rc_sources ${ide_moc_src} ${ide_forms_src} ${ide_rcc} ${translations})

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
  add_executable( SuperCollider MACOSX_BUNDLE scide_final.cpp ${ide_rc_sources} ${RES_FILES} ${AdditionalBundleSources})
else()
  add_executable( SuperCollider MACOSX_BUNDLE ${ide_sources} ${ide_rc_sources} ${RES_FILES} ${AdditionalBundleSources})
endif()

if(APPLE)
    set_target_properties(SuperCollider PROPERTIES OUTPUT_NAME "${ide_name}")
else()
    set_target_properties(SuperCollider PROPERTIES OUTPUT_NAME "scide")
endif()

target_link_libraries( SuperCollider
    ${QT_IDE_LIBRARIES}
    ${YAMLCPP_LIBRARY}
    oscpack
)

if(Boost_FOUND)
    target_link_libraries( SuperCollider ${Boost_SYSTEM_LIBRARY} )
else()
    # in-house-built boost system
    target_link_libraries( SuperCollider boost_system )
endif()

if(APPLE)
    target_link_libraries( ${ide_name} "-framework CoreServices -framework Foundation")
    target_link_libraries( ${ide_name} "-framework Cocoa" )
elseif(WIN32)
    target_link_libraries( SuperCollider wsock32 )
    # The following prevents a Windows console from showing up
    # when the executable is started:
    set_target_properties( SuperCollider PROPERTIES WIN32_EXECUTABLE TRUE )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(SuperCollider rt)

    find_package(X11 REQUIRED)

    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(SuperCollider ${X11_X11_LIB})
endif()

if(PTHREADS_FOUND)
    target_link_libraries(SuperCollider ${PTHREADS_LIBRARIES})
endif()

if(LTO)
    set_property(TARGET SuperCollider
                 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

    set_property(TARGET SuperCollider
                 APPEND PROPERTY LINK_FLAGS "-flto -flto-report -fwhole-program")
endif()


# Installation

if(APPLE)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Install" CACHE PATH "Install path prefix" FORCE)
    endif()

    # set how it shows up in the Info.plist file
    SET(MACOSX_BUNDLE_ICON_FILE ../../icons/SCcube.icns)

    install(TARGETS SuperCollider
        DESTINATION ${scappbundlename}
        )

    # for processing into the plist:
    set_property(TARGET SuperCollider
	    PROPERTY MACOSX_BUNDLE_BUNDLE_NAME ${scappbundlename})

    set(MACOSX_BUNDLE_BUNDLE_NAME "${scappbundlename}")

    include(InstallRequiredSystemLibraries)

    include(DeployQt5)
    install_qt5_executable("${scappbundlename}/${scappbundlename}.app" "" "" "${CMAKE_BINARY_DIR}/lang/sclang;${CMAKE_BINARY_DIR}/server/scsynth;${CMAKE_BINARY_DIR}/external_libraries")

    # install Qt plugins (adapted from kvirc)
    set(plugin_dest_dir ${CMAKE_INSTALL_PREFIX}/SuperCollider/SuperCollider.app/Contents/Plugins)
    set(QT_PLUGINS_DIR "${Qt5Widgets_DIR}/../../../plugins")
    install(DIRECTORY "${QT_PLUGINS_DIR}/" DESTINATION ${plugin_dest_dir} COMPONENT Runtime
                       FILES_MATCHING REGEX "(platforms)/[^_]*\\.dylib")

    install(CODE "
           file(GLOB_RECURSE QTPLUGINS
                  \"${plugin_dest_dir}/*.dylib\")
           file(GLOB_RECURSE SCPLUGINS
                  \"${CMAKE_INSTALL_PREFIX}/SuperCollider/SuperCollider.app/Contents/Resources/plugins/*.scx\")
                include(BundleUtilities)
                fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/SuperCollider/SuperCollider.app\" \"\${QTPLUGINS};\${SCPLUGINS}\" \"${QT_LIBRARY_DIR}\")
                #fixup_qt5_executable(\"${CMAKE_INSTALL_PREFIX}/SuperCollider/SuperCollider.app\")
                " COMPONENT Runtime)

elseif(WIN32)

    if(NOT MSVC)
        set_target_properties(SuperCollider PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<CONFIG>")
    endif(NOT MSVC)

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

    include(DeployQt5)
    install_qt5_executable(
      "${SC_WIN_BUNDLE_NAME}/sclang.exe"
      ""
      ""
      "${SC_WIN_DLL_DIRS}"
    )

else()
  install(TARGETS SuperCollider
      RUNTIME DESTINATION "bin"
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
  install(FILES ${translations}
      DESTINATION "share/SuperCollider/translations")
endif()

if (LINUX)
  install( PROGRAMS SuperColliderIDE.desktop  DESTINATION "share/applications")
endif()
