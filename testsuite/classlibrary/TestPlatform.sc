//# VERSION 3.16
TestPlatform : UnitTest {
	test_systemInformation {
		var info = Platform.systemInformation();
		this.assert(info[\version] == Main.version);
	}
}
