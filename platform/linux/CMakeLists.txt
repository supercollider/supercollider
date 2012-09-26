install(DIRECTORY examples
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/SuperCollider )

install(FILES icons/supercollider.png icons/supercollider.xpm
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )

install(FILES ../../icons/sc_ide.svg
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )

install(FILES supercollider.xml
    DESTINATION share/mime/packages )
