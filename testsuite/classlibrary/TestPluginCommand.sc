TestPluginCommand : UnitTest {

	asyncPluginCommand { arg serverName;
		var bus, cond = CondVar();
		var done = false;
		var value = 1.0, busValue = 0.0, nodeID = 900;
		var server = Server(thisMethod.name ++ "_" ++ serverName);

		this.bootServer(server);

		bus = Bus.control(server, 1);

		SynthDef(\number, { |bus|
			Out.kr(bus, value);
		}).add;

		server.sync;

		// listen for /done message
		OSCdef(\cmd_test, {
			"pluginCmdDemo done!".postln;
			done = true;
			cond.signalOne;
		}, '/done', nil, nil, [ \pluginCmdDemo ]);

		// 1. test plugin command success

		// send command
		server.sendMsg('/cmd', \pluginCmdDemo, 7, 9, \mno, ['/s_new', \number, nodeID, 0, 0, bus.index]);

		cond.waitFor(3, { done });

		this.assert(done, "%: async plugin command finished and sent /done message".format(serverName));

		bus.get { |f|
			busValue = f;
		};

		server.sync;

		server.sendMsg('/n_free', nodeID);

		this.assert(busValue == value, "%: async plugin command did perform completion message".format(serverName));

		// 2. test plugin command failure

		done = false;
		busValue = 0.0;
		bus.set(0.0);

		// send command ('fail' should cause the command to fail)
		server.sendMsg('/cmd', \pluginCmdDemo, 7, 9, \fail, ['/s_new', \number, 900, 0, 0, bus.index]);

		cond.waitFor(1, { done });

		this.assert(done.not, "%: failed async plugin command did not send /done message".format(serverName));

		bus.get { |f|
			busValue = f;
		};

		server.sync;

		this.assert(busValue == 0.0, "%: failed async plugin command did not perform completion message".format(serverName));

		if (server.serverRunning) {
			server.quit;
			// avoid UDP address in use errors when restarting the Server.
			// Unfortunately, there is no way to wait for the Server to quit...
			1.wait;
		};
	}

	test_asyncPluginCommandInScsynth {
		this.asyncPluginCommand("scsynth");
	}

	test_asyncPluginCommandInSupernova{
		Server.supernova;
		this.asyncPluginCommand("supernova");
		// restore scsynth
		Server.scsynth;
	}

}
