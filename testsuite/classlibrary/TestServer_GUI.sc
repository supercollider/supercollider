// TestServer_GUI.sc
// Author: Brian Heim (brianlheim@gmail.com)
// Tests for GUI behavior during ServerBoot, ServerTree, ServerQuit
TestServer_GUI : UnitTest {

	var window, button, server;

	setUp {
		window = Window.new;
		button = Button(window);
		button.states_([["a"], ["b"]]);
		button.value_(0);

		server = Server(this.class.name);
	}

	tearDown {
		server.remove;

		window.close;
		window = button = server = nil;
	}

	// Since we have no programmatic way to tell when ServerBoot/ServerTree have finished,
	// we wait on a timeout.

	// Test that a GUI function executed by ServerBoot completes.
	// In other words, tests that ServerBoot functions run on an AppClock.
	test_serverBoot {
		var updateFunc;
		var cond = Condition();

		updateFunc = { button.value_(1); cond.test_(true).signal };
		ServerBoot.add(updateFunc, server);
		this.bootServer(server);

		fork { 3.wait; cond.test_(true).signal };
		cond.wait;

		this.assert(button.value == 1, "ServerBoot should be able to update GUI objects in the main function");

		ServerBoot.remove(updateFunc, server);
		server.quit;
	}

	// Test that a GUI function executed by ServerTree completes during boot.
	// Simulates behavior of FreqScope, etc.
	test_serverTreeDuringBoot {
		var updateFunc;
		var cond = Condition();

		updateFunc = { button.value_(1); cond.test_(true).signal };
		ServerTree.add(updateFunc, server);
		this.bootServer(server);

		fork { 3.wait; cond.test_(true).signal };
		cond.wait;

		this.assert(button.value == 1, "ServerTree should be able to update GUI objects in the main function during boot");

		ServerTree.remove(updateFunc, server);
		server.quit;
	}

	// Test that a GUI function executed by ServerTree completes during cmd-period.
	// Simulates behavior of FreqScope, etc.
	test_serverTreeDuringCmdPeriod {
		var updateFunc;
		var cond = Condition();

		this.bootServer(server);

		updateFunc = { button.value_(1); cond.test_(true).signal };
		ServerTree.add(updateFunc, server);

		CmdPeriod.run;

		fork { 3.wait; cond.test_(true).signal };
		cond.wait;

		this.assert(button.value == 1, "ServerTree should be able to update GUI objects in the main function during CmdPeriod");

		ServerTree.remove(updateFunc, server);
		server.quit;
	}

	// Test that a GUI function executed by ServerQuit completes.
	test_serverQuit {
		var updateFunc;
		var cond = Condition();

		updateFunc = { button.value_(1); cond.test_(true).signal };
		ServerQuit.add(updateFunc, server);

		server.bootSync;
		server.quit;

		fork { 3.wait; cond.test_(true).signal };
		cond.wait;

		this.assert(button.value == 1, "ServerQuit should be able to update GUI objects in the main function");

		ServerQuit.remove(updateFunc, server);
	}
}
