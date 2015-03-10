install(DIRECTORY examples
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/SuperCollider )

install(FILES ${CMAKE_SOURCE_DIR}/icons/supercollider.png ${CMAKE_SOURCE_DIR}/icons/supercollider.xpm
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )

install(FILES ${CMAKE_SOURCE_DIR}/icons/sc_ide.svg
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )

install(FILES ${CMAKE_SOURCE_DIR}/platform/linux/supercollider.xml
    DESTINATION share/mime/packages )
