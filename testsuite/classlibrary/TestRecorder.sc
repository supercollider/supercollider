TestRecorder : UnitTest {

	test_recorder_makePath_extension {
		var ext, server = Server();
		server.options.recHeaderFormat = "aiff";
		server.recorder.recHeaderFormat = "wav";
		ext = server.recorder.makePath.splitext[1];
		this.assertEquals("wav", ext)
	}

}
