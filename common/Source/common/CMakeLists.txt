add_library(sccommon SHARED
			SC_AllocPool.cpp
			SC_DirUtils.cpp
			SC_Sem.cpp
			SC_StandAloneInfo_Darwin.cpp
			SC_StringBuffer.cpp
			SC_StringParser.cpp
			scsynthsend.cpp
			sc_popen.cpp)

include_directories(../../Headers/lang)

if (APPLE)
	target_link_libraries(sccommon "-framework CoreServices")
endif()

install(TARGETS sccommon
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
