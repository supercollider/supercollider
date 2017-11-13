// see also TestServer_clientID
/*
TestServer_clientID_booted.run;
*/

TestServer_clientID_booted : UnitTest {

	// with running server
	test_clientIDResetByServer {
		var options = ServerOptions.new;
		var s = Server(\testserv1, NetAddr("localhost", 57111), options);

		s.options.maxLogins = 4;
		s.clientID = 3;
		this.bootServer(s);
		s.sync;
		1.wait;
		this.assert(s.clientID == 3, "clientID should be settable before booting.");
		s.quit;
		s.remove;

		1.wait;

		s = Server(\testserv2, NetAddr("localhost", 57112), options, 3);

		this.bootServer(s);
		s.sync;
		1.wait;
		this.assert(s.clientID == 3, "clientID should remain as is when booting a server.");
		s.quit;
		s.remove;
	}

	test_nodeIDAlloc_ServerTree {
		var s = Server(\testserv3, NetAddr("localhost", 57113));
		var synth1, synth2;
		var func = { synth1 = Synth("default"); };

		ServerTree.add( func, s );
		this.bootServer(s);

		synth2 = Synth("default", [\freq, 330]);
		this.assert(synth1.nodeID != synth2.nodeID,
			"first nodeID after booting should not repeat nodeID created in ServerTree.");

		0.5.wait;
		synth1.free;
		synth2.free;
		s.quit;
		ServerTree.remove( func, s );
		s.remove;
	}

	test_clientIDLockedWhileRunning {
		var options = ServerOptions.new.maxLogins_(4);
		var s = Server(\testserv4, NetAddr("localhost", 57114), options, clientID: 1);
		var lockedID;

		this.bootServer(s);
		lockedID = s.clientID;

		s.clientID = 3;
		this.assert(s.clientID == lockedID,
			"clientID should be locked while server is running.");

		s.quit;
		s.remove;
	}
}
