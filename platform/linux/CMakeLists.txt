install(FILES icons/supercollider.xpm
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps )

install(FILES icons/supercollider.png
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps )

install(FILES ../../icons/sc_ide.svg
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps )

install(FILES supercollider.xml
    DESTINATION share/mime/packages )

install(FILES online.supercollider.SuperCollider.metainfo.xml
    DESTINATION share/metainfo )
