TestProxySpace : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		server.remove;
	}


	test_storeOn_recoversState {
		var psA, psB, stream, bValue, bNodeMap, aMapString, bMapString;

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

	test_copy_as_currentEnvironment {

		var environment = currentEnvironment;
		var proxySpace = ProxySpace(server);
		var copySpace;
		var family = IdentitySet.new;
		var commonProxies;

		this.bootServer(server);

		proxySpace.push;

		~freq = 80;
		~hpf = { HPF.ar(WhiteNoise.ar(0.1), ~freq.kr) };

		0.1.wait;

		copySpace = proxySpace.copy;

		proxySpace.pop;
		proxySpace.end;


		copySpace[\hpf].getFamily(family);
		this.assert(family.size == 2, "there should be one parent in this case", report: true); // sanity check

		commonProxies = proxySpace.envir.values.as(IdentitySet) sect: family;
		this.assert(commonProxies.isEmpty, "proxyspace copying should refresh local environment crossreferences");

		proxySpace.clear;
		copySpace.clear;

		currentEnvironment = environment;

	}


}
