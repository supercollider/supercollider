# WebAssembly

this file is document current work on getting scsynth to build for wasm.

Current config:

    $ emcmake cmake -DSC_WII=no -DSC_EL=no -DSUPERNOVA=no -DSC_HIDAPI=no -DNO_LIBSNDFILE=yes -DSC_QT=no -DNO_AVAHI=yes -DSC_ABLETON_LINK=no -DCMAKE_BUILD_TYPE="Debug" -Wno-dev -s USE_PTHREADS=1 -s WASM=1 -DPTHREADS_LIBRARY=ignore -DSSE=no -DSSE2=no -DNO_X11=yes --target scsynth ..

Current missing symbols:

 - `_Z17SC_NewAudioDriverP5World`
 - `_Z19initializeSchedulerv`
 - `_ZN13SC_Filesystem24defaultResourceDirectoryEv`
 - `_ZN13SC_Filesystem24defaultUserHomeDirectoryEv`
 - `_ZN13SC_Filesystem26defaultUserConfigDirectoryEv`
 - `_ZN13SC_Filesystem30defaultUserAppSupportDirectoryEv`
 - `_ZN13SC_Filesystem30isNonHostPlatformDirectoryNameERKNSt3__212basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEE`
 - `_ZN13SC_Filesystem32defaultSystemAppSupportDirectoryEv`
 - `_ZN13SC_Filesystem8freeGlobEPNS_4GlobE`
 - `_ZN13SC_Filesystem8globNextEPNS_4GlobE`
 - `_ZN13SC_Filesystem8makeGlobEPKc`
 - `oscTimeNow`
 - `pthread_setschedparam`
 - `sched_get_priority_max`
 - `sched_get_priority_min`
 - `server_timeseed`
 - `shmat`
 - `shmctl`
 - `shmdt`

