if(APPLE)

	###### The user may set the app name, causing a standalone build
	######  cmake -D standalone:string=MyFabApp ../
	if ("${standalone}" STREQUAL "")
		set(scappbundlename ${PROJECT_NAME})
	else()
		# We're building a standalone, change the app name.
		set(scappbundlename ${standalone})
		message(STATUS "Building sc in STANDALONE mode. App name: " ${standalone})
	endif()
	
	# For standalone, many things go into the bundle rather than beside it
	if ("${standalone}" STREQUAL "")
		set(scappauxresourcesdir "${scappbundlename}")
	else()
		set(scappauxresourcesdir "${scappdir}/${scappbundlename}.app/Contents/Resources")
	endif()

	###### Allow user to select a FHS-style install
	# TODO not yet used
	option(INSTALL_FHS
		"use FHS-style install (e.g. to /usr/local) rather than to a mac-style app folder"
		FALSE)
	
endif(APPLE)
