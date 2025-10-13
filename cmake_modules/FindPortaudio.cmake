# this should only be run on systems with pkgconfig installed
find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO_SYSTEM_LIB REQUIRED IMPORTED_TARGET portaudio-2.0)

# alias for using a consistent name throughout the codebase
add_library(PortAudio::PortAudio ALIAS PkgConfig::PORTAUDIO_SYSTEM_LIB)
