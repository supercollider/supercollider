find_package(Qt4 REQUIRED)

set ( ide_moc_hdr
    widgets/main_window.hpp
    widgets/post_window.hpp
    widgets/code_edit.hpp
    main.hpp
    sc_process.hpp
    doc_manager.hpp
)

set ( ide_src
    main.cpp
    doc_manager.cpp
    widgets/main_window.cpp
    widgets/code_edit.cpp
)

include( ${QT_USE_FILE} )

qt4_wrap_cpp( ide_moc_src ${ide_moc_hdr} )

add_executable( scide ${ide_src} ${ide_moc_src} )
target_link_libraries( scide ${QT_LIBRARIES} )
