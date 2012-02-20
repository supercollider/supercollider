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
  set(PORTMIDI_FOUND FALSE)
endif()

