cmake_minimum_required( VERSION 2.8 )

project(sc-ide)

find_package(Qt4 REQUIRED)

set ( ide_moc_hdr
    core/main.hpp
    core/sig_mux.hpp
    core/sc_ipc.hpp
    core/sc_syntax_highlighter.hpp
    core/sc_process.hpp
    core/doc_manager.hpp
    widgets/main_window.hpp
    widgets/post_window.hpp
    widgets/code_edit.hpp
    widgets/multi_editor.hpp
    widgets/doc_list.hpp
    widgets/cmd_line.hpp
    widgets/settings/dialog.hpp
    widgets/settings/sclang_page.hpp
    widgets/settings/editor_page.hpp
    widgets/util/path_chooser_widget.hpp
    widgets/util/text_format_list_widget.hpp
)

set ( ide_src
    core/main.cpp
    core/sig_mux.cpp
    core/doc_manager.cpp
    core/sc_syntax_highlighter.cpp
    core/sc_process.cpp
    core/settings.cpp
    widgets/main_window.cpp
    widgets/code_edit.cpp
    widgets/multi_editor.cpp
    widgets/doc_list.cpp
    widgets/cmd_line.cpp
    widgets/settings/dialog.cpp
    widgets/settings/sclang_page.cpp
    widgets/settings/editor_page.cpp
    widgets/util/text_format_list_widget.cpp
)

set( ide_forms
    forms/settings_dialog.ui
    forms/settings_sclang.ui
    forms/settings_editor.ui
)

list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_DirUtils.cpp)

file(GLOB yaml_sources ${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp-0.3.0/src/*cpp)
list(APPEND ide_src ${yaml_sources})

if(APPLE)
	list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_StandAloneInfo_Darwin.cpp)
elseif(WIN32)
    list(APPEND ide_src ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/common)
include_directories(${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp-0.3.0/include)
include_directories(${CMAKE_SOURCE_DIR}/external_libraries/boost)
# Needed for auto-generated forms headers:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/widgets/util)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include( ${QT_USE_FILE} )

qt4_wrap_cpp( ide_moc_src ${ide_moc_hdr} )
qt4_wrap_ui( ide_forms_src ${ide_forms} )
qt4_add_resources( ide_rcc resources.qrc )

add_executable( scide ${ide_src} ${ide_moc_src} ${ide_forms_src} ${ide_rcc} )
target_link_libraries( scide ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARY} )

if(APPLE)
	target_link_libraries( scide "-framework CoreServices")
elseif(WIN32)
    target_link_libraries( scide wsock32 )
endif()
