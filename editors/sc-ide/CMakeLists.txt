cmake_minimum_required( VERSION 2.8.11 )

find_package(Qt4 4.7 REQUIRED QtCore QtGui QtWebKit QtNetwork)

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
    core/session_manager.cpp
    core/util/standard_dirs.cpp
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
	list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_StandAloneInfo_Darwin.cpp)
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

include( ${QT_USE_FILE} )

qt4_wrap_cpp( ide_moc_src ${ide_moc_hdr} )
qt4_wrap_ui( ide_forms_src ${ide_forms} )
qt4_add_resources( ide_rcc resources.qrc )
qt4_add_translation( translations ${translation_src} ${native_translation_src} )

set(ide_sources ${ide_src} ${all_hdr} )

set(ide_rc_sources ${ide_moc_src} ${ide_forms_src} ${ide_rcc} ${translations})

if(APPLE)
    foreach(file ${translations})
        set_source_files_properties(${file} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/translations)
    endforeach()
endif()

# This sets up the exe icon for windows under mingw.
if(MINGW)
 set(RES_FILES ${CMAKE_SOURCE_DIR}/platform/windows/Resources/scide.rc)
 set(CMAKE_RC_COMPILER_INIT windres)
 ENABLE_LANGUAGE(RC)
 SET(CMAKE_RC_COMPILE_OBJECT
 "<CMAKE_RC_COMPILER> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)


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

target_link_libraries( SuperCollider ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${YAMLCPP_LIBRARY} )

if(Boost_FOUND)
    target_link_libraries( SuperCollider ${Boost_SYSTEM_LIBRARY} )
else()
    # in-house-built boost system
    target_link_libraries( SuperCollider boost_system )
endif()

if(APPLE)
    target_link_libraries( SuperCollider "-framework CoreServices -framework Foundation")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -F/Library/Frameworks")
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
    add_dependencies(SuperCollider sclang scsynth ${plugins} ${supernova_plugins})
    add_custom_command(TARGET SuperCollider POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sclang> $<TARGET_FILE_DIR:SuperCollider>/../Resources)

    add_custom_command(TARGET SuperCollider POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:scsynth> $<TARGET_FILE_DIR:SuperCollider>/../Resources)

    if(TARGET supernova)
        add_dependencies(SuperCollider supernova)
        add_custom_command(TARGET SuperCollider POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:supernova> $<TARGET_FILE_DIR:SuperCollider>/../Resources)
    endif()

    foreach(plugin ${plugins} ${supernova_plugins})
        add_custom_command(TARGET SuperCollider POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SuperCollider>/../Resources/plugins/
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:SuperCollider>/../Resources/plugins/)
    endforeach()


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

    include(DeployQt4)
    install_qt4_executable("${scappbundlename}/${scappbundlename}.app" "" "" "${CMAKE_BINARY_DIR}/server/scsynth;${CMAKE_BINARY_DIR}/external_libraries")

elseif(WIN32)
	install(TARGETS SuperCollider
			DESTINATION "SuperCollider"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    install(FILES ${translations}
            DESTINATION "SuperCollider/translations")
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
