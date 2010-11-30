if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (../cmake_modules/MacAppFolder.cmake)

	set(auxresourcesdir ${scappauxresourcesdir})

	# Some aux files which go in the app's folder
	install(FILES README ChangeLog
		DESTINATION ${auxresourcesdir}
	)
	install(FILES ../../COPYING
		DESTINATION ${scappdir}
	)
else()
	set(auxresourcesdir "share/SuperCollider")
endif()

install(DIRECTORY SCClassLibrary
	DESTINATION ${auxresourcesdir}
	PATTERN .svn EXCLUDE
	PATTERN .git EXCLUDE
)

install(DIRECTORY Help
	DESTINATION ${auxresourcesdir}
	PATTERN .svn EXCLUDE
	PATTERN .git EXCLUDE
)

install(DIRECTORY examples
	DESTINATION ${auxresourcesdir}
	PATTERN .svn EXCLUDE
	PATTERN .git EXCLUDE
)

install(DIRECTORY sounds
	DESTINATION ${auxresourcesdir}
	PATTERN .svn EXCLUDE
	PATTERN .git EXCLUDE
)
