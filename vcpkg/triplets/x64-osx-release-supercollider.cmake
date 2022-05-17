set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
if(PORT MATCHES "libsndfile|fftw|readline") # only specific libraries are build as shared
    set(VCPKG_LIBRARY_LINKAGE dynamic)
endif()


set(VCPKG_CMAKE_SYSTEM_NAME Darwin)
set(VCPKG_OSX_ARCHITECTURES x86_64)
set(VCPKG_BUILD_TYPE release)
