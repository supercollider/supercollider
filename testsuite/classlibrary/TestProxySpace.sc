TestProxySpace : UnitTest {


	test_storeOn_recoversState {
		var psA, psB, stream, bValue, bNodeMap, aMapString, bMapString;
		var server = Server(this.class.name, NetAddr("127.0.0.1", 57111));

		this.bootServer(server);
		server.initTree;

		psA = ProxySpace(server).make {
			~out = { \in.ar(0!2) };
			~osc = { |freq = 440| SinOsc.ar(freq).dup };
			~out.set(\in, ~osc);
			~freqmod = { LFDNoise3.kr(14).exprange(200, 800) };
			~osc.set(\freq, ~freqmod);

			// must do this inside the ProxySpace environment
			// otherwise it can't identify the ~keys
			stream = CollStream.new;
			currentEnvironment.storeOn(stream);
		};

		psB = ProxySpace(server).make {
			stream.collection.interpret;
		};

		psA.keysValuesDo { |key, value|
			bValue = psB[key];
			// basic checks
			this.assertEquals(value.rate, bValue.rate, "Restored NodeProxy rate should match original (%)".format(key));
			this.assertEquals(value.numChannels, bValue.numChannels, "Restored NodeProxy numChannels should match original (%)".format(key));
			this.assertEquals(value.source.asCompileString, bValue.source.asCompileString, "Restored NodeProxy source should match original (%)".format(key));
			// check nodemap
			bNodeMap = bValue.nodeMap;
			value.nodeMap.keysValuesDo { |parm, map|
				// output bus indices will not match, skip them
				if(#[out, i_out].includes(parm).not) {
					psA.use { aMapString = map.asCompileString };
					psB.use { bMapString = bNodeMap[parm].asCompileString };
					this.assertEquals(
						aMapString, aMapString,
						"Restored NodeProxy's nodeMap should match original (%, parameter %)"
						.format(key, parm)
					);
				};
			};
		};

		psA.clear;
		psB.clear;
		server.quit;
		server.remove;
	}
}
