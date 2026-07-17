TestUnitCommand : UnitTest {
	synchronousUnitCommand { arg serverName;
		var bus, cond = CondVar();
		var busValue = 0.0, synthIndex, u;
		var server = Server(thisMethod.name ++ "_" ++ serverName);

		this.bootServer(server);

		bus = Bus.control(server, 1);

		SynthDef(\u_cmd_test, { |bus|
			Out.kr(bus, UnitCmdDemo.kr);
		}).add;
		// UnitCmdDemo has index 1
		synthIndex = 1;

		server.sync;

		// 1. test queued unit command

		u = Synth(\u_cmd_test, [ bus: bus ], server);

		// send command
		server.sendMsg('/u_cmd', u.nodeID, synthIndex, \setValue, 4.0);

		server.sync; // give the UGen a chance to output the value

		bus.get { |f|
			busValue = f;
		};

		server.sync;

		this.assert(busValue == 4.0, "%: synchronous unit command (queued) works".format(serverName));

		// 2. test non-queued unit command

		// send command
		server.sendMsg('/u_cmd', u.nodeID, synthIndex, \setValue, -3.0);

		server.sync; // give the UGen a chance to output the value

		bus.get { |f|
			busValue = f;
		};

		server.sync;

		this.assert(busValue == -3.0, "%: synchronous unit command (non-queued) works".format(serverName));

		if (server.serverRunning) {
			server.quitSync;
		};
	}

	test_synchronousUnitCommandInScsynth {
		this.synchronousUnitCommand("scsynth");
	}

	test_synchronousUnitCommandInSupernova{
		Server.supernova;
		this.synchronousUnitCommand("supernova");
		// restore scsynth
		Server.scsynth;
	}

	asynchronousUnitCommand { arg serverName;
		var bus, cond = CondVar();
		var done = false;
		var value = 1.0, busValue = 0.0, nodeID = 900, synthIndex, u;
		var server = Server(thisMethod.name ++ "_" ++ serverName);

		this.bootServer(server);

		bus = Bus.control(server, 1);

		SynthDef(\number, { |bus|
			Out.kr(bus, value);
		}).add;

		SynthDef(\u_cmd_test, { |bus|
			Out.kr(bus, UnitCmdDemo.kr);
		}).add;
		// UnitCmdDemo has index 1
		synthIndex = 1;

		server.sync;

		// listen for /done message
		OSCdef(\u_cmd_test, {
			"testCommand done!".postln;
			done = true;
			cond.signalOne;
		}, '/done', nil, nil, [ \testCommand ]);

		// 1. test unit command success

		u = Synth(\u_cmd_test, nil, server);

		// send command
		server.sendMsg('/u_cmd', u.nodeID, synthIndex, \testCommand, 48000, 1.0, ['/s_new', \number, nodeID, 0, 0, bus.index]);

		cond.waitFor(3, { done });

		this.assert(done, "%: async unit command finished and sent /done message".format(serverName));

		bus.get { |f|
			busValue = f;
		};

		server.sync;

		server.sendMsg('/n_free', nodeID);

		this.assert(busValue == value, "%: async unit command did perform completion message".format(serverName));

		// 2. test unit command failure

		done = false;
		busValue = 0.0;
		bus.set(0.0);

		// send command (-1.0 should cause the command to fail)
		server.sendMsg('/u_cmd', u.nodeID, synthIndex, \testCommand, 48000, -1.0, ['/s_new', \number, nodeID, 0, 0, bus.index]);

		cond.waitFor(1, { done });

		this.assert(done.not, "%: failed async unit command did not send /done message".format(serverName));

		bus.get { |f|
			busValue = f;
		};

		server.sync;

		this.assert(busValue == 0.0, "%: failed async unit command did not perform completion message".format(serverName));

		// 3. make sure that the Server doesn't crash when the Synth is freed while an asynchronous unit command is still running.
		// Also, there should be no completion message and no /done message.
		done = false;
		busValue = 0.0;
		bus.set(0.0);

		server.sendMsg('/u_cmd', u.nodeID, synthIndex, \testCommand, 48000, 1.0, ['/s_new', \number, nodeID, 0, 0, bus.index]);
		u.free;

		cond.waitFor(1, { done });

		this.assert(done.not, "%: cancelled async unit command did not send /done message".format(serverName));

		bus.get { |f|
			busValue = f;
		};

		server.sync;

		this.assert(busValue == 0.0, "%: cancelled async unit command did not perform completion message".format(serverName));

		if (server.serverRunning) {
			server.quitSync;
		};
	}

	test_asynchronousUnitCommandInScsynth {
		this.asynchronousUnitCommand("scsynth");
	}

	test_asynchronousUnitCommandInSupernova{
		Server.supernova;
		this.asynchronousUnitCommand("supernova");
		// restore scsynth
		Server.scsynth;
	}
}
