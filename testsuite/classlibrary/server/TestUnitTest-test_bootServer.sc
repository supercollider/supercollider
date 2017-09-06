+ TestUnitTest {
	test_bootServer {
		this.bootServer;
		this.assert(Server.default.serverRunning, "Server.default should be booted while we waited");
	}
}