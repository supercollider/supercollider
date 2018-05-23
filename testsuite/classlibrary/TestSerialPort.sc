// Tests for SerialPort
// Brian Heim, 22 May 2018
//
// These tests perform integration testing on SerialPort by creating an input/output pair of virtual serial ports
// prior to each test.

TestSerialPort : UnitTest {

	var input;
	var output;

	const kBufferSize = 8192;

	setUp {
		#input, output = this.createPorts();
	}

	tearDown {
		this.destroyPorts();
	}

	// Create a pair of virtual serial ports and return their names
	createPorts {
		if(thisProcess.platform.name === \windows) {
			Error("Not yet implemented").throw;
		} {
			^this.createSocatPorts();
		};
	}

	destroyPorts {
		if(thisProcess.platform.name === \windows) {
			Error("Not yet implemented").throw;
		} {
			"killall socat".unixCmdGetStdOut();
		};
	}

	createSocatPorts {
		var cmd = "socat -d -d pty,raw,echo=0 pty,raw,echo=0 2>&1";
		var pipe = Pipe.new(cmd, "r");
		var first, second;

		first = pipe.getLine;
		first = first.findRegexp("/dev/ttys.*");
		if(first.isEmpty) {
			Error("socat is not installed");
		};

		first = first[0][1];

		second = pipe.getLine;
		second = second.findRegexp("/dev/ttys.*");
		if(second.isEmpty) {
			Error("couldn't read second tty from socat");
		};

		second = second[0][1];

		^[first, second]
	}

	// Sanitizes done action to avoid extra post
	mkPort { |dev ...args|
		var port = SerialPort(dev, *args);
		port.doneAction_({});
		^port
	}

	// ----------- tests -----------------------------------------------------------------------------------------

	test_open_onExistingDevice_defaultArgs {
		var port = this.mkPort(input);
		this.assert(port.isOpen);
		port.close();
	}

	test_open_onMissingDevice {
		try {
			SerialPort("/dev/doesnt_exist")
		} { |e|
			^this.passed(thisMethod);
		};

		this.failed(thisMethod, "Trying to open a missing serial port should throw");
	}

	test_close {
		var port = this.mkPort(input);
		port.close();
		this.assert(port.isOpen.not);
	}

	test_connectionLost {
		var in = this.mkPort(input);
		var cond = Condition();
		this.destroyPorts();

		fork { 3.wait; cond.test_(true) };
		while { in.isOpen and: cond.test.not } { 0.001.wait };

		this.assert(in.isOpen.not);
	}

	test_next_noData_producesNil {
		var port = this.mkPort(input);
		this.assertEquals(port.next, nil);
		port.close();
	}

	test_putAndNext_oneByte {
		var in = this.mkPort(input);
		var out = this.mkPort(output);
		var result = nil;
		var rout;
		var cond = Condition();

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

		in.close();
		out.close();
	}

	test_putAndNext_twoBytes {
		var in = this.mkPort(input);
		var out = this.mkPort(output);
		var result = nil;
		var rout;
		var cond = Condition();

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

		in.close();
		out.close();
	}

	test_putAndRead_oneByte {
		var in = this.mkPort(input);
		var out = this.mkPort(output);

		var rout;
		var cond = Condition();
		var result = nil;

		out.put($a);
		rout = fork { result = in.read; cond.test_(true).signal };
		fork { 3.wait; cond.test_(true).signal };

		cond.wait();
		this.assertEquals(result, $a.ascii);

		in.close();
		out.close();
	}

	test_rxErrors_noDataSent_isZero {
		var in = this.mkPort(input);
		this.assertEquals(in.rxErrors, 0);
		in.close();
	}

	test_rxErrors_bufferOverflow {
		var in = this.mkPort(input);
		var out = this.mkPort(output);
		var cond = Condition();
		var rxErrs = 0;

		// Overflow the buffer by exactly 1
		for(0, kBufferSize) { |i|
			out.put($a);
			0.000001.wait;
		};

		"meow".postln;
		fork { 3.wait; cond.test_(true).signal };

		// spin until all data has been read
		while { (rxErrs == 0) and: cond.test.not } { rxErrs = in.rxErrors; 0.01.wait; };

		this.assertEquals(rxErrs, 1);

		in.close();
		out.close();
	}

	test_putAll {
		var in = this.mkPort(input);
		var out = this.mkPort(output);
		var result = nil;
		var rout;
		var cond = Condition();

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

		in.close();
		out.close();
	}

	test_doneAction_onNormalClose {
		var in = this.mkPort(input);
		var ranAction = false;
		var cond = Condition();

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
		var in = this.mkPort(input);
		var ranAction = false;
		var cond = Condition();

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
