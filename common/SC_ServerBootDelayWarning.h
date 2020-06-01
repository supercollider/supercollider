// Add a warning when Windows Defender delays scsynth boot by 60+ seconds
// cf. github issue #4368

#pragma once

// On Windows, starts a timer that will display a warning message
// after 10 seconds if the server has not booted,
// which may be due to Windows Defender.
// On other platforms, it does not do anything.
// Must be called at the start of the server boot sequence.
void startServerBootDelayWarningTimer();

// Must be called at the end of the server boot sequence.
void stopServerBootDelayWarningTimer();
