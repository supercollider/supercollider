if(APPLE)

	###### The user may set the app name, causing a standalone build
	###### cmake -D standalone:string=MyFabApp ../
	###### TODO fix or find alternative
	if ("${standalone}" STREQUAL "")
		set(scappbundlename ${PROJECT_NAME})
	else()
		# We're building a standalone, change the app name.
		set(scappbundlename ${standalone})
		message(STATUS "Building sc in STANDALONE mode. App name: " ${standalone})
	endif()

	set(scappauxresourcesdir "${scappbundlename}/${scappbundlename}.app/Contents/Resources" CACHE STRING "Installation path for the Resourcedir")
	set(scappbindir "${scappbundlename}/${scappbundlename}.app/Contents/MacOS" CACHE STRING "Installation path for the Bindir")

endif(APPLE)
