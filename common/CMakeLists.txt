include_directories(${CMAKE_SOURCE_DIR}/include/common)

set(sccommon_sources SC_AllocPool.cpp
			SC_DirUtils.cpp
			SC_Sem.cpp
			SC_StandAloneInfo_Darwin.cpp
			SC_StringBuffer.cpp
			SC_StringParser.cpp
			scsynthsend.cpp
			sc_popen.cpp)


if (WIN32)
	set (sccommon_sources ${sccommon_sources} SC_Win32Utils.cpp)
endif()

add_library(sccommon SHARED ${sccommon_sources})

include_directories(${CMAKE_SOURCE_DIR}/include/common)

target_link_libraries(sccommon ${PTHREADS_LIBRARY})

if (APPLE)
	target_link_libraries(sccommon "-framework CoreServices")
endif()

if(CURL)
	if(CURL_FOUND)
		target_link_libraries(sccommon ${CURL_LIBRARIES})
	endif()
endif()

if(WIN32)
    target_link_libraries(sccommon wsock32)    
endif()

if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (${CMAKE_SOURCE_DIR}/cmake_modules/MacAppFolder.cmake)

	install(TARGETS sccommon
			DESTINATION ${scappauxresourcesdir}
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE OWNER_WRITE
			COMPONENT app)
else()
	install(TARGETS sccommon
			DESTINATION "lib"
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()
