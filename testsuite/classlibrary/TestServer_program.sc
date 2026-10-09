TestServer_program : UnitTest {
	test_replaceProgram {
		this.assertEquals(
			Server.prReplaceProgram("old program string /path/to/supernova meow", "/scsynth/path", "supernova", "scsynth"),
			"old program string /scsynth/path meow"
		);
		this.assertEquals(
			Server.prReplaceProgram("old program string /path/to/supernova meow", nil, "supernova", "scsynth"),
			"old program string /path/to/scsynth meow"
		);
	}

}
