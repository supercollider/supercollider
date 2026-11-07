//# VERSION 3.16
TestUnitTest_Server : UnitTest {

	test_bootServer {
		var server = Server(this.class.name);
		server.bootSync;
		this.assert(server.serverRunning, "The test's Server should be booted while we waited");
		server.quitSync.remove;
	}

}
