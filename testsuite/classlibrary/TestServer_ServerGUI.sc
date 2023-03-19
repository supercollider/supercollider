// TestServer_ServerGUI.sc
// Author: Brian Heim (brianlheim@gmail.com)
// Tests for the GUI created by s.makeGui
// TODO: recording state tests?
TestServer_ServerGUI : UnitTest {

	// Note: If you are changing these, you probably mean to change `getButtons`!
	const bootButtonIdx = 0;
	const killButtonIdx = 1;
	const defaultButtonIdx = 2;
	const recordButtonIdx = 3;
	const volumeButtonIdx = 4;

	var server;
	var window;
	var buttons;
	var buttonNames;

	setUp {
		if(buttonNames.isNil) {
			buttonNames = Array.fill(5, "");
			buttonNames[bootButtonIdx] = "boot";
			buttonNames[killButtonIdx] = "kill";
			buttonNames[defaultButtonIdx] = "default";
			buttonNames[recordButtonIdx] = "record";
			buttonNames[volumeButtonIdx] = "volume";
		};

		server = Server(this.class.name);
		server.makeGui;
		window = server.window;
		buttons = this.getButtons(window);
	}

	tearDown {
		server.stopAliveThread; // XXX: needed to avoid alive threads started by server.makeGui
		server.remove;
		window.close;
		0.5.wait; // XXX: wait for window to execute its onClose
		window = buttons = server = nil;
	}

	// Puts the buttons from the window in the same order as the indices expected
	// by this class. If the order ever changes, it would be much easier to adjust
	// here than to edit the values in each test method.
	getButtons { |w|
		^w.view.children.select(_.isKindOf(QButton));
	}

	// Asserts states (values & enabled/disabled) of buttons. `testName` is the
	// name of the condition being tested (e.g. "Before booting")
	assertButtonStates { |expValues, expEnabled, testName|
		var actValues = buttons.collect(_.value);
		var actEnabled = buttons.collect(_.enabled);

		expValues.do { |val, i|
			this.assertEquals(actValues[i], expValues[i],
				"%: '%' button should have correct value".format(testName, buttonNames[i]));
			this.assert(actEnabled[i] == expEnabled[i],
				"%: '%' button should be %".format(
					testName, buttonNames[i], expEnabled[i].if("enabled", "disabled")));
		}
	}

	// test the GUI state without any action taken
	test_noBoot {
		var expValues = [0, 0, 0, 0, 0];
		var expEnabled = [true, true, true, false, true];
		this.assertButtonStates(expValues, expEnabled, "Before booting");
	}

	// GUI state after booting
	test_boot {
		var expValues = [1, 0, 0, 0, 0];
		var expEnabled = [true, true, true, true, true];
		this.bootServer(server);
		this.assertButtonStates(expValues, expEnabled, "After booting");
		server.quit;
	}

	// GUI state after booting and muting the server
	test_mute {
		var expValues = [1, 0, 0, 0, 1];
		var expEnabled = [true, true, true, true, true];
		this.bootServer(server);
		server.volume.mute;
		this.assertButtonStates(expValues, expEnabled, "After muting");
		server.volume.unmute;
		server.quit;
	}

	// GUI state after booting, muting, then unmuting the server
	test_unmute {
		var expValues = [1, 0, 0, 0, 0];
		var expEnabled = [true, true, true, true, true];
		this.bootServer(server);
		server.volume.mute;
		server.volume.unmute;
		this.assertButtonStates(expValues, expEnabled, "After muting and unmuting");
		server.quit;
	}

	// GUI state after booting then quitting
	test_quit {
		var expValues = [0, 0, 0, 0, 0];
		var expEnabled = [true, true, true, false, true];
		this.bootServer(server);
		server.quit;
		this.assertButtonStates(expValues, expEnabled, "After quitting");
	}

	// GUI state after setting default
	test_default {
		var expValues = [0, 0, 1, 0, 0];
		var expEnabled = [true, true, true, false, true];
		var oldDefault;

		oldDefault = Server.default;
		Server.default = server;

		this.assertButtonStates(expValues, expEnabled, "After defaulting this server");

		Server.default = oldDefault;
	}

	// GUI state after setting not default
	test_undefault {
		var expValues = [0, 0, 0, 0, 0];
		var expEnabled = [true, true, true, false, true];
		var oldDefault;

		oldDefault = Server.default;
		Server.default = server;
		Server.default = oldDefault;

		this.assertButtonStates(expValues, expEnabled, "After un-defaulting this server");
	}

}