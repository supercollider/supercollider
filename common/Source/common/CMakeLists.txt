add_library(sccommon SHARED
			SC_AllocPool.cpp
			SC_DirUtils.cpp
			SC_Sem.cpp
			SC_StandAloneInfo_Darwin.cpp
			SC_StringBuffer.cpp
			SC_StringParser.cpp
			scsynthsend.cpp
			sc_popen.cpp)

target_link_libraries(sccommon ${PTHREADS_LIBRARY})

if (APPLE)
	target_link_libraries(sccommon "-framework CoreServices")
endif()

if(CURL)
	if(CURL_FOUND)
		target_link_libraries(sccommon ${CURL_LIBRARIES})
	endif()
endif()


if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (../../cmake_modules/MacAppFolder.cmake)

	install(TARGETS sccommon
			DESTINATION ${scappauxresourcesdir}
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE OWNER_WRITE
			COMPONENT app)
else()
	install(TARGETS sccommon
			DESTINATION "lib"
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()
