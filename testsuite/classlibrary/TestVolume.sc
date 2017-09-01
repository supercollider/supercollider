TestVolume : UnitTest {

	test_setVolume {
		var server, volume, level, temp;

		server = Server.default;
		volume = server.volume;
		level = volume.volume;

		this.bootServer;

		this.assert(level == 0, "initial volume is 0 db");
		volume = -36;

		server.sync;

		this.assert(volume.ampSynth.notNil, "Server volume synth exists");
		volume.ampSynth.get(\volumeAmp, { |val| temp = val});
		this.assertFloatEquals(temp == level, "Volume level correctly set");

		volume.reset;
		server.quit;
	}
}
