oscpack -- Open Sound Control packet manipulation library
http://www.audiomulch.com/~rossb/code/oscpack

Copyright (c) 2004-2005 Ross Bencina <rossb@audiomulch.com>

A simple C++ library for packing and unpacking OSC packets.


Oscpack is simply a set of C++ classes for packing and unpacking OSC packets. 
Oscpack includes a minimal set of UDP networking classes for windows and posix 
which are sufficient for writing many OSC applications and servers, but you are 
encouraged to use another networking framework if it better suits your needs. 
Oscpack is not an OSC application framework, it doesn't include infrastructure for 
constructing or routing OSC namespaces, just classes for easily constructing, 
sending, receiving and parsing OSC packets. The library should also be easy to use 
for other transport methods (eg serial).

The key goals of the oscpack library are:

	- to be a simple and complete implementation of OSC
	- to be portable to a wide variety of platforms
	- to allow easy development of robust OSC applications 
		(for example it should be impossible to crash a server
		 by sending it malformed packets, and difficult to 
		 create malformed packets.)

Here's a summary of the key files:

osc/OscReceivedElements -- classes for parsing a packet
osc/OscPrintRecievedElements -- iostream << operators for printing packet elements
osc/OscOutboundPacket -- a class for packing messages into a packet
osc/OscPacketListener -- base class for listening to OSC packets on a UdpSocket
tests/OscUnitTests -- unit test program for the OSC modules
tests/OscSendTests -- examples of how to send messages
tests/OscReceiveTest -- example of how to receive the messages sent by OSCSendTests
examples/OscDump -- a program that prints received OSC packets



Building
--------

In general the idea is that you will embed this source code in your projects as you 
see fit. The Makefile has an install rule for building a shared library and 
installing headers in usr/local.

The Makefile works for Linux and MaxOS X except that if you are on a big endian 
machine such as PowerPC Macintosh you need to edit the line which sets the 
endianness to OSC_HOST_BIG_ENDIAN (see the makefile comment for details) or it won't 
work. If you want to build and install liboscpack as a library on OS X you also need 
to edit the $(LIBFILENAME) rule by commenting out the Linux case and uncommenting 
the OS X case since OS X uses different gcc flags for shared libraries.

On Windows there is a batch file for doing a simple test build with MinGW gcc called 
make.MinGW32.bat. This will build the test executables and oscdump in ./bin and run 
the unit tests.

--


If you fix anything or write a set of TCP send/recieve classes 
please consider sending me a patch. Thanks :)

For more information about Open Sound Control, see:
http://www.cnmat.berkeley.edu/OpenSoundControl/


Thanks to Till Bovermann for helping with POSIX networking code and
Mac compatibility, and to Martin Kaltenbrunner and the rest of the
reacTable team for giving me a reason to finish this library. Thanks
to Merlijn Blaauw for reviewing the interfaces. Thanks to Xavier Oliver
for additional help with Linux builds and POSIX implementation details.

Portions developed at the Music Technology Group, Audiovisual Institute, 
University Pompeu Fabra, Barcelona, during my stay as a visiting
researcher, November 2004 - September 2005. 

See the file LICENSE for information about distributing and using this code.


