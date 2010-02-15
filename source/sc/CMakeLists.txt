add_subdirectory(supercollider/common/Source/plugins)

set_property(DIRECTORY supercollider/common/Source/plugins
             APPEND
             PROPERTY COMPILE_DEFINITIONS SUPERNOVA)

install(FILES supercollider/build/SCClassLibrary/Common/Control/PGroup.sc
        DESTINATION share/SuperCollider/SCClassLibrary/Common/Control)

add_subdirectory(sc3-plugins/build)
add_subdirectory(sc3-plugins/source)