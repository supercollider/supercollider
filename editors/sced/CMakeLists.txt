install(FILES data/supercollider.xml
		DESTINATION share/mime/packages)

# sced 2
install(FILES data/supercollider.lang
		DESTINATION share/gtksourceview-2.0/language-specs)

install(FILES data/supercollider.xml
		DESTINATION share/mime/packages)

install(FILES data/sced.gedit-plugin
		DESTINATION lib/gedit-2/plugins)

install(DIRECTORY sced
		DESTINATION lib/gedit-2/plugins)

# sced 3
install(FILES data/supercollider.lang
		DESTINATION share/gtksourceview-3.0/language-specs)

install(FILES sced3/supercollider.plugin sced3/supercollider.py
		DESTINATION lib/gedit/plugins)
