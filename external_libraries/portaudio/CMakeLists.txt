# we put portaudio_submodule in a subdirectory in order to disable flags and definitions
unset(CMAKE_C_FLAGS)
unset(CMAKE_CXX_FLAGS)
if(WIN32)
    remove_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
endif()

set(PA_BUILD_STATIC ON CACHE BOOL "PortAudio static library" FORCE)
set(PA_BUILD_SHARED OFF CACHE BOOL "PortAudio shared library" FORCE)

# disable platform suffix
set(PA_LIBNAME_ADD_SUFFIX OFF CACHE BOOL "PortAudio static library suffix" FORCE)
# disable install target
set(PA_DISABLE_INSTALL ON CACHE BOOL "Disable PortAudio install" FORCE)

# disable obsolete APIs
if(WIN32)
    set(PA_USE_DS OFF CACHE BOOL "Enable support for DirectSound" FORCE)
    set(PA_USE_WMME OFF CACHE BOOL "Enable support for MME" FORCE)
    set(PA_USE_WDMKS OFF CACHE BOOL "Enable support for WDMKS" FORCE)

	# automatically download ASIO SDK if headers are not present
	# asio is expected in the subdirectory "external_libraries/portaudio/as*"
	# e.g. "external_libraries/portaudio/asiosdk"
	# with header file at "external_libraries/portaudio/asiosdk/common/asio.h"
	# FIXME: remove this after the portaudio submodule is updated above v19.7.0
	# as the newer PortAudio provides the asio sdk download functionality
	file(GLOB HEADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/as*/common/asio.h")
	if(NOT HEADER_FILE)
		if(CMAKE_VERSION VERSION_LESS 3.18)
			message(STATUS "ASIO SDK NOT found. Download the ASIO SDK ZIP from "
				"https://www.steinberg.net/asiosdk and extract its contents to "
				"${CMAKE_CURRENT_SOURCE_DIR}/asiosdk"
			)
		else()
			if(NOT ASIO_SDK_ZIP_PATH)
				set(ASIO_SDK_ZIP_PATH "${CMAKE_CURRENT_BINARY_DIR}/asiosdk.zip")
			endif()
			message(STATUS "Downloading ASIO SDK... ${ASIO_SDK_ZIP_PATH}")
			file(DOWNLOAD "https://www.steinberg.net/asiosdk"
				"${ASIO_SDK_ZIP_PATH}"
				STATUS ASIO_DOWNLOAD_STATUS
			)
			list(GET ASIO_DOWNLOAD_STATUS 0 DOWNLOAD_CODE)
			if(DOWNLOAD_CODE EQUAL 0)
				message(STATUS "ASIO SDK downloaded successfully")
			else()
				list(GET ASIO_DOWNLOAD_STATUS 1 DOWNLOAD_ERROR)
				message(STATUS "Error downloading ASIO SDK: ${DOWNLOAD_ERROR}")
			endif()
			if(EXISTS "${ASIO_SDK_ZIP_PATH}")
				message(STATUS "Extracting ASIO SDK ZIP archive: ${ASIO_SDK_ZIP_PATH}")
				file(ARCHIVE_EXTRACT INPUT "${ASIO_SDK_ZIP_PATH}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}")
			endif()
		endif()
	endif()
endif()

# bump up cmake version to fix compatibility with cmake 4
# FIXME: remove this after the portaudio submodule is updated above v19.7.0
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)

add_subdirectory(portaudio_submodule)

if(WIN32 AND NOT PA_USE_ASIO)
    message(STATUS "Building PortAudio WITHOUT support for ASIO")
endif()

# when building PA ourselves, we create an ALIAS target
add_library(PortAudio::PortAudio ALIAS portaudio_static)

