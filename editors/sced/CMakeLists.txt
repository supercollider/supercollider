install(FILES data/supercollider.lang
		DESTINATION share/gtksourceview-2.0/language-specs)

install(FILES data/supercollider.xml
		DESTINATION share/mime/packages)

install(FILES data/sced.gedit-plugin
		DESTINATION lib/gedit-2/plugins)

install(DIRECTORY sced
		DESTINATION lib/gedit-2/plugins)
