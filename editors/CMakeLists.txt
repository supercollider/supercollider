
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	option(SC_EL "Build emacs-based IDE." ON)
    option(SC_ED "Build gedit-based IDE." ON)
    option(SC_VIM "Build vim-based IDE." ON)
else()
	option(SC_EL "Build emacs-based IDE." OFF)
    option(SC_ED "Build gedit-based IDE." OFF)
    option(SC_VIM "Build vim-based IDE." OFF)
endif()


if(SC_EL)
	add_subdirectory(scel)
endif()

if(SC_ED)
	add_subdirectory(sced)
endif()

if(SC_VIM)
	add_subdirectory(scvim)
endif()
