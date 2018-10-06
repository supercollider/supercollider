TestNodeProxy : UnitTest {


	test_load_init {

		var x, proxy;
		var s = Server(this.class.name);

		// fail safe for large inits - must be Integer for supernova
		s.options.numWireBufs = (64 * (2**7)).asInteger;

		proxy = NodeProxy(s);

		x = nil;
		proxy.source = { x = "rebuilt"; SinOsc.ar([661.1, 877.1, 551.1]) };

		this.assert(x == "rebuilt", "when server isn't running, the SynthDef should be built");

		this.assert(
			proxy.rate == \audio,
			"when server isn't running, audio rate ugen graph should init neutral proxy in 'audio' rate",
		);

		this.assert(
			proxy.numChannels == 3,
			"when server isn't running, 3 channel ugen graph should init neutral proxy to 3 channels"
		);

		this.assert(
			proxy.loaded == false,
			"when server isn't running, nothing should be sent to it."
		);
		this.assert(
			proxy.isPlaying == false,
			"when server isn't running, NodeProxy shouldn't assume to be playing."
		);



		x = nil;

		// BOOT
		this.bootServer(s);
		Server.default.sync;

		this.assert(
			proxy.loaded == false,
			"after server has started, NodeProxy shouldn't change by itself."
		);

		proxy.send;

		this.assert(x == nil,
			"by calling 'send', SynthDef shouldn't be rebuilt");

		this.assert(proxy.isPlaying,
			"after send: NodeProxy group should be playing");

		this.assert(proxy.loaded == true,
			"after send: NodeProxy should have sent resources to the server (loaded)");

		proxy.rebuild;


		this.assert(x == "rebuilt",
			"after rebuilt: synth function should have been called");

		this.assert(proxy.loaded == true,
			"after rebuild and server running: resources should be on the server");

		// QUIT
		s.quit;
		x = nil;
		proxy.rebuild;

		this.assert(x == "rebuilt",
			"after server quit and rebuild: synth function should have been called");

		this.assert(proxy.loaded == false,
			"after server quit: no resources should be on the server");

		/* will be fixed in master
		proxy.send;
		this.assert(proxy.loaded != true,
			"after server quit: send should not assume server resources loaded");
		*/

		// CLEAR
		proxy.clear;
		try { proxy.ar(s.options.numAudioBusChannels * 2) };

		this.assert(proxy.isNeutral,
			"trying to allocate more bus channels than available should leave the node proxy untouched"
		);

		// ELASTIC

		proxy.reshaping = \elastic;

		try { Ndef(\x, { DC.ar(0 ! (s.options.numAudioBusChannels * 2)) }) };

		this.assert(proxy.isNeutral,
			"when elastic: trying to allocate more bus channels than available should leave the node proxy untouched"
		);

		proxy.ar(8);

		this.assert(proxy.numChannels == 8,
			"setting number of channels should change them"
		);

		try { Ndef(\x, { DC.ar(0 ! (s.options.numAudioBusChannels * 2)) }) };

		this.assert(proxy.numChannels == 8,
			"when elasic and initialised: trying to allocate more bus channels than available should leave the node proxy untouched"
		);


		proxy.clear;
		s.quit;
		s.remove;

	}

	test_fadeTime {

		var s = Server(this.class.name);
		var proxy;

		this.bootServer(s);
		s.sync;

		proxy = NodeProxy(s);
		proxy.fadeTime = 2;
		proxy.reshaping = \elastic;
		proxy.clear(1);
		1.01.wait;

		this.assert(proxy.isNeutral, "after fadeTime of clear, node proxy should be neutral again");

		proxy.clear;
		s.quit;
		s.remove;

	}

	test_synthDefControl_build {

		var s = Server(this.class.name);
		var proxy;

		s.quit;
		this.bootServer(s);
		s.sync;

		proxy = NodeProxy(s);
		proxy.source = { Silent.ar };

		this.assert(proxy.objects.first.hasFadeTimeControl, "functions should register their fadeTime control");

		proxy.clear;
		s.quit;
		s.remove;
	}

}

