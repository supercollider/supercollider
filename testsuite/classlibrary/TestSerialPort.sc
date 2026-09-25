// Tests for SerialPort
//
// These tests perform integration testing on SerialPort by creating an input/output pair of virtual serial ports
// prior to each test.
//
// To run these tests on macOS/Linux, you need the `socat` utility. To run on Windows, you need to install `com0com`
// and configure it for use with the CNCA0 and CNCB0 ports.

TestSerialPort : UnitTest {

	var skipSerialTests;
	var input;
	var output;
	var socatPath;
	var com0comPath;

	const kBufferSize = 8192;

	setUp {
		if(this.skipSerialTests) { ^this };
		#input, output = this.createPorts();
	}

	tearDown {
		if(this.skipSerialTests) { ^this };
		this.destroyPorts();
	}

	// Return if socat (macOS/Linux) or com0com (Windows) is not installed.
	// This method memorizes its results.
	// FIXME find better way to annotate/skip tests
	skipSerialTests {
		if(skipSerialTests.notNil) {
			^skipSerialTests;
		};
		skipSerialTests = false;
		if(thisProcess.platform.name == \windows) {
			com0comPath ?? { this.findCom0com };
			com0comPath ?? {
				"Skipping most SerialPort tests because com0com could not be found.".warn;
				skipSerialTests = true;
			}
		} {
			socatPath ?? { this.findSocat };
			socatPath ?? {
				"Skipping most SerialPort tests because socat could not be found.".warn;
				skipSerialTests = true;
			}
		};
		^skipSerialTests;
	}

	findCom0com {
		block {|break|
			[
				"C:\\Program Files\\com0com\\setupc.exe",
				"C:\\Program Files (x86)\\com0com\\setupc.exe",
				"C:\\Program Files (Arm)\\com0com\\setupc.exe"
			].do({|thisPath|
				if(File.exists(thisPath), {
					com0comPath = thisPath;
					break.();
				})
			})
		};
		com0comPath !? {com0comPath = thisProcess.platform.formatPathForCmdLine(com0comPath)}
	}

	findSocat {
		socatPath = "which socat".unixCmdGetStdOut.replace($\n);
		if(socatPath.size == 0, {socatPath = nil}); //reset to nil if it's an empty string
		socatPath ?? {
			block {|break|
				[
					"/usr/bin/socat",
					"/usr/local/bin/socat",
					"/opt/homebrew/bin/socat"
				].do({|thisPath|
					if(File.exists(thisPath), {
						socatPath = thisPath;
						break.();
					})
				})
			}
		};
		socatPath !? {socatPath = thisProcess.platform.formatPathForCmdLine(socatPath)}
	}

	// Create a pair of virtual serial ports and return their names
	createPorts {
		if(thisProcess.platform.name == \windows) {
			^this.getCom0ComPorts;
		} {
			^this.createSocatPorts;
		}
	}

	destroyPorts {
		if(thisProcess.platform.name != \windows) {
			"killall socat".unixCmdGetStdOut();
		};
	}

	getCom0ComPorts {
		var cmd = "% list".format(socatPath);
		var allPorts = cmd.unixCmdGetStdOut;
		var first, second;
		var getNameFromLine;

		if(allPorts.size == 0) {
			Error("No virtual ports available, create ports first").throw;
		};

		allPorts = allPorts.split($\n);

		getNameFromLine = {|line|
			var thisPort = line.findRegexp("PortName=([^,\r\n]+)")[1][1];
			if((thisPort == "-") || (thisPort == "COM#")) {thisPort = "\\\\.\\" ++ line.split($ ).first}; // use internal name if there's no alias;
			thisPort
		};

		first = allPorts[0].stripWhiteSpace;
		first = getNameFromLine.(first);
		if(first.isEmpty) {
			Error("Could not get the port name").throw;
		};

		second = allPorts[1].stripWhiteSpace;
		second = getNameFromLine.(second);
		if(second.isEmpty) {
			Error("Could not get the port name").throw;
		};

		"Using ports % and %\n".postf(first, second);

		^[first, second]
	}

	createSocatPorts {
		var cmd = "% -d -d pty,raw,echo=0 pty,raw,echo=0 2>&1".format(socatPath);
		var pipe = Pipe.new(cmd, "r");
		var first, second;

		first = pipe.getLine;
		first = first.findRegexp("/dev/(ttys|pts).*");
		if(first.isEmpty) {
			Error("socat is not installed").throw;
		};

		first = first[0][1];

		second = pipe.getLine;
		second = second.findRegexp("/dev/(ttys|pts).*");
		if(second.isEmpty) {
			Error("couldn't read second tty from socat").throw;
		};

		second = second[0][1];

		^[first, second]
	}

	// Sanitizes done action to avoid extra post
	// args should be an event with keyword-arg pairs for SerialPort.new, or nil
	mkPort { |dev, args = (Event())|
		^SerialPort.performWithEnvir(\new, args ++ (port: dev)).doneAction_({});
	}

	// ----------- tests -----------------------------------------------------------------------------------------

	test_devices {
		// used to hang forever on macOS (#4131)
		SerialPort.devices;
	}

	test_open_onExistingDevice_defaultArgs {
		var port;
		if(this.skipSerialTests) { ^this };

		port = this.mkPort(input);
		this.assert(port.isOpen);
		port.close();
	}

	// check that reasonable combinations of arguments are supported for opening ports
	test_open_onExistingDevice_variousSupportedArgs {
		var port;
		var argLists = [
			(crtscts: true),
			(xonxoff: true),
			(stopbit: false),
			(baudrate: 19200),
			(exclusive: true),
		];
		if(this.skipSerialTests) { ^this };

		argLists.do { |argList|
			var details = "Arguments: %".format(argList);
			this.assertNoException({ port = this.mkPort(input, argList) },
				"Opening serial port with reasonable arguments should not throw",
				details: details);
			this.assert(port.isOpen,
				"Opening serial port with reasonable arguments should succeed",
				details: details);
			port.close();
		};
	}

	test_open_errorOnExistingDevice_crtsctsAndXonxoffBothTrue {
		var port;
		if(this.skipSerialTests) { ^this };
		this.assertException({ port = SerialPort(input, crtscts: true, xonxoff: true) },
			PrimitiveFailedError,
			"Trying to open a serial port with both xonxoff and crtscts should throw");
		port !? { port.close() };
	}

	test_open_errorOnMissingDevice {
		if(this.skipSerialTests) { ^this };
		this.assertException( { SerialPort("/dev/doesnt_exist") }, PrimitiveFailedError, "Trying to open a missing serial port should throw");
	}

	test_close {
		var port;
		if(this.skipSerialTests) { ^this };

		port = this.mkPort(input);
		port.close();
		this.assert(port.isOpen.not);
	}

	test_connectionLost {
		var in, cond;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		cond = Condition();
		this.destroyPorts();

		fork { 3.wait; cond.test_(true) };
		while { in.isOpen or: cond.test.not } { 0.001.wait };

		this.assert(in.isOpen.not);
	}

	test_next_noData_producesNil {
		var port;
		if(this.skipSerialTests) { ^this };

		port = this.mkPort(input);
		this.assertEquals(port.next, nil);
		port.close();
	}

	test_putAndNext_oneByte {
		var in, out, result, rout, cond;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		out = this.mkPort(output);
		result = nil;
		cond = Condition();

		out.put($a);

		rout = fork {
			while { result.isNil } {
				result = in.next;
				0.01.wait
			};
			cond.test_(true).signal;
		};
		fork { 3.wait; cond.test_(true).signal };

		cond.wait();
		this.assertEquals(result, $a.ascii);
		rout.stop();

		in.close();
		out.close();
	}

	test_putAndNext_twoBytes {
		var in, out, result, rout, cond;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		out = this.mkPort(output);
		result = nil;
		cond = Condition();

		out.put($a);
		out.put($b);

		rout = fork {
			while { result.isNil } {
				result = in.next;
				0.01.wait
			};
			cond.test_(true).signal;
		};
		fork { 3.wait; cond.test_(true).signal };

		cond.wait();
		this.assertEquals(result, $a.ascii);
		this.assertEquals(in.next, $b.ascii);
		rout.stop();

		in.close();
		out.close();
	}

	test_putAndRead_oneByte {
		var in, out, rout, cond, result;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		out = this.mkPort(output);

		rout;
		cond = Condition();
		result = nil;

		out.put($a);
		rout = fork { result = in.read; cond.test_(true).signal };
		fork { 3.wait; cond.test_(true).signal };

		cond.wait();
		this.assertEquals(result, $a.ascii);
		rout.stop();

		in.close();
		out.close();
	}

	test_rxErrors_noDataSent_isZero {
		var in;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		this.assertEquals(in.rxErrors, 0);
		in.close();
	}

	test_rxErrors_bufferOverflow {
		var in, out, rxErrs;
		var written = 0, res, now, timeoutTime = 5;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		out = this.mkPort(output);
		rxErrs = 0;

		// Overflow the buffer by exactly 1
		for(0, kBufferSize) { |i|
			while { res = out.put($a); res == false } { // retry if writing failed
				"retrying send...".postln;
				0.001.wait;
			};
			if(res) {written = written + 1};
			0.0001.wait;
		};

		now = thisProcess.mainThread.seconds;

		// spin until all data has been read
		while { (rxErrs == 0) and: ((thisProcess.mainThread.seconds - now) < timeoutTime) } { rxErrs = in.rxErrors; 0.01.wait; };

		if((thisProcess.mainThread.seconds - now) > timeoutTime) { "TIMEOUT!".warn };
		// "Written bytes: %\nrxErrs: %\n".postf(written,rxErrs);

		this.assert(rxErrs > 0);

		in.close();
		out.close();
	}

	test_putAll {
		var in, out, result, rout, cond;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		out = this.mkPort(output);
		result = nil;
		rout;
		cond = Condition();

		out.putAll("Hello");

		rout = fork {
			while { result.isNil } {
				result = in.next;
				0.01.wait
			};
			cond.test_(true).signal;
		};
		fork { 3.wait; cond.test_(true).signal };

		cond.wait();
		this.assertEquals(result, $H.ascii);
		this.assertEquals(in.next, $e.ascii);
		this.assertEquals(in.next, $l.ascii);
		this.assertEquals(in.next, $l.ascii);
		this.assertEquals(in.next, $o.ascii);
		this.assertEquals(in.next, nil);
		rout.stop();

		in.close();
		out.close();
	}

	test_doneAction_onNormalClose {
		var in, ranAction, cond;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		ranAction = false;
		cond = Condition();

		in.doneAction_ {
			ranAction = true;
			cond.test_(true).signal;
		};
		fork { 3.wait; cond.test_(true).signal };

		in.close();
		cond.wait();

		this.assert(ranAction);
	}


	test_doneAction_onConnectionLost {
		var in, ranAction, cond;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		ranAction = false;
		cond = Condition();

		in.doneAction_ {
			ranAction = true;
			cond.test_(true).signal;
		};
		fork { 3.wait; cond.test_(true).signal };

		this.destroyPorts();
		cond.wait();

		this.assert(ranAction);
	}
}
