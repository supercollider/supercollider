//# VERSION 3.16

TestNodeProxyBusMapping_Server : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
		server.latency = 0.1;
		this.bootServer(server);
		server.sync;
	}


	tearDown {
		server.quitSync.remove;
	}

	test_audiorate_mapping {

		var defaultValues = [-1.0, -2.0];
		var controlValues = [162.0, 54.0];

		var proxy = NodeProxy(server, \audio);
		proxy.reshaping = \elastic;
		proxy.fadeTime = 0;

		proxy.source = {
			var in = \in.ar(defaultValues);
			A2K.kr(in);
		};

		var inputProxy = NodeProxy(server, \audio);
		inputProxy.source = {
			DC.ar(controlValues)
		};

		0.2.wait;

		this.assert(proxy.rate == \control, "proxy should have initialized itself to control rate");

		var synthValues = server.getControlBusValues(proxy.bus.index, proxy.numChannels);

		this.assertEquals(synthValues, defaultValues, "before mapping, synth values should be default values");

		proxy <<>.in inputProxy;

		0.2.wait;

		synthValues = server.getControlBusValues(proxy.bus.index, proxy.numChannels);

		this.assertEquals(synthValues, controlValues, "after mapping, synth should return mapped values");


	}

	test_audiorate_mapping_elasticity {

		var defaultValues = [-1.0, -2.0, -3.0];
		var controlValues = [162.0, 54.0, 45.0, 87.0];

		var proxy = NodeProxy(server, \audio, 7);
		proxy.reshaping = \elastic;
		proxy.fadeTime = 0.001;

		proxy.source = {
			var in = \in.ar(defaultValues);
			A2K.kr(in);
		};

		var inputProxy = NodeProxy(server, \audio, 2);
		inputProxy.reshaping = \elastic;
		inputProxy.fadeTime = 0.001;
		inputProxy.source = {
			DC.ar(controlValues)
		};

		0.2.wait;

		this.assertEquals(proxy.rate, \control, "proxy should have initialized itself to control rate");
		this.assertEquals(proxy.numChannels, defaultValues.size, "proxy should have initialized itself to the correct number of channels");

		var synthValues = server.getControlBusValues(proxy.bus.index, proxy.numChannels);

		this.assertEquals(synthValues.round, defaultValues.round, "before mapping, synth values should be default values");

		proxy <<>.in inputProxy;

		0.2.wait;

		synthValues = server.getControlBusValues(proxy.bus.index, proxy.numChannels);

		this.assertEquals(synthValues.round, controlValues.keep(synthValues.size).round, "after mapping, synth should return mapped values");
	}
}
