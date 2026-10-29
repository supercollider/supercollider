if(NOT NO_LIBSNDFILE)
    find_package(Sndfile)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/include/common
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/include/plugin_interface
)

# large file support
if (CMAKE_SIZEOF_VOID_P MATCHES "4")
    execute_process(COMMAND getconf LFS_CFLAGS
        OUTPUT_VARIABLE LFS_CFLAGS
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif(CMAKE_SIZEOF_VOID_P MATCHES "8")
    execute_process(COMMAND getconf LFS64_CFLAGS
        OUTPUT_VARIABLE LFS_CFLAGS
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

add_compile_options(${LFS_CFLAGS})

set(plugins "")
set(supernova_plugins "")

set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_SHARED_MODULE_SUFFIX ".scx")

# If STATIC_PLUGINS is ON, we build the plugins as static libraries.
# We maintain a list of all plugins (`plugins` and `supernova_plugins`)
# and make them available to the parent scope (as `static_plugins` resp.
# `static_supernova_plugins`) so that scsynth/supernova can link them.
if(STATIC_PLUGINS)
    set(TYPE STATIC)
else()
    set(TYPE MODULE)
endif()

if (FFTW3F_FOUND)
    include_directories (${FFTW3F_INCLUDE_DIR})
endif()

# common properties shared by all scsynth plugins
add_library(plugin_common INTERFACE)
target_link_libraries(plugin_common INTERFACE Boost::headers)
if(NOVA_SIMD)
    target_include_directories(plugin_common INTERFACE ${CMAKE_SOURCE_DIR}/external_libraries/nova-simd)
    target_compile_definitions(plugin_common INTERFACE NOVA_SIMD)
endif()
if(STATIC_PLUGINS)
    target_compile_definitions(plugin_common INTERFACE STATIC_PLUGINS)
endif()

# common properties shared by all supernova plugins
add_library(supernova_plugin_common INTERFACE)
target_link_libraries(supernova_plugin_common INTERFACE plugin_common)
target_compile_definitions(supernova_plugin_common INTERFACE SUPERNOVA)

# additional common properties for FFT UGens
add_library(fft_plugin_common INTERFACE)
target_link_libraries(fft_plugin_common INTERFACE ${PTHREADS_LIBRARY} ${MATH_LIBRARY})
if(APPLE)
    target_link_libraries(fft_plugin_common INTERFACE "-framework Accelerate")
endif()
if (FFTW3F_FOUND)
    target_include_directories(fft_plugin_common INTERFACE ${FFTW3F_INCLUDE_DIR})
endif()

# core UGens

set(plugin_sources
    BinaryOpUGens.cpp
    ChaosUGens.cpp
    DelayUGens.cpp
    DemandUGens.cpp
    DemoUGens.cpp
    DynNoiseUGens.cpp
    FilterUGens.cpp
    GendynUGens.cpp
    GrainUGens.cpp
    IOUGens.cpp
    LFUGens.cpp
    MulAddUGens.cpp
    NoiseUGens.cpp
    OscUGens.cpp
    PanUGens.cpp
    PhysicalModelingUGens.cpp
    ReverbUGens.cpp
    TestUGens.cpp
    TriggerUGens.cpp
    UnaryOpUGens.cpp
    UnpackFFTUGens.cpp
)

foreach(plugin_src ${plugin_sources})
    string(REPLACE .cpp "" plugin_name ${plugin_src} )
    add_library(${plugin_name} ${TYPE} ${plugin_src})
    target_link_libraries(${plugin_name} PRIVATE plugin_common)
    list(APPEND plugins ${plugin_name})
    if(SUPERNOVA)
        add_library(${plugin_name}_supernova ${TYPE} ${plugin_src})
        target_link_libraries(${plugin_name}_supernova PRIVATE supernova_plugin_common)
        list(APPEND supernova_plugins ${plugin_name}_supernova)
    endif()
endforeach()

# BelaUGens

if(AUDIOAPI STREQUAL bela)
    # The Bela lib will have its own set of libraries and includes
    # However, the Bela audio backend for SC has some direct calls to Xenomai
    # Therefore we need to get flags for those as well
    find_package(Xenomai REQUIRED)
    find_package(Bela REQUIRED)
    message(STATUS "Found Bela libraries: ${BELA_LIBRARIES}")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # recommended compile flags for beaglebone etc; set here because bela api flag directly implies the architecture
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BELA_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BELA_CXX_FLAGS}")
    endif()
endif()

if(BELA_FOUND)
    add_library(BelaUGens ${TYPE} BelaUGens.cpp)
    target_compile_definitions(BelaUGens PRIVATE SC_BELA)
    target_compile_options(BelaUGens PRIVATE ${XENOMAI_DEFINITIONS} ${BELA_DEFINITIONS})
    include_directories(${XENOMAI_INCLUDE_DIRS} ${BELA_INCLUDE_DIRS})
    target_link_libraries(BelaUGens PRIVATE plugin_common ${XENOMAI_LIBRARIES} ${BELA_LIBRARIES})
    list(APPEND plugins BelaUGens)
    # NOTE: Bela plugins are not built for Supernova
endif()

# UIUGens

if(NOT NO_X11)
    if (APPLE)
        add_library(UIUGens ${TYPE} UIUGens.mm)
        target_link_libraries(UIUGens PRIVATE "-framework CoreServices -framework AppKit")
        if(SUPERNOVA)
            add_library(UIUGens_supernova ${TYPE} UIUGens.mm)
            target_link_libraries(UIUGens_supernova PRIVATE "-framework CoreServices -framework AppKit")
        endif()
    else()
        add_library(UIUGens ${TYPE} UIUGens.cpp)
        target_link_libraries(UIUGens PRIVATE ${PTHREADS_LIBRARY})

        if(SUPERNOVA)
            add_library(UIUGens_supernova ${TYPE} UIUGens.cpp)
            target_link_libraries(UIUGens_supernova PRIVATE ${PTHREADS_LIBRARY})
        endif()

        if (LINUX_OR_BSD)
            find_package(X11 REQUIRED)
            if (NOT X11_Xt_FOUND)
                message(FATAL_ERROR "libxt not found")
            endif()

            target_include_directories(UIUGens PRIVATE ${X11_Xt_INCLUDE_PATH})
            target_link_libraries(UIUGens PRIVATE ${X11_X11_LIB})

            if(SUPERNOVA)
                target_include_directories(UIUGens_supernova PRIVATE ${X11_Xt_INCLUDE_PATH})
                target_link_libraries(UIUGens_supernova PRIVATE ${X11_X11_LIB})
            endif()
        endif()
    endif()

    target_link_libraries(UIUGens PRIVATE plugin_common)
    list(APPEND plugins UIUGens)

    if(SUPERNOVA)
        target_link_libraries(UIUGens_supernova PRIVATE supernova_plugin_common)
        list(APPEND supernova_plugins UIUGens_supernova)
    endif()
endif()

# FFT_UGens

set(FFT_sources
        FFTInterfaceTable.cpp
        FFT_UGens.cpp
        PV_UGens.cpp
        PartitionedConvolution.cpp
)

add_library(FFT_UGens ${TYPE} ${FFT_sources})
target_link_libraries(FFT_UGens PRIVATE plugin_common fft_plugin_common)
list(APPEND plugins FFT_UGens)

if(SUPERNOVA)
    add_library(FFT_UGens_supernova ${TYPE} ${FFT_sources})
    target_link_libraries(FFT_UGens_supernova PRIVATE supernova_plugin_common fft_plugin_common)
    list(APPEND supernova_plugins FFT_UGens_supernova)
endif()

# PV_ThirdParty

set(PV_ThirdParty_sources
        Convolution.cpp
        FFT2InterfaceTable.cpp
        FeatureDetection.cpp
        PV_ThirdParty.cpp
)

add_library(PV_ThirdParty ${TYPE} ${PV_ThirdParty_sources})
target_link_libraries(PV_ThirdParty PRIVATE plugin_common fft_plugin_common)
list(APPEND plugins PV_ThirdParty)

if(SUPERNOVA)
    add_library(PV_ThirdParty_supernova ${TYPE} ${PV_ThirdParty_sources})
    target_link_libraries(PV_ThirdParty_supernova PRIVATE supernova_plugin_common fft_plugin_common)
    list(APPEND supernova_plugins PV_ThirdParty_supernova)
endif()

# ML_UGens

set(ML_sources
        ML.cpp
        Loudness.cpp
        BeatTrack.cpp
        Onsets.cpp
        onsetsds.c
        KeyTrack.cpp
        MFCC.cpp
        BeatTrack2.cpp
        ML_SpecStats.cpp
        BeatTrack.h
        BeatTrack2.h
        ML.h
        Onsets.h
        onsetsds.h
)

add_library(ML_UGens ${TYPE} ${ML_sources})
target_link_libraries(ML_UGens PRIVATE plugin_common fft_plugin_common)
list(APPEND plugins ML_UGens)

if(SUPERNOVA)
    add_library(ML_UGens_supernova ${TYPE} ${ML_sources})
    target_link_libraries(ML_UGens_supernova PRIVATE supernova_plugin_common fft_plugin_common)
    list(APPEND supernova_plugins ML_UGens_supernova)
endif()

# LinkUGen

if(SC_ABLETON_LINK)
    include(../../external_libraries/link/AbletonLinkConfig.cmake)

    add_library(Link_UGen ${TYPE} LinkUGen.cpp)
    target_link_libraries(Link_UGen PRIVATE plugin_common Ableton::Link)
    if(WIN32)
        target_link_libraries(Link_UGen PRIVATE ws2_32)
    endif()
    # bug-fix for undefined reference to `GetAdaptersAddresses'
    if (MINGW)
        target_link_libraries(Link_UGen PRIVATE iphlpapi)
    endif()
    list(APPEND plugins Link_UGen)

    if (SUPERNOVA)
        add_library(Link_UGen_supernova ${TYPE} LinkUGen.cpp)
        target_link_libraries(Link_UGen_supernova PRIVATE supernova_plugin_common Ableton::Link)
        if(WIN32)
            target_link_libraries(Link_UGen_supernova PRIVATE ws2_32)
        endif()
        # bug-fix for undefined reference to `GetAdaptersAddresses'
        if (MINGW)
            target_link_libraries(Link_UGen_supernova PRIVATE iphlpapi)
        endif()
        list(APPEND supernova_plugins Link_UGen_supernova)
    endif()
endif()

# DiskIO_UGens

if(NOT NO_LIBSNDFILE)
    if(SNDFILE_FOUND)
        set(sndfile_defines "")
        if(SNDFILE_HAS_VORBIS)
            list(APPEND sndfile_defines SNDFILE_HAS_VORBIS)
        endif()
        if(SNDFILE_HAS_OPUS)
            list(APPEND sndfile_defines SNDFILE_HAS_OPUS)
        endif()
        if(SNDFILE_HAS_MPEG)
            list(APPEND sndfile_defines SNDFILE_HAS_MPEG)
        endif()

        add_library(DiskIO_UGens ${TYPE} DiskIO_UGens.cpp)
        if (XENOMAI_FOUND)
            target_sources(DiskIO_UGens ../../common/XenomaiLock.cpp)
        endif()
        target_compile_definitions(DiskIO_UGens PRIVATE ${sndfile_defines})
        target_include_directories(DiskIO_UGens SYSTEM PRIVATE ${SNDFILE_INCLUDE_DIR})
        target_link_libraries(DiskIO_UGens PRIVATE plugin_common ${PTHREADS_LIBRARY} ${SNDFILE_LIBRARIES})
        if (APPLE)
            target_link_libraries(DiskIO_UGens PRIVATE "-framework CoreServices")
        endif()
        list(APPEND plugins DiskIO_UGens)

        if(SUPERNOVA)
            add_library(DiskIO_UGens_supernova ${TYPE} DiskIO_UGens.cpp)
            if (XENOMAI_FOUND)
                target_sources(DiskIO_UGens_supernova ../../common/XenomaiLock.cpp)
            endif()
            target_compile_definitions(DiskIO_UGens_supernova PRIVATE ${sndfile_defines})
            target_include_directories(DiskIO_UGens_supernova SYSTEM PRIVATE ${SNDFILE_INCLUDE_DIR})
            target_link_libraries(DiskIO_UGens_supernova PRIVATE supernova_plugin_common ${PTHREADS_LIBRARY} ${SNDFILE_LIBRARIES})
            if (APPLE)
                target_link_libraries(DiskIO_UGens_supernova PRIVATE "-framework CoreServices")
            endif()
            list(APPEND supernova_plugins DiskIO_UGens_supernova)
        endif()
    else()
        message(SEND_ERROR "Cannot find libsndfile")
    endif(SNDFILE_FOUND)
endif()

# install plugins

if(NOT STATIC_PLUGINS)
    if (WIN32)
        foreach(plugin ${plugins} ${supernova_plugins})
            add_custom_command(TARGET ${plugin}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:scsynth>/plugins
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:scsynth>/plugins
                    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:sclang>/plugins
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:sclang>/plugins
            )
        endforeach()

        if(SC_IDE)
            foreach(plugin ${plugins} ${supernova_plugins})
                add_custom_command(TARGET ${plugin}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SuperCollider>/plugins
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:SuperCollider>/plugins
                )
            endforeach()
        endif(SC_IDE)

        if(SUPERNOVA)
            foreach(plugin ${supernova_plugins})
                add_custom_command(TARGET ${plugin}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:supernova>/plugins
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:supernova>/plugins
                )
            endforeach()
        endif(SUPERNOVA)

        install(TARGETS ${plugins} ${supernova_plugins}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
                DESTINATION "${SC_WIN_BUNDLE_NAME}/plugins"
        )

    elseif(APPLE)
        if(scappauxresourcesdir)
            foreach(plugin ${plugins} ${supernova_plugins})
                add_custom_command(TARGET ${plugin} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/${scappauxresourcesdir}/plugins
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> ${CMAKE_INSTALL_PREFIX}/${scappauxresourcesdir}/plugins)
            endforeach()
        endif()

    else()
        install(TARGETS ${plugins} ${supernova_plugins}
                DESTINATION "lib${LIB_SUFFIX}/SuperCollider/plugins"
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    endif()
endif()

set_property(TARGET ${plugins} ${supernova_plugins} PROPERTY FOLDER UnitGenerators)

if(STATIC_PLUGINS)
    # pass the list of plugins to the parent directory so it can be picked up by scsynth and supernova.
    set(static_plugins ${plugins} PARENT_SCOPE)
    if (SUPERNOVA)
        set(static_supernova_plugins ${supernova_plugins} PARENT_SCOPE)
    endif()
endif()
