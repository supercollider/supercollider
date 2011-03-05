find_package(PythonLibs)

set(pycxxdir ${CMAKE_SOURCE_DIR}/external_libraries/pycxx-6.2.2)

set(PySCLangSource
	${pycxxdir}/Src/cxx_extensions.cxx
	${pycxxdir}/Src/cxxextensions.c
	${pycxxdir}/Src/cxxsupport.cxx
	${pycxxdir}/Src/IndirectPythonInterface.cxx

	PySCLang_Module.cpp
	PySCLang_Module_GUIStuff.cpp
	PySCLang_vpost_stuff.cpp
)

if(WIN32)
	list(APPEND PySCLangSource PySCLang.cpp)
endif()

include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${pycxxdir})

include_directories(${CMAKE_SOURCE_DIR}/include/common
					${CMAKE_SOURCE_DIR}/include/lang
					${CMAKE_SOURCE_DIR}/include/plugin_interface
					${CMAKE_SOURCE_DIR}/editors/scapp/include
				   )

add_library(PySCLang ${PySCLangSource})
set_target_properties(PySCLang
					  PROPERTIES LINKER_LANGUAGE C)
