option(SC_EL_BYTECOMPILE "Build emacs-based IDE." ON)
mark_as_advanced(SC_EL_BYTECOMPILE)

file(GLOB scel_sources
	 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.el)

set(PKG_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share/SuperCollider)

configure_file(sclang-vars.el.in
			   ${CMAKE_CURRENT_BINARY_DIR}/sclang-vars.el)

foreach (el ${scel_sources})
	configure_file(${el} ${CMAKE_CURRENT_BINARY_DIR}/${el})
endforeach()

set(all_scel_sources ${scel_sources} sclang-vars.el)

if (SC_EL_BYTECOMPILE)
	find_program(EMACS_EXECUTABLE emacs)
	if(NOT EMACS_EXECUTABLE)
		message(SEND_ERROR "Emacs could not be found.\n (If emacs interface is not required, then set SC_EL=no)")
	endif()

	foreach (el ${all_scel_sources})
		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${el}c
			COMMAND ${EMACS_EXECUTABLE} -batch
				-L ${CMAKE_CURRENT_BINARY_DIR}
				-f batch-byte-compile
				${CMAKE_CURRENT_BINARY_DIR}/${el}
			DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${el}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Creating byte-compiled Emacs lisp ${CMAKE_CURRENT_BINARY_DIR}/${el}c")

		add_custom_target(${el}c ALL
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${el}c)

		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${el}c
		DESTINATION share/emacs/site-lisp/SuperCollider)
	endforeach()
else()
	install (FILES ${scel_sources} ${CMAKE_CURRENT_BINARY_DIR}/sclang-vars.el
			DESTINATION share/emacs/site-lisp/SuperCollider)
endif()
