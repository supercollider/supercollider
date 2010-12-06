# should be either OSC_HOST_BIG_ENDIAN or OSC_HOST_LITTLE_ENDIAN
# Apple Mac OS X: OSC_HOST_BIG_ENDIAN
# Win32: OSC_HOST_LITTLE_ENDIAN
# i386 GNU/Linux: OSC_HOST_LITTLE_ENDIAN
ENDIANESS=OSC_HOST_LITTLE_ENDIAN

CXX = g++
INCLUDES = -I./
COPTS  = -Wall -O3
CDEBUG = -Wall -g 
CXXFLAGS = $(COPTS) $(INCLUDES) -D$(ENDIANESS)

PREFIX = /usr/local
INSTALL = /usr/bin/install -c

#Name definitions
UNITTESTS=OscUnitTests
SEND=OscSendTests
RECEIVE=OscReceiveTest
DUMP=OscDump

INCLUDEDIR = oscpack
LIBNAME = liboscpack
LIBSONAME = $(LIBNAME).so
LIBFILENAME = $(LIBSONAME).1.0.2

#Test and example source
SENDSOURCES = ./tests/OscSendTests.cpp ./osc/OscOutboundPacketStream.cpp ./osc/OscTypes.cpp ./ip/posix/NetworkingUtils.cpp ./ip/posix/UdpSocket.cpp ./ip/IpEndpointName.cpp
SENDOBJECTS = $(SENDSOURCES:.cpp=.o)

RECEIVESOURCES = ./tests/OscReceiveTest.cpp ./osc/OscTypes.cpp ./osc/OscReceivedElements.cpp ./osc/OscPrintReceivedElements.cpp ./ip/posix/NetworkingUtils.cpp ./ip/posix/UdpSocket.cpp
RECEIVEOBJECTS = $(RECEIVESOURCES:.cpp=.o)

DUMPSOURCES = ./examples/OscDump.cpp ./osc/OscTypes.cpp ./osc/OscReceivedElements.cpp ./osc/OscPrintReceivedElements.cpp ./ip/posix/NetworkingUtils.cpp ./ip/posix/UdpSocket.cpp
DUMPOBJECTS = $(DUMPSOURCES:.cpp=.o)

UNITTESTSOURCES = ./tests/OscUnitTests.cpp ./osc/OscOutboundPacketStream.cpp ./osc/OscTypes.cpp ./osc/OscReceivedElements.cpp ./osc/OscPrintReceivedElements.cpp
UNITTESTOBJECTS = $(UNITTESTSOURCES:.cpp=.o)

#Library sources
LIBSOURCES = ./ip/IpEndpointName.cpp \
	./ip/posix/NetworkingUtils.cpp ./ip/posix/UdpSocket.cpp\
	./osc/OscOutboundPacketStream.cpp ./osc/OscPrintReceivedElements.cpp ./osc/OscReceivedElements.cpp ./osc/OscTypes.cpp
LIBOBJECTS = $(LIBSOURCES:.cpp=.o)

all:	unittests send receive dump

unittests : $(UNITTESTOBJECTS)
	@if [ ! -d bin ] ; then mkdir bin ; fi
	$(CXX) -o bin/$(UNITTESTS) $+ $(LIBS) 
send : $(SENDOBJECTS)
	@if [ ! -d bin ] ; then mkdir bin ; fi
	$(CXX) -o bin/$(SEND) $+ $(LIBS) 
receive : $(RECEIVEOBJECTS)
	@if [ ! -d bin ] ; then mkdir bin ; fi
	$(CXX) -o bin/$(RECEIVE) $+ $(LIBS) 
dump : $(DUMPOBJECTS)
	@if [ ! -d bin ] ; then mkdir bin ; fi
	$(CXX) -o bin/$(DUMP) $+ $(LIBS) 

clean:
	rm -rf bin $(UNITTESTOBJECTS) $(SENDOBJECTS) $(RECEIVEOBJECTS) $(DUMPOBJECTS) $(LIBOBJECTS) $(LIBFILENAME) include lib oscpack &> /dev/null

$(LIBFILENAME): $(LIBOBJECTS)
	@#GNU/Linux case
	$(CXX) -shared -Wl,-soname,$(LIBSONAME) -o $(LIBFILENAME) $(LIBOBJECTS) -lc
	@#Mac OS X case
	@#$(CXX) -dynamiclib -Wl,-install_name,$(LIBSONAME) -o $(LIBFILENAME) $(LIBOBJECTS) -lc
	
lib: $(LIBFILENAME)

#Installs the library on a system global location
install: lib
	@$(INSTALL) -m 755 $(LIBFILENAME) $(PREFIX)/lib/$(LIBFILENAME)
	@ln -s -f $(PREFIX)/lib/$(LIBFILENAME) $(PREFIX)/lib/$(LIBSONAME) 
	@mkdir  -p $(PREFIX)/include/oscpack/ip $(PREFIX)/include/oscpack/osc
	@$(INSTALL) -m 644 ip/*.h $(PREFIX)/include/oscpack/ip
	@$(INSTALL) -m 644 osc/*.h $(PREFIX)/include/oscpack/osc
	@echo "SUCCESS! oscpack has been installed in $(PREFIX)/lib and $(PREFIX)/include/ospack/"
	@echo "now doing ldconfig..."
	@ldconfig

#Installs the include/lib structure locally
install-local: lib
	@echo ""
	@echo " Installing in local directory <$(INCLUDEDIR)>"
	@echo "   > Creating symbolic link"
	@ln -s $(LIBFILENAME) $(LIBSONAME)
	@echo "   > Creating directories"
	@mkdir -p oscpack/lib
	@mkdir -p oscpack/include/ip
	@mkdir -p oscpack/include/osc
	@echo "   > Copying files"
	@mv $(LIBFILENAME) $(LIBSONAME) oscpack/lib
	@cp ip/*.h oscpack/include/ip
	@cp osc/*.h oscpack/include/osc
	@echo ""
	@echo "   > Success!"

