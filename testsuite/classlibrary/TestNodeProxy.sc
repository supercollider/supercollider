TestNodeProxy : UnitTest {


	test_load_init {

		var x, s;

		s = Server.default;
		s.quit;

		Ndef.clear;
		x = nil;
		Ndef(\x, { x = "rebuilt"; SinOsc.ar([661.1, 877.1, 551.1]) });

		this.assert(x == "rebuilt", "when server isn't running, the SynthDef should be built");

		this.assert(
			Ndef(\x).rate == \audio,
			"when server isn't running, audio rate ugen graph should init neutral proxy in 'audio' rate",
		);

		this.assert(
			Ndef(\x).numChannels == 3,
			"when server isn't running, 3 channel ugen graph should init neutral proxy to 3 channels"
		);

		this.assert(
			Ndef(\x).loaded == false,
			"when server isn't running, nothing should be sent to it."
		);
		this.assert(
			Ndef(\x).isPlaying == false,
			"when server isn't running, NodeProxy shouldn't assume to be playing."
		);



		x = nil;

		// BOOT
		this.bootServer(s);
		Server.default.sync;

		this.assert(
			Ndef(\x).loaded == false,
			"after server has started, NodeProxy shouldn't change by itself."
		);

		Ndef(\x).send;

		this.assert(x == nil,
			"by calling 'send', SynthDef shouldn't be rebuilt");

		this.assert(Ndef(\x).isPlaying,
			"after send: NodeProxy group should be playing");

		this.assert(Ndef(\x).loaded == true,
			"after send: NodeProxy should have sent resources to the server (loaded)");

		Ndef(\x).rebuild;


		this.assert(x == "rebuilt",
			"after rebuilt: synth function should have been called");

		this.assert(Ndef(\x).loaded == true,
			"after rebuild and server running: resources should be on the server");

		// QUIT
		s.quit;
		x = nil;
		Ndef(\x).rebuild;

		this.assert(x == "rebuilt",
			"after server quit and rebuild: synth function should have been called");

		this.assert(Ndef(\x).loaded == false,
			"after server quit: no resources should be on the server");

		/* will be fixed in master
		Ndef(\x).send;
		this.assert(Ndef(\x).loaded != true,
			"after server quit: send should not assume server resources loaded");
		*/

		// fail safe for large inits
		s.options.numWireBufs = 64 * (2**7);

		// CLEAR
		Ndef.clear;
		try { Ndef(\x).ar(s.options.numAudioBusChannels * 2) };

		this.assert(Ndef(\x).isNeutral,
			"trying to allocate more bus channels than available should leave the node proxy untouched"
		);

		// ELASTIC

		Ndef(\x).reshaping = \elastic;

		try { Ndef(\x, { DC.ar(0 ! (s.options.numAudioBusChannels * 2)) }) };

		this.assert(Ndef(\x).isNeutral,
			"when elastic: trying to allocate more bus channels than available should leave the node proxy untouched"
		);

		Ndef(\x).ar(8);

		this.assert(Ndef(\x).numChannels == 8,
			"setting number of channels should change them"
		);

		try { Ndef(\x, { DC.ar(0 ! (s.options.numAudioBusChannels * 2)) }) };

		this.assert(Ndef(\x).numChannels == 8,
			"when elasic and initialised: trying to allocate more bus channels than available should leave the node proxy untouched"
		);


	}

	test_fadeTime {

		var s = Server.default;
		s.quit;
		this.bootServer(s);
		s.sync;
		1.wait;

		Ndef.clear;
		Ndef(\x).fadeTime = 2;
		Ndef(\x).reshaping = \elastic;
		Ndef(\x).clear(1);
		1.01.wait;
		this.assert(Ndef(\x).isNeutral, "after fadeTime of clear, node proxy should be neutral again");

	}


}

