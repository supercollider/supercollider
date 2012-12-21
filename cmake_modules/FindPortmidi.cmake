# - Try to find PortMidi
# Once done this will define
#
#  PORTMIDI_FOUND - system has Portaudio
#  PORTMIDI_INCLUDE_DIRS - the Portaudio include directory
#  PORTMIDI_LIBRARIES - the Portaudio libraries
#

if(PORTMIDI_INCLUDE_DIRS AND PORTMIDI_LIBRARIES)
    set(PORTMIDI_FOUND TRUE)
else()
    # Right now we only use PortMidi on Windows.
    # When needed, implement finding it on other platforms.

    find_path(PORTMIDI_COMON_INCLUDE_DIR NAMES portmidi.h )
    find_path(PORTMIDI_TIME_INCLUDE_DIR NAMES porttime.h )
    find_library(PORTMIDI_LIBRARY NAMES portmidi )

    if( PORTMIDI_COMON_INCLUDE_DIR AND
        PORTMIDI_TIME_INCLUDE_DIR AND
        PORTMIDI_LIBRARY )
        set(PORTMIDI_FOUND TRUE)
        set(PORTMIDI_INCLUDE_DIRS ${PORTMIDI_COMON_INCLUDE_DIR} ${PORTMIDI_TIME_INCLUDE_DIR})
        set(PORTMIDI_LIBRARIES ${PORTMIDI_LIBRARY})
        message(STATUS "Found Portmidi: ${PORTMIDI_INCLUDE_DIRS} ${PORTMIDI_LIBRARIES}")
    else()
        set(PORTMIDI_FOUND FALSE)
    endif()

endif()
