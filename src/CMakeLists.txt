
find_package(Qt4 REQUIRED)

set ( ide_src
    main.cpp
)

include( ${QT_USE_FILE} )

add_executable( scide ${ide_src} )
target_link_libraries( scide ${QT_LIBRARIES} )
