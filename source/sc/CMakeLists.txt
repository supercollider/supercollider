set(plugin_sources
    supercollider/Source/plugins/BinaryOpUGens.cpp
    supercollider/Source/plugins/ChaosUGens.cpp
    supercollider/Source/plugins/DelayUGens.cpp
    supercollider/Source/plugins/DemandUGens.cpp
    supercollider/Source/plugins/DynNoiseUGens.cpp
    supercollider/Source/plugins/FilterUGens.cpp
    supercollider/Source/plugins/GendynUGens.cpp
    supercollider/Source/plugins/GrainUGens.cpp
    supercollider/Source/plugins/IOUGens.cpp
    supercollider/Source/plugins/LFUGens.cpp
    supercollider/Source/plugins/MulAddUGens.cpp
    supercollider/Source/plugins/NoiseUGens.cpp
    supercollider/Source/plugins/OscUGens.cpp
    supercollider/Source/plugins/PanUGens.cpp
    supercollider/Source/plugins/PhysicalModelingUGens.cpp
    supercollider/Source/plugins/ReverbUGens.cpp
    supercollider/Source/plugins/TestUGens.cpp
    supercollider/Source/plugins/TriggerUGens.cpp
    supercollider/Source/plugins/UnaryOpUGens.cpp

    supercollider/Source/plugins/KeyboardUGens.cpp
    supercollider/Source/plugins/MouseUGens.cpp
)

set(plugins "")

set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-finite-math-only")

set_property(DIRECTORY
             APPEND
             PROPERTY COMPILE_DEFINITIONS JACK_BACKEND SUPERNOVA)

foreach(plugin ${plugin_sources})
    string(REPLACE .cpp "" plugin_name ${plugin} )
    string(REPLACE supercollider/Source/plugins/ "" plugin_name ${plugin_name} )
    add_library(${plugin_name} MODULE ${plugin})
    list(APPEND plugins ${plugin_name})
endforeach(plugin)


if (UNIX)
    target_link_libraries(MouseUGens X11)
    target_link_libraries(KeyboardUGens X11)
endif (UNIX)

set(scfft_sources
    supercollider/Source/common/SC_fftlib.cpp
    supercollider/Source/plugins/SCComplex.cpp)

add_library(FFT_UGens MODULE
            supercollider/Source/plugins/FFTInterfaceTable.cpp
            supercollider/Source/plugins/FFT_UGens.cpp
            supercollider/Source/plugins/PV_UGens.cpp
            supercollider/Source/plugins/PartitionedConvolution.cpp
            ${scfft_sources}
            )

add_library(PV_ThirdParty MODULE
            supercollider/Source/plugins/Convolution.cpp
            supercollider/Source/plugins/FFT2InterfaceTable.cpp
            supercollider/Source/plugins/FeatureDetection.cpp
            supercollider/Source/plugins/PV_ThirdParty.cpp
            ${scfft_sources}
            )

add_library(ML_UGens MODULE
            supercollider/Source/plugins/ML.cpp
            supercollider/Source/plugins/Loudness.cpp
            supercollider/Source/plugins/BeatTrack.cpp
            supercollider/Source/plugins/Onsets.cpp
            supercollider/Source/plugins/onsetsds.c
            supercollider/Source/plugins/KeyTrack.cpp
            supercollider/Source/plugins/MFCC.cpp
            supercollider/Source/plugins/BeatTrack2.cpp
            supercollider/Source/plugins/ML_SpecStats.cpp
            ${scfft_sources}
            )

add_library(DiskIO_UGens MODULE
            supercollider/Source/plugins/DiskIO_UGens.cpp
            supercollider/Source/server/SC_SyncCondition.cpp)

list(APPEND plugins FFT_UGens PV_ThirdParty ML_UGens DiskIO_UGens)

foreach(plugin ${plugins})
    install(TARGETS ${plugins}
            DESTINATION "libs/supernova/plugins"
            PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endforeach(plugin)

install(FILES supercollider/build/SCClassLibrary/Common/Control/PGroup.sc
        DESTINATION share/SuperCollider/SCClassLibrary/Common/Control)
