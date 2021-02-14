TestOSCBundle : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
		server.bootSync;
	}

	tearDown {
		server.quit;
		server.remove;
	}

	test_doPrepare {
		var synthDef, bundle;
		var completed = false;

		bundle = OSCBundle.new;
		synthDef = Array.fill(100, { |i|
			var def = SynthDef("TestOSCBundle" ++ i, { Silent.ar });
			bundle.addPrepare(["/d_recv", def.asBytes])
		});
		bundle.doPrepare(server, { completed = true });
		this.wait({ completed }, "% timed out waiting for bundle to be sent".format(thisMethod));

		this.assertEquals(completed, true, "'doPrepare' sent the prepare bundle to the server");
	}

}

