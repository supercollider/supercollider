In the file
/etc/udev/udev.rules
(on Debian /etc/udev/rules.d/50-udev.rules)
(on Ubuntu this should be in /etc/udev/rules.d/20-names.rules)

make sure there is the following:

KERNEL=="event[0-9]*",          NAME="input/%k",        GROUP="audio"

i.e. add GROUP="audio" to the lines, so that any user in the group "audio" can open the device.

