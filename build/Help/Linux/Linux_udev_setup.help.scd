In the file
/etc/udev/udev.rules
(on Debian at least; other distros probably the same, but may be different)

make sure there is the following:

KERNEL=="mice",                 NAME="input/%k",        GROUP="audio"
KERNEL=="mouse[0-9]*",          NAME="input/%k",        GROUP="audio"
KERNEL=="event[0-9]*",          NAME="input/%k",        GROUP="audio"
KERNEL=="js[0-9]*",             NAME="input/%k",        GROUP="audio"
KERNEL=="ts[0-9]*",             NAME="input/%k",        GROUP="audio"
KERNEL=="uinput",               NAME="input/%k",        GROUP="audio"

i.e. add GROUP="audio" to the lines, so that any user in the group "audio" can open the device.

