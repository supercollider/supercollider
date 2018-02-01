// see also TestServer_clientID
/*
TestServer_clientID_booted.run;
*/

// with running server
TestServer_clientID_booted : UnitTest {

	test_default {
		var s = Server(this.class.name);
		this.bootServer(s);
		this.assertEquals(s.clientID, 0, "clientID should be 0 by default");
		s.quit;
		s.remove;
	}

	test_setWithSetter {
		var options = ServerOptions.new.maxLogins_(4);
		var s = Server(this.class.name, nil, options);
		s.clientID = 3;

		this.bootServer(s);
		this.assertEquals(s.clientID, 3, "clientID should be settable by clientID_ before boot.");
		s.quit;
		s.remove;
	}

	test_setWithConstructor {
		var options = ServerOptions.new.maxLogins_(4);
		var s = Server(this.class.name, nil, options, 3);

		this.bootServer(s);
		this.assertEquals(s.clientID, 3, "clientID should be settable by Server constructor.");
		s.quit;
		s.remove;
	}

	// TODO: find a better home for this test
	test_nodeIDAlloc_ServerTree {
		var s = Server(this.class.name);
		var synth1, synth2;
		var func = { synth1 = Synth("default"); };

		ServerTree.add(func, s);
		this.bootServer(s);

		synth2 = Synth("default", [\freq, 330]);
		this.assert(synth1.nodeID != synth2.nodeID,
			"first nodeID after booting should not repeat nodeID created in ServerTree.");

		synth1.free;
		synth2.free;
		s.quit;
		ServerTree.remove(func, s);
		s.remove;
	}

	test_lockedWhileRunning {
		var options = ServerOptions.new.maxLogins_(4);
		var s = Server(this.class.name, nil, options, 1);
		var lockedID;

		this.bootServer(s);
		lockedID = s.clientID;

		s.clientID = 3;
		this.assertEquals(s.clientID, lockedID,
			"clientID should be locked while server is running.");

		s.quit;
		s.remove;
	}

	test_recoverRemoteReLogin {
		var options, homeServer, remote;

		options = ServerOptions.new;
		options.maxLogins = 4;
		options.sampleRate = 48000;

		homeServer = Server(\pseudoHome, options: options);

		homeServer.clientID = 3;
		this.bootServer(homeServer);

		this.assert(homeServer.clientID == 3,
			"homeServer gets requested clientID back from server process."
		);

		// make s play dead now, but leave the process running
		homeServer.statusWatcher.stopStatusWatcher.stopAliveThread.serverRunning_(false);
		// homeServer.serverRunning.postln; // client thinks it is dead

		// now login again from different server object, but same client address
		// -> this client netaddr is already registered, and should say so!
		// -> so the response should go thru prHandleNotifyFailString

		remote = Server.remote(\remTest, homeServer.addr, homeServer.options);
		// Server.default = remote; // to test IDE server display

		while { remote.serverRunning.not } { 0.1.wait };

		this.assert(remote.clientID == 3,
			"after recovering, remote client gets same clientID from server process."
		);

	}
}
