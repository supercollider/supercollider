// TestServer_SynthDefVersion1
// Brian Heim
// 03-01-2018
// Tests for the server handling Version 1 synthdefs.
TestServer_SynthDefVersion1 : UnitTest {

	// See https://github.com/supercollider/supercollider/issues/3266
	// Synthdef file (sonic-pi-beep.scsynthdef) by Sam Aaron, MIT License
	// Original file: https://github.com/samaaron/sonic-pi/blob/36496b69e7e821562c6608a276b4536229d38250/etc/synthdefs/compiled/sonic-pi-beep.scsyndef
	test_serverLoadsSonicPiBeepSynthDef {
		var server = Server(thisMethod.name);
		var cond = Condition();

		this.bootServer(server);

		server.sendMsg('/d_load', "sonic-pi-beep.scsyndef".resolveRelative, ['/s_new', 'sonic-pi-beep', server.nextNodeID]);

		fork { 3.wait; cond.test_(true).signal };
		server.sync(cond);

		this.assert(server.serverRunning, "sonic-pi-beep synthdef should not crash server");
		if (server.serverRunning) {
			server.quit;
		};

		server.remove;
	}
}
