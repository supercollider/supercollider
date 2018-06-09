TestNode : UnitTest {

	var server, node;

	setUp {
		server = Server(this.class.name);
		this.bootServer(server);
	}

	tearDown {
		server.quit.remove;
	}

	test_get {
		var set_value, get_value;

		SynthDef(\test_get, { |control = 8| }).add;
		server.sync;
		set_value = 888;
		node = Synth(\test_get, [control: set_value]);
		server.sync;

		get_value = 0;
		node.get(\control, { |value|
			get_value = value;
		});
		0.1.wait;

		this.assertFloatEquals(get_value, set_value, "Node:get works", 0.001);
		node.free;
	}

	test_getn {
		var setn_values, getn_values;

		SynthDef(\test_getn, { |control1 = 2, control2 = 22.2, control3 = 222| }).add;
		server.sync;
		setn_values = [888, 88.8, 8.88];
		node = Synth(\test_getn, [
			control1: setn_values[0],
			control2: setn_values[1],
			control3: setn_values[2]
		]);
		server.sync;

		getn_values = 0;
		node.getn(0, 3, { |values|
			getn_values = values;
		});
		0.1.wait;

		this.assertArrayFloatEquals(getn_values, setn_values, "Node:getn works", 0.001);
		node.free;
	}

	test_node_releaseGate {
		var reply, msg;

		SynthDef(\sendReply, { |gate=1, out=0|
			var sig = DC.ar(0.01) * EnvGen.kr(Env.cutoff(1e-3), gate, doneAction: 2);
			SendReply.kr(gate<=0, '/gateVal', gate);
			Out.ar(out, sig);
		}).add;

		OSCFunc({ |replyMsg| reply = replyMsg }, '/gateVal', server.addr);
		server.sync;

		[0, -1, 2, -3, nil, 0, -2, -1].clump(2).do { |releaseGatePair|
			node = Synth(\sendReply);
			server.sync;

			msg = ['/gateVal', node.nodeID, -1, releaseGatePair[1].asFloat];
			node.release(releaseGatePair[0]);
			server.sync;

			this.assertEquals(reply, msg, "Gate should be set to % in Server".format(releaseGatePair[1]));
			reply = nil;
		};

		node.free;

	}

}
