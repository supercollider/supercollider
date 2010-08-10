file(GLOB scel_sources
	 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.sc)


install(FILES ${scel_sources}
		DESTINATION share/SuperCollider/Extensions/scide_scel)
