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
		this.assert(s.clientID == 0, "Non-user-defined clientID should be reset by the server.");
		s.quit;
		s.remove;

		1.wait;

		s = Server(\testserv2, NetAddr("localhost", 57112), options, 3);

		this.bootServer(s);
		s.sync;
		1.wait;
		this.assert(s.clientID == 3, "User-defined clientID should be left as is by the server.");
		s.quit;
		s.remove;
	}

	test_nodeIDAlloc_ServerTree {
		var s = Server(\testserv3, NetAddr("localhost", 57111));
		var synth1, synth2;

		ServerTree.add( { synth1 = Synth("default"); });
		this.bootServer(s);

		1.wait;

		synth2 = Synth("default", [\freq, 330]);
		this.assert(synth1.nodeID != synth2.nodeID,
			"first nodeID after booting should not repeat nodeID created in ServerTree.");
		s.quit;
		s.remove;
	}
}
