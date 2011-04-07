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

add_library(PySCLang MODULE ${PySCLangSource})
set_target_properties(PySCLang
					  PROPERTIES LINKER_LANGUAGE C
								 PREFIX ""
								 SUFFIX .pyd)

target_link_libraries(PySCLang ${PYTHON_LIBRARY})
target_link_libraries(PySCLang libsclang)

if (WIN32)
	install(TARGETS PySCLang
		DESTINATION "SuperCollider")

	install(FILES ${PYTHON_LIBRARY}
		DESTINATION "SuperCollider")

elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	find_package(PythonInterp)

	execute_process ( COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
		OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)

	install(TARGETS PySCLang DESTINATION ${PYTHON_SITE_PACKAGES})
endif()
