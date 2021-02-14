TestNodeProxyRoles : UnitTest {

	var server, proxy;

	setUp {
		server = Server(this.class.name);
		server.bootSync;
		proxy = NodeProxy(server);
		server.sync;
	}

	tearDown {
		proxy.clear;
		server.quit;
		server.remove;
	}

	test_proxyroles_xfade_smoothly {
		var result = Array.new(8);
		var cond = Condition.new;
		var numSegments = 4;
		var buffer = Buffer.alloc(server, server.sampleRate.asInteger);
		var fadeTime = buffer.duration / numSegments;

		server.sync;

		proxy[3] = \filter -> { |in| RecordBuf.ar(in, buffer, loop: 0) };
		proxy.fadeTime = fadeTime;

		proxy[0] = { DC.ar(0.1) };
		fadeTime.wait;
		proxy[1] = \filter -> { DC.ar(0.2) };
		fadeTime.wait;
		proxy[1] = nil;
		fadeTime.wait;
		proxy[0] = nil;
		fadeTime.wait;

		fork {
			forBy(0, (buffer.numFrames - 1), (buffer.numFrames / (numSegments*2)).asInteger, { |index|
				buffer.get(index, { |val| result.add(val) });
				server.sync;
			});
			cond.unhang;
			buffer.free;
		};

		cond.hang;

		this.assertArrayFloatEquals(
			result,
			[0.0, 0.05, 0.1, 0.15, 0.2, 0.15, 0.1, 0.05],
			"Signal should fade smoothly over 'fadeTime' duration when adding/removing roles.",
			within: 0.02
		);
	}

}
