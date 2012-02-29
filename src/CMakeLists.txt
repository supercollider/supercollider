find_package(Qt4 REQUIRED)

set ( ide_moc_hdr
    widgets/main_window.hpp
    widgets/post_window.hpp
    sc_process.hpp
)

set ( ide_src
    main.cpp
    widgets/main_window.cpp
)

include( ${QT_USE_FILE} )

qt4_wrap_cpp( ide_moc_src ${ide_moc_hdr} )

add_executable( scide ${ide_src} ${ide_moc_src} )
target_link_libraries( scide ${QT_LIBRARIES} )
