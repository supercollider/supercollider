# Try to find Bluetooth
#  BLUETOOTH_FOUND - If false, do not try to use Bluetooth.
#  BLUETOOTH_INCLUDE_DIRS - where to find bluetooth/bluetooth.h
#  BLUETOOTH_LIBRARIES - the libraries to link against


if (BLUETOOTH_LIBRARIES AND BLUETOOTH_INCLUDE_DIRS)
	# path set by user or was found in the past
	set(BLUETOOTH_FOUND TRUE)
else (BLUETOOTH_LIBRARIES AND BLUETOOTH_INCLUDE_DIRS)
	find_path(BLUETOOTH_INCLUDE_DIRS
		NAMES bluetooth/bluetooth.h
	)

	find_library(BLUETOOTH_LIBRARIES
		NAMES bluetooth
	)

	if (BLUETOOTH_INCLUDE_DIRS AND BLUETOOTH_LIBRARIES)
		set(BLUETOOTH_FOUND TRUE)
	endif (BLUETOOTH_INCLUDE_DIRS AND BLUETOOTH_LIBRARIES)

	if (BLUETOOTH_FOUND)
		if (NOT BLUETOOTH_FIND_QUIETLY)
			message(STATUS "Found Bluetooth: ${BLUETOOTH_LIBRARIES}")
		endif (NOT BLUETOOTH_FIND_QUIETLY)
	else (BLUETOOTH_FOUND)
		if (BLUETOOTH_FIND_REQUIRED)
			message(FATAL_ERROR "Could not find Bluetooth")
		endif (BLUETOOTH_FIND_REQUIRED)
	endif (BLUETOOTH_FOUND)

	# set visibility in cache
	mark_as_advanced(BLUETOOTH_INCLUDE_DIRS BLUETOOTH_LIBRARIES)

endif (BLUETOOTH_LIBRARIES AND BLUETOOTH_INCLUDE_DIRS)
