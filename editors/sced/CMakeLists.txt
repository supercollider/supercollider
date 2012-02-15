if(WIN32)
    if(NOT DEFINED SC_ED_WIN )
        set(SC_ED_WIN ON)
    endif()
else()
    if(NOT DEFINED SC_ED_2 AND NOT DEFINED SC_ED_3)
        set(SC_ED_3 ON)
    endif()
endif()

if(SC_ED_2)
    message(STATUS "Building Sced2")

    install(FILES data/supercollider.lang
            DESTINATION share/gtksourceview-2.0/language-specs)

    install(FILES data/supercollider.xml
            DESTINATION share/mime/packages)

    install(FILES data/sced.gedit-plugin
            DESTINATION lib/gedit-2/plugins)

    install(DIRECTORY sced
            DESTINATION lib/gedit-2/plugins)
endif()

if(SC_ED_3)
    message(STATUS "Building Sced3")

    configure_file(sced3/supercollider.plugin.in sced3/supercollider.plugin)

    install(FILES data/supercollider.lang
            DESTINATION share/gtksourceview-3.0/language-specs)

    install(FILES data/supercollider.xml
		DESTINATION share/mime/packages)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sced3/supercollider.plugin sced3/supercollider.py
            DESTINATION lib/gedit/plugins)
endif()

if(SC_ED_WIN)
    message(STATUS "Building Scedwin")
    add_subdirectory(scedwin)
endif()

