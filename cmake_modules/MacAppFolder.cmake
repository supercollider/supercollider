if(APPLE)

	###### The user may set the app name, causing a standalone build
	######	cmake -D standalone:string=MyFabApp ../
	if(STANDALONE)
		if("${standalone}" STREQUAL "")
			set(standalone "SuperCollider-Standalone") # default standalone name
		endif()
		# We're building a standalone, change the app name.
		set(scappbundlename ${standalone})
		message(STATUS "Building sc in STANDALONE mode. App name: " ${standalone})
	else()
		set(scappbundlename ${PROJECT_NAME})
	endif()

	set(scappauxresourcesdir "${scappbundlename}/${scappbundlename}.app/Contents/Resources" CACHE STRING "Installation path for the Resourcedir")
	set(scappbindir "${scappbundlename}/${scappbundlename}.app/Contents/MacOS" CACHE STRING "Installation path for the Bindir")

	###### Allow user to select a FHS-style install
	# TODO not yet used
	option(INSTALL_FHS
		"use FHS-style install (e.g. to /usr/local) rather than to a mac-style app folder"
		FALSE)

endif(APPLE)
