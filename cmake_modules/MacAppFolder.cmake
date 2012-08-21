if(APPLE)

	###### The user may set the app name, causing a standalone build
	######	cmake -D standalone:string=MyFabApp ../
	if ("${standalone}" STREQUAL "")
		if (SC_IDE)
			set(scappbundlename ${PROJECT_NAME})
	        set(scappauxresourcesdir  "${scappbundlename}/${scappbundlename}.app/Contents/Resources")
	        set(sclangauxresourcesdir "${scappbundlename}/${scappbundlename}.app/Contents/Resources/sclang.app/Contents")
		else()
			set(scappbundlename ${PROJECT_NAME}Cocoa)
	        set(scappauxresourcesdir  "${scappbundlename}/${scappbundlename}.app/Contents/Resources")
	        set(sclangauxresourcesdir "${scappbundlename}/${scappbundlename}.app/Contents/Resources")
		endif()
	else()
		# We're building a standalone, change the app name.
		set(scappbundlename ${standalone})
		message(STATUS "Building sc in STANDALONE mode. App name: " ${standalone})
	    set(scappauxresourcesdir  "${scappbundlename}/${scappbundlename}.app/Contents/Resources")
	    set(sclangauxresourcesdir "${scappbundlename}/${scappbundlename}.app/Contents/Resources")
	endif()

	###### Allow user to select a FHS-style install
	# TODO not yet used
	option(INSTALL_FHS
		"use FHS-style install (e.g. to /usr/local) rather than to a mac-style app folder"
		FALSE)

endif(APPLE)
