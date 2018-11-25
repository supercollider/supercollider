SerialPort {
	classvar <>devicePattern, allPorts;
	var dataptr, semaphore, <isOpen = false;

	var <>doneAction;

	*initClass {
		allPorts = Array[];
		ShutDown.add {
			this.closeAll;
		};
	}

	// device listing
	*devices {
		var regQueryResult, devices;
		var regexp;

		// Backward compatibility
		if(devicePattern.notNil) {
			^devicePattern.pathMatch;
		};

		if(thisProcess.platform.name == \windows) {
			// There are somewhere around a dozen ways you can get a list of serial port devices
			// on Windows. We here duplicate the method used in JSSC (Java Simple Serial Connector),
			// which is in turn used in the Arduino IDE. If it's good enough for Arduino, it's good
			// enough for us.

			// The registry key HKLM\HARDWARE\DEVICEMAP\SERIALCOMM can be queried using the below
			// "reg query" command. The next step is to parse its output, which looks like this
			// (note the use of 4 spaces rather than tabs):

			// HKEY_LOCAL_MACHINE\Hardware\DeviceMap\SerialComm
			//     \Device\Serial0    REG_SZ    COM3
			//     \Device\LSIModem5    REG_SZ    COM4
			//     \Device\USBSER000    REG_SZ    COM5

			// The regexp "\\bREG_SZ {4}(.*)$" matches our desired port names (COM3/COM4/COM5).

			regQueryResult = "reg query HKLM\\HARDWARE\\DEVICEMAP\\SERIALCOMM".unixCmdGetStdOut;
			devices = [];
			regQueryResult.split($\n).do { |line|
				var match;
				match = line.findRegexp("\\bREG_SZ {4}(.*)$");
				if(match.notEmpty) {
					devices = devices.add(match[1][1]);
				};
			};
			^devices;
		} {
			// These regexps are also taken from the Arduino IDE:
			// https://github.com/arduino/Arduino/blob/ec2e9a642a085b32701cf81297ee7c1570177195/arduino-core/src/processing/app/SerialPortList.java#L48
			regexp = thisProcess.platform.name.switch(
				\linux,   "^(ttyS|ttyUSB|ttyACM|ttyAMA|rfcomm|ttyO)[0-9]{1,3}",
				\osx,     "^(tty|cu)\\..*"
			);
			devices = [];
			PathName("/dev/").files.do { |path|
				if(regexp.matchRegexp(path.fileName)) {
					devices = devices.add(path.fullPath);
				};
			};
			^devices;
		};
	}
	*listDevices {
		this.devices.do(_.postln);
	}

	*new {
		| port,
			baudrate(9600),
			databits(8),
			stopbit(true),
			parity(nil),
			crtscts(false),
			xonxoff(false)
			exclusive(false) |

		if (port.isNumber) {
			"Using integers to identify serial ports is deprecated. Please pass strings instead.".warn;
			port = this.devices[port] ?? {
				Error("invalid port index").throw;
			};
		}

		^super.new.prInit(
			port,
			exclusive,
			baudrate,
			databits,
			stopbit,
			( even: 1, odd: 2 ).at(parity) ? 0,
			crtscts,
			xonxoff
		)
	}

	prInit { | ... args |
		semaphore = Semaphore(0);
		if ( isOpen.not ){
			this.prOpen(*args);
			allPorts = allPorts.add(this);
			isOpen = true;
		}
	}

	close {
		if (this.isOpen) {
			this.prClose;
			isOpen = false;
		}
	}

	*closeAll {
		var ports = allPorts;
		allPorts = Array[];
		ports.do(_.close);
	}

	*cleanupAll {
		this.deprecated(thisMethod, SerialPort.findMethod('closeAll'));
		this.closeAll
	}

	// non-blocking read
	next {
		_SerialPort_Next
		^this.primitiveFailed
	}
	// blocking read
	read {
		var byte;
		while { (byte = this.next).isNil } {
			semaphore.wait;
		};
		^byte
	}
	// rx errors since last query
	rxErrors {
		_SerialPort_RXErrors
		^this.primitiveFailed;
	}

	// always blocks
	put { | byte, timeout=0.005 |
		if (timeout != 0.005) {
			"SerialPort:-put: the timeout argument is deprecated and will be removed in a future release".warn
		};

		if (isOpen) {
			^this.prPut(byte);
		}{
			"SerialPort not open".warn;
			^false;
		}
	}

	putAll { | bytes, timeout=0.005 |
		if (timeout != 0.005) {
			"SerialPort:-putAll: the timeout argument is deprecated and will be removed in a future release".warn
		};

		bytes.do { |byte|
			this.put(byte);
		}
	}

	// PRIMITIVE
	prOpen { | ... args |
		_SerialPort_Open
		^this.primitiveFailed
	}

	// Close the port; triggers doneAction
	prClose {
		_SerialPort_Close
		^this.primitiveFailed
	}

	prPut { | byte |
		_SerialPort_Put
		^this.primitiveFailed
	}

	// Deletes the C++ object for this port.
	prCleanup {
		_SerialPort_Cleanup
		^this.primitiveFailed
	}

	// callback from C++ read thread
	prDataAvailable {
		semaphore.signal;
	}

	// callback from C++ read thread when done
	prDoneAction {
		// Catches case where connection is closed unexpectedly.
		isOpen = false;

		// Ensure memory is freed even if doneAction throws.
		protect {
			this.doneAction.value
		} {
			// Needs to run after this callback; otherwise crash when
			// we try to wait for this thread to join, from itself.
			// Remove reference as last act, otherwise we risk GC running early.
			{ this.prCleanup; allPorts.remove(this) }.defer(0);
		}
	}
}
