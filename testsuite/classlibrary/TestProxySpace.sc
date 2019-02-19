TestProxySpace : UnitTest {


	test_storeOn_recoversState {
		var psA, psB, stream, bValue, bNodeMap, failedKey;
		var server = Server(\test, NetAddr("127.0.0.1", 57111));

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

		failedKey = block { |break|
			psA.keysValuesDo { |key, value|
				bValue = psB[key];
				// basic checks
				if(bValue.rate != value.rate or: {
					bValue.numChannels != value.numChannels or: {
						bValue.source.asCompileString != value.source.asCompileString
					}
				}) {
					break.value(key);
				};
				// check nodemap
				bNodeMap = bValue.nodeMap;
				value.nodeMap.keysValuesDo { |parm, map|
					// output bus indices will not match, skip them
					if(#[out, i_out].includes(parm).not) {
						if(map.asCompileString != bNodeMap[parm].asCompileString) {
							break.value(key);
						};
					};
				};
			};
			nil
		};

		psA.clear;
		psB.clear;
		server.quit;

		this.assert(failedKey.isNil,
			"ProxySpace document string should recover all proxies' sources, rates, numChannels and nodemaps %"
			.format(if(failedKey.notNil) { "(% failed)".format(failedKey) } { "" })
		);
	}
}
