// see also TestServer_clientID
/*
TestServer_clientID_booted.run;
*/

TestServer_clientID_booted : UnitTest {

	// with running server
	test_clientIDResetByServer {
		var options = ServerOptions.new;
		var s;
		s = Server(\testserv1, NetAddr("localhost", 57111), options);

		s.options.maxLogins = 4;
		s.clientID = 3;
		this.bootServer(s);
		s.sync;
		1.wait;
		this.assert(s.clientID == 0, "Non-user-defined clientID should be reset by the server.");
		Server.named.removeAt(s.name);
		Server.all.remove(s);
		s.quit;
		1.wait;

		s = Server(\testserv2, NetAddr("localhost", 57112), options, 3);

		this.bootServer(s);
		s.sync;
		1.wait;
		this.assert(s.clientID == 3, "User-defined clientID should be left as is by the server.");
		Server.named.removeAt(s.name);
		Server.all.remove(s);
	}
}
