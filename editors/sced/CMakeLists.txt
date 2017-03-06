set(SC_ED_VERSIONS 2 3)

list(FIND SC_ED_VERSIONS ${SC_ED} SC_ED_VERSION)

if(SC_ED_VERSION EQUAL -1)
    set(SC_ED 3)
endif()

if(NOT WIN32 AND SC_ED EQUAL 2)

    message(STATUS "Building with Sced for gedit 2 (UNIX)")

    install(FILES data/supercollider.lang
            DESTINATION share/gtksourceview-2.0/language-specs)

    install(FILES data/sced.gedit-plugin
            DESTINATION lib${LIB_SUFFIX}/gedit-2/plugins)

    install(DIRECTORY sced
            DESTINATION lib${LIB_SUFFIX}/gedit-2/plugins)

elseif(NOT WIN32 AND SC_ED EQUAL 3)

    message(STATUS "Building with Sced for gedit 3 (UNIX)")

    configure_file(sced3/supercollider.plugin.in sced3/supercollider.plugin)

    install(FILES data/supercollider.lang
            DESTINATION share/gtksourceview-3.0/language-specs)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sced3/supercollider.plugin sced3/supercollider.py
            DESTINATION lib${LIB_SUFFIX}/gedit/plugins)

elseif(WIN32)

    message(STATUS "Building with Sced for gedit 2 (Windows)")
    add_subdirectory(scedwin)

endif()
