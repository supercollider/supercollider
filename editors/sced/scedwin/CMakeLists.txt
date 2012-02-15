set(PYTHON_INCLUDE_DIR "C:/Python26/include")
set(PYTHON_LIB_DIR "C:/Python26/libs")
#set(PYTHON_LIBRARY "C:/Python26/libs/python26.lib")
set(PYTHON_LIBRARY "C:/root/lib/python26.dll")

set( pipe_util_src ext/pipe_util.cpp )

include_directories( ${PYTHON_INCLUDE_DIR} )
#link_directories( "C:/mingw/lib/gcc/mingw32/4.6.2/" )

add_library(pipe_util MODULE ${pipe_util_src})
set_target_properties( pipe_util PROPERTIES
    #COMPILE_FLAGS "-specs=${CMAKE_CURRENT_SOURCE_DIR}/cppspecs -static-libstdc++ -static-libgcc"
    LINK_FLAGS "-specs=${CMAKE_CURRENT_SOURCE_DIR}/ext/cppspecs -static-libstdc++ -static-libgcc"
    PREFIX ""
    OUTPUT_NAME pipe_util
    SUFFIX ".pyd"
)

#set_target_properties( scedlib
#    PROPERTIES LINK_FLAGS "-LC:/mingw/lib/gcc/mingw32/4.6.2/" )

target_link_libraries( pipe_util ${PYTHON_LIBRARY} )

set( GEDIT_DIR gedit )
set( GEDIT_PLUGINS_DIR ${GEDIT_DIR}/lib/gedit-2/plugins )

install( TARGETS pipe_util DESTINATION ${GEDIT_PLUGINS_DIR}/sced )

install( DIRECTORY py/ DESTINATION ${GEDIT_PLUGINS_DIR}/sced )

install( DIRECTORY ${PROJECT_SOURCE_DIR}/external_libraries/simplejson-2.3.2/
    DESTINATION ${GEDIT_PLUGINS_DIR}/sced/simplejson )

install( FILES ../data/sced.gedit-plugin DESTINATION ${GEDIT_PLUGINS_DIR} )

install( FILES ../data/supercollider.lang
    DESTINATION ${GEDIT_DIR}/share/gtksourceview-2.0/language-specs )
