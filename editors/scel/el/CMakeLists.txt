find_program(EMACS_EXECUTABLE emacs)
if(NOT EMACS_EXECUTABLE)
  message(SEND_ERROR "Emacs could not be found")
endif()

file(GLOB scel_sources
	 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.el)

set(PKG_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share/SuperCollider)

configure_file(sclang-vars.el.in
			   ${CMAKE_CURRENT_BINARY_DIR}/sclang-vars.el)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sclang-vars.elc
	COMMAND ${EMACS_EXECUTABLE} -batch -f batch-byte-compile
		${CMAKE_CURRENT_BINARY_DIR}/sclang-vars.el
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/sclang-vars.el
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Creating byte-compiled Emacs lisp ${CMAKE_CURRENT_BINARY_DIR}/sclang-vars.elc")

add_custom_target(sclang-vars.elc ALL
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/sclang-vars.elc)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sclang-vars.elc
	DESTINATION share/emacs/site-lisp)


foreach (el ${scel_sources})
	configure_file(${el} ${CMAKE_CURRENT_BINARY_DIR}/${el})
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${el}c
		COMMAND ${EMACS_EXECUTABLE} -batch -f batch-byte-compile
			${CMAKE_CURRENT_BINARY_DIR}/${el}
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${el}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Creating byte-compiled Emacs lisp ${CMAKE_CURRENT_BINARY_DIR}/${el}c")

	add_custom_target(${el}c ALL
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${el}c)

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${el}c
	DESTINATION share/emacs/site-lisp)
endforeach()
