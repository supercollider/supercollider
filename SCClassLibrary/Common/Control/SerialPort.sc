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
		^(devicePattern ?? {
			thisProcess.platform.name.switch(
				\linux,   "/dev/tty[A,S,U]*",
				\osx,     "/dev/tty.*",
				\windows, "COM"
			)
		}).pathMatch
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
			allPorts.remove(this);
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
			{ this.prCleanup }.defer(0);
		}
	}
}
