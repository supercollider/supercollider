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

	// test that repeated login from the same server object to a running server process
	// ends with correct server status, and keeps clientID and allocators  as is.
	test_repeatedLogin {
		var server, cond, timer, lastNodeID;

		server = Server(thisMethod.name.asSymbol);
		server.options.maxLogins = 4;
		server.clientID = 3;
		this.bootServer(server);

		// use some nodeIDs so we know whether nodeID allocator is reset
		10.do { server.nextNodeID };
		lastNodeID = server.nextNodeID;

		// simulate losing network contact by turning off watcher,
		// but actually leave the server process running
		server.statusWatcher
		.stopStatusWatcher
		.stopAliveThread
		.notified_(false)
		.serverRunning_(false);

		// now login again from the same server object
		// should get "This seems to be a login after loss of network ... all is well" post.
		server.startAliveThread;

		cond = Condition.new;

		timer = fork { 3.wait; cond.unhang };
		server.doWhenBooted {
			"% - server login timed out.\n".postf(thisMethod);
			timer.stop;
			cond.unhang;
		};

		cond.hang;

		this.assert(server.serverRunning,
			"after repeated login, server should be known to be fully running."
		);

		this.assert(server.clientID == 3,
			"after repeated login, remote client should get the same clientID from server process."
		);

		this.assert(server.nextNodeID > lastNodeID,
			"after repeated login, nodeID allocator should not be reset."
		);

		server.quit;
		server.remove;

	}

	// test that logins to a (pseudo-) remote server process work as intended
	// first login initializes fully,
	// second login from same address gets the same clientID, and sets running state.
	test_remoteLogin {
		var options, serverPid, remote1, cond, timer;

		options = ServerOptions.new;
		options.maxLogins = 4;

		// simulate a remote server process by starting a server process independently of SC
		serverPid = unixCmd(Server.program + options.asOptionsString);

		// login with standard Server.remote method to test for
		remote1 = Server.remote(\remoteLogin1, options: options, clientID: 3);
		cond = Condition.new;
		timer = fork { 3.wait; cond.unhang };

		remote1.doWhenBooted {
			"% - server first login timed out.\n".postf(thisMethod);
			timer.stop;
			cond.unhang;
		};
		cond.hang;

		this.assert(remote1.serverRunning,
			"after first login, remote client should be known to be fully running."
		);

		this.assert(remote1.clientID == 3,
			"after first login, remote client should have its requested clientID."
		);

		remote1.remove;
		thisProcess.platform.killProcessByID(serverPid);
	}

	test_repeatedRemoteLogin {

		var options, serverPid, remote1, remote2, cond, timer;

		options = ServerOptions.new;
		options.maxLogins = 4;

		// simulate a remote server process by starting a server process independently of SC
		serverPid = unixCmd(Server.program + options.asOptionsString);

		// login with standard Server.remote method to test for
		remote1 = Server.remote(\remoteLogin1, options: options, clientID: 3);
		cond = Condition.new;
		timer = fork { 3.wait; cond.unhang };

		remote1.doWhenBooted {
			"% - server first login timed out.\n".postf(thisMethod);
			timer.stop;
			cond.unhang;
		};
		cond.hang;

		// now login again from a different server object,
		// but from the same client address (in the test, local, in reality, it would be remote).
		// This simulates a crashed remote client which gets restarted,
		// and logs in with its old setup data.

		// Because client netaddr is already registered in the server process,
		// the response goes thru prHandleNotifyFailString, and prHandleLoginWhenAlreadyRegistered

		remote2 = Server.remote(\remoteLogin2, options: options);
		cond = Condition.new;
		timer = fork { 3.wait; cond.unhang };

		remote2.doWhenBooted {
			"% - server repeated login timed out.\n".postf(thisMethod);
			timer.stop;
			cond.unhang;
		};

		cond.hang;

		this.assert(remote2.serverRunning,
			"after re-login from a new server object, remote client should be known to be fully running."
		);

		this.assert(remote2.clientID == 3,
			"after re-login from a new server object, remote client should have its requested clientID."
		);

		// cleanup
		remote1.remove;
		remote2.remove;
		thisProcess.platform.killProcessByID(serverPid);
	}
}
