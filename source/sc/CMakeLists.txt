set(plugin_sources
    supercollider/common/Source/plugins/BinaryOpUGens.cpp
    supercollider/common/Source/plugins/ChaosUGens.cpp
    supercollider/common/Source/plugins/DelayUGens.cpp
    supercollider/common/Source/plugins/DemandUGens.cpp
    supercollider/common/Source/plugins/DynNoiseUGens.cpp
    supercollider/common/Source/plugins/FilterUGens.cpp
    supercollider/common/Source/plugins/GendynUGens.cpp
    supercollider/common/Source/plugins/GrainUGens.cpp
    supercollider/common/Source/plugins/IOUGens.cpp
    supercollider/common/Source/plugins/LFUGens.cpp
    supercollider/common/Source/plugins/MulAddUGens.cpp
    supercollider/common/Source/plugins/NoiseUGens.cpp
    supercollider/common/Source/plugins/OscUGens.cpp
    supercollider/common/Source/plugins/PanUGens.cpp
    supercollider/common/Source/plugins/PhysicalModelingUGens.cpp
    supercollider/common/Source/plugins/ReverbUGens.cpp
    supercollider/common/Source/plugins/TestUGens.cpp
    supercollider/common/Source/plugins/TriggerUGens.cpp
    supercollider/common/Source/plugins/UnaryOpUGens.cpp

    supercollider/common/Source/plugins/KeyboardUGens.cpp
    supercollider/common/Source/plugins/MouseUGens.cpp
)

set(plugins "")

set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-finite-math-only")

set_property(DIRECTORY
             APPEND
             PROPERTY COMPILE_DEFINITIONS JACK_BACKEND SUPERNOVA)

foreach(plugin ${plugin_sources})
    string(REPLACE .cpp "" plugin_name ${plugin} )
    string(REPLACE supercollider/common/Source/plugins/ "" plugin_name ${plugin_name} )
    add_library(${plugin_name} MODULE ${plugin})
    list(APPEND plugins ${plugin_name})
endforeach(plugin)


if (UNIX)
    target_link_libraries(MouseUGens X11)
    target_link_libraries(KeyboardUGens X11)
endif (UNIX)

set(scfft_sources
    supercollider/common/Source/common/SC_fftlib.cpp
    supercollider/common/Source/plugins/SCComplex.cpp)

add_library(FFT_UGens MODULE
            supercollider/common/Source/plugins/FFTInterfaceTable.cpp
            supercollider/common/Source/plugins/FFT_UGens.cpp
            supercollider/common/Source/plugins/PV_UGens.cpp
            supercollider/common/Source/plugins/PartitionedConvolution.cpp
            ${scfft_sources}
            )

add_library(PV_ThirdParty MODULE
            supercollider/common/Source/plugins/Convolution.cpp
            supercollider/common/Source/plugins/FFT2InterfaceTable.cpp
            supercollider/common/Source/plugins/FeatureDetection.cpp
            supercollider/common/Source/plugins/PV_ThirdParty.cpp
            ${scfft_sources}
            )

add_library(ML_UGens MODULE
            supercollider/common/Source/plugins/ML.cpp
            supercollider/common/Source/plugins/Loudness.cpp
            supercollider/common/Source/plugins/BeatTrack.cpp
            supercollider/common/Source/plugins/Onsets.cpp
            supercollider/common/Source/plugins/onsetsds.c
            supercollider/common/Source/plugins/KeyTrack.cpp
            supercollider/common/Source/plugins/MFCC.cpp
            supercollider/common/Source/plugins/BeatTrack2.cpp
            supercollider/common/Source/plugins/ML_SpecStats.cpp
            ${scfft_sources}
            )

add_library(DiskIO_UGens MODULE
            supercollider/common/Source/plugins/DiskIO_UGens.cpp
            supercollider/common/Source/server/SC_SyncCondition.cpp)

list(APPEND plugins FFT_UGens PV_ThirdParty ML_UGens DiskIO_UGens)

foreach(plugin ${plugins})
    install(TARGETS ${plugins}
            DESTINATION "libs/supernova/plugins"
            PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endforeach(plugin)

install(FILES supercollider/build/SCClassLibrary/Common/Control/PGroup.sc
        DESTINATION share/SuperCollider/SCClassLibrary/Common/Control)

add_subdirectory(sc3-plugins/build)
add_subdirectory(sc3-plugins/source)