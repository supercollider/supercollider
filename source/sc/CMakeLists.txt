add_subdirectory(supercollider/common/Source/plugins)

set_property(DIRECTORY supercollider/common/Source/plugins
             APPEND
             PROPERTY COMPILE_DEFINITIONS SUPERNOVA)

add_subdirectory(sc3-plugins/build)
add_subdirectory(sc3-plugins/source)

set_property(DIRECTORY sc3-plugins/source
             APPEND
             PROPERTY COMPILE_DEFINITIONS SUPERNOVA)
