//# VERSION 3.16
TestRecorder : UnitTest {

	test_recorder_makePath_extension {
		var server = Server();
		server.options.recHeaderFormat = "aiff";
		server.recorder.recHeaderFormat = "wav";
		var ext = server.recorder.makePath.splitext[1];
		this.assertEquals("wav", ext)
	}

}
