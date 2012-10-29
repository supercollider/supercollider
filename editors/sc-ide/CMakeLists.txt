cmake_minimum_required( VERSION 2.8 )

find_package(Qt4 4.7 REQUIRED QtCore QtGui QtWebKit)

if(${CMAKE_COMPILER_IS_GNUCXX})
    add_definitions(-Wreorder)
endif()

if (APPLE)
  include (${CMAKE_SOURCE_DIR}/cmake_modules/MacAppFolder.cmake)
  set(ide_name ${scappbundlename})
else()
  set(ide_name scide)
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
    widgets/util/text_format_list_widget.cpp
    widgets/util/popup_widget.cpp
    widgets/util/docklet.cpp
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

# Translation files
SET( translations
    translations/scide_de.ts
    translations/scide_es.ts
    translations/scide_fr.ts
    translations/scide_ja.ts
    translations/scide_pt.ts
    translations/scide_ru.ts
    translations/scide_sl.ts
    translations/scide_sv.ts
    translations/scide_zh.ts
)

# QtCollider shared sources:
list(APPEND ide_moc_hdr
    ${CMAKE_SOURCE_DIR}/QtCollider/layouts/stack_layout.hpp
)
list(APPEND ide_src
    ${CMAKE_SOURCE_DIR}/QtCollider/layouts/stack_layout.cpp
)

QT4_ADD_TRANSLATION( translations_src ${translations} )

list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_DirUtils.cpp)

if(APPLE)
	list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_StandAloneInfo_Darwin.cpp)
elseif(WIN32)
    list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/common)
include_directories(${CMAKE_SOURCE_DIR}/include/plugin_interface)
include_directories(${YAMLCPP_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external_libraries/boost)
# For QtCollider headers:
include_directories(${CMAKE_SOURCE_DIR})
# Needed for auto-generated forms headers:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets/util)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include( ${QT_USE_FILE} )

qt4_wrap_cpp( ide_moc_src ${ide_moc_hdr} )
qt4_wrap_ui( ide_forms_src ${ide_forms} )
qt4_add_resources( ide_rcc resources.qrc )

set(ide_sources ${ide_src} ${all_hdr} )

set(ide_rc_sources ${ide_moc_src} ${ide_forms_src} ${ide_rcc} ${translations_src})

if(FINAL_BUILD)
  CREATE_FINAL_FILE(scide_final.cpp ${ide_sources})
  add_executable( ${ide_name} MACOSX_BUNDLE scide_final.cpp ${ide_rc_sources} )
else()
  add_executable( ${ide_name} MACOSX_BUNDLE ${ide_sources} ${ide_rc_sources} )
endif()


target_link_libraries( ${ide_name} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${YAMLCPP_LIBRARY})

if(APPLE)
    target_link_libraries( ${ide_name} "-framework CoreServices")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -F/Library/Frameworks")
elseif(WIN32)
    target_link_libraries( ${ide_name} wsock32 )
    # The following prevents a Windows console from showing up
    # when the executable is started:
    set_target_properties( ${ide_name} PROPERTIES WIN32_EXECUTABLE TRUE )
endif()

if(PTHREADS_FOUND)
    target_link_libraries(${ide_name} ${PTHREADS_LIBRARIES})
endif()

if(LTO)
    set_property(TARGET ${ide_name}
                 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

    set_property(TARGET ${ide_name}
                 APPEND PROPERTY LINK_FLAGS "-flto -flto-report -fwhole-program")
endif()


# Installation

if(APPLE)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Install" CACHE PATH "Install path prefix" FORCE)
    endif()

    # set how it shows up in the Info.plist file
    SET(MACOSX_BUNDLE_ICON_FILE ../../platform/mac/SCcube.icns)

    install(DIRECTORY ../../platform/mac/Resources/ DESTINATION ${scappauxresourcesdir})

    install(TARGETS ${ide_name}
        DESTINATION ${scappbundlename}
        )

    # for processing into the plist:
    set_property(TARGET ${ide_name}
	    PROPERTY MACOSX_BUNDLE_BUNDLE_NAME ${scappbundlename})

    set(MACOSX_BUNDLE_BUNDLE_NAME "${scappbundlename}")

    include(InstallRequiredSystemLibraries)

    include(DeployQt4)
    install_qt4_executable("${scappbundlename}/${scappbundlename}.app" "" "" "${CMAKE_BINARY_DIR}/server/scsynth;${CMAKE_BINARY_DIR}/external_libraries")

elseif(WIN32)
	install(TARGETS ${ide_name}
			DESTINATION "SuperCollider"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
else()
	install(TARGETS ${ide_name}
			RUNTIME DESTINATION "bin"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if (LINUX)
   install( PROGRAMS SuperColliderIDE.desktop  DESTINATION "share/applications")
endif()
