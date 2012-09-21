SerialPort
{
	classvar <>devicePattern, allPorts;
	var dataptr, semaphore;

	var <>doneAction;

	*initClass {
		allPorts = Array[];
		ShutDown.add {
			this.cleanupAll;
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

		^super.new.initSerialPort(
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
	initSerialPort { | ... args |
		semaphore = Semaphore(0);
		if ( dataptr.isNil ){
		  this.prOpen(*args);
		  allPorts = allPorts.add(this);
		  doneAction = { ("SerialPort"+args[0]+"was closed").postln; };
		}
	}

	isOpen {
		^dataptr.notNil
	}
	close {
		if (this.isOpen) {
			this.prClose;
			allPorts.remove(this);
		}
	}
	*closeAll {
		var ports = allPorts;
		allPorts = Array[];
		ports.do(_.close);
	}

	*cleanupAll {
		var ports = allPorts;
		allPorts = Array[];
		ports.do(_.primCleanup);
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
	}

	// always blocks
	put { | byte, timeout=0.005 |
		if ( dataptr.notNil ){
		    while { this.prPut(byte).not } {
			    timeout.wait;
			    timeout = timeout * 2;
		    }
		}{
		  "SerialPort not open".warn;
		}
	}
	putAll { | bytes, timeout=0.005 |
		bytes.do { |byte|
			this.put(byte, timeout);
		}
	}

	// PRIMITIVE
	prOpen { | ... args | // was: | port, baudRate | but that misses out on all the other args?
		_SerialPort_Open
		^this.primitiveFailed
	}
	prClose {
		_SerialPort_Close
		^this.primitiveFailed
	}
	primCleanup {
		_SerialPort_Cleanup
		^this.primitiveFailed
	}
	prCleanup{
		if (this.isOpen) {
			this.primCleanup;
			allPorts.remove(this);
		}
	}
	prPut { | byte |
		_SerialPort_Put
		^this.primitiveFailed
	}
	prDataAvailable {
		// callback
		semaphore.signal;
	}
	prDoneAction {
		// callback
		this.doneAction.value;
		// cleanup the port
		this.prCleanup
	}
}
