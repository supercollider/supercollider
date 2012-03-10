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
)

set ( ide_src
    core/main.cpp
    core/sig_mux.cpp
    core/doc_manager.cpp
    core/sc_syntax_highlighter.cpp
    widgets/main_window.cpp
    widgets/code_edit.cpp
    widgets/multi_editor.cpp
    widgets/doc_list.cpp
    widgets/cmd_line.cpp
)

include( ${QT_USE_FILE} )

qt4_wrap_cpp( ide_moc_src ${ide_moc_hdr} )

add_executable( scide ${ide_src} ${ide_moc_src} )
target_link_libraries( scide ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARY} )
