TestVolume : UnitTest {

	test_booting {
		var s = Server(thisMethod.name);
		var correctReply = [ '/g_queryTree.reply', 0, 0, 2, 1, 0, 2, 1, 3, -1, 'volumeAmpControl2' ];
		var queryReply;
		var nodeIDidx = 6;

		// set volume so its synthdef, synth and set get sent right after boot
		s.volume.volume = -1;
		s.bootSync;

		// FIXME: wait for Volume synth to be requested after boot. Since there is no programmatic way
		// to check when this request has been sent, we have to do this the uncomfortable way.
		0.2.wait;

		OSCFunc({ |msg|
			queryReply = msg;
		},'/g_queryTree.reply', s.addr).oneShot;
		s.sendMsg("/g_queryTree", 0, 0);
		s.sync;

		// XXX: in some cases the node ID is 1001
		correctReply[nodeIDidx] = queryReply[nodeIDidx];
		this.assertEquals(queryReply, correctReply,
			"Server boot should send volume synthdef and create synth immediately when set to nonzero volume.");

		s.volume.reset;
		s.quit.remove;
	}

	test_setVolume {

		var s, ampSynthVolume;
		s = Server(thisMethod.name);

		this.bootServer(s);
		s.sync;

		this.assert(s.volume.volume == 0, "initial volume is 0 db");

		s.volume.volume = -36; // creates volume synth
		this.assert(s.volume.ampSynth.notNil, "Server volume synth exists");

		s.volume.ampSynth.get(\volumeAmp, { |level| ampSynthVolume = level });
		s.sync;
		this.assertFloatEquals(ampSynthVolume, s.volume.volume.dbamp, "volume level correctly set", 0.0001);

		s.volume.reset;
		s.quit.remove;
	}

	test_numOutputs {

		var s, default_numChannels;
		s = Server(thisMethod.name);

		s.options.numOutputBusChannels = 8;

		this.bootServer(s);
		s.sync;

		this.assert(s.outputBus.numChannels == s.volume.numOutputChannels, "volume synth has correct number of channels");

		s.quit.remove;
	}

	test_remoteSetVolume {
		var s = Server(thisMethod.name);
		var condition = CondVar.new;

		s.options.bindAddress = "0.0.0.0"; // allow connections from any address
		s.options.maxLogins = 2; // set to 2 clients

		this.bootServer(s);

		// create ampSynth
		s.bind {
			s.volume = -3;
			s.sync;
		};

		// send from outside like a remote client
		s.sendMsg(15, 3, \volumeAmp, 0.25);

		s.volume.ampSynth.addDependant({ |synth, changer|
			if (changer == 'n_end') {
				condition.signalOne;
			};
		});
		condition.waitFor(1);

		this.assertFloatEquals(s.volume.volume, 0.25.ampdb,
			"server should update volume level when set from remote client."
		);

		s.quit.remove;
	}

	test_resetVolumeAfterKill {
		var s = Server(thisMethod.name);
		var condition = CondVar.new;

		s.options.bindAddress = "0.0.0.0"; // allow connections from any address
		s.options.maxLogins = 2; // set to 2 clients

		this.bootServer(s);

		// create ampSynth
		s.bind {
			s.volume = -3;
			s.sync;
		};

		// kill ampSynth irregularly
		s.sendMsg("n_free", s.volume.ampSynth.nodeID);
		s.volume.ampSynth.onFree({ condition.signalOne });
		condition.waitFor(1);

		this.assertFloatEquals(s.volume.volume, 0,
			"server should reset volume level when ampSynth dies irregularly."
		);

		s.quit.remove;
	}


}
