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

	const kBufferSize = 8192;

	setUp {
		if(this.skipSerialTests) { ^this };
		#input, output = this.createPorts();
	}

	tearDown {
		if(this.skipSerialTests) { ^this };
		this.destroyPorts();
	}

	// Return true if we are on Windows or socat is not installed.
	// This method memoizes its results.
	// FIXME find better way to annotate/skip tests
	skipSerialTests {
		if(skipSerialTests.notNil) {
			^skipSerialTests;
		};
		skipSerialTests = false;
		if(thisProcess.platform.name == \windows) {
			"Skipping most SerialPort tests because platform is Windows.".warn;
			skipSerialTests = true;
		};
		if("which socat".systemCmd != 0) {
			"Skipping most SerialPort tests because socat is not installed.".warn;
			skipSerialTests = true;
		};
		^skipSerialTests;
	}

	// Create a pair of virtual serial ports and return their names
	createPorts {
		^this.createSocatPorts();
	}

	destroyPorts {
		"killall socat".unixCmdGetStdOut();
	}

	createSocatPorts {
		var cmd = "socat -d -d pty,raw,echo=0 pty,raw,echo=0 2>&1";
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
		if(this.skipSerialTests) { ^this };
		this.assertException({ SerialPort(input, crtscts: true, xonxoff: true) },
			PrimitiveFailedError,
			"Trying to open a serial port with both xonxoff and crtscts should throw");
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
		var in, out, cond, rxErrs;
		if(this.skipSerialTests) { ^this };

		in = this.mkPort(input);
		out = this.mkPort(output);
		cond = Condition();
		rxErrs = 0;

		// Overflow the buffer by exactly 1
		for(0, kBufferSize) { |i|
			out.put($a);
			0.0001.wait;
		};

		fork { 3.wait; cond.test_(true).signal };

		// spin until all data has been read
		while { (rxErrs == 0) and: cond.test.not } { rxErrs = in.rxErrors; 0.01.wait; };

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
