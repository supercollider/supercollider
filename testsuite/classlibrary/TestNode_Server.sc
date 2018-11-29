TestNode_Server : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
		this.bootServer(server);
	}

	tearDown {
		server.quit.remove;
	}

	test_get {
		var setValue, getValue, node;
		var condition = Condition.new;

		SynthDef(\test_get, { |control = 8| }).add;
		server.sync;
		setValue = 888;
		node = Synth(\test_get, [control: setValue]);
		server.sync;

		getValue = 0;
		node.get(\control, { |value|
			getValue = value;
			condition.unhang;
		});
		condition.hang;

		this.assertFloatEquals(getValue, setValue, "Node:get works", 0.001);
		node.free;
	}

	// this one currently fails with supernova (3.9.3)
	test_getn {
		var setnValues, getnValues, node, timeout = 1;
		var condition = Condition.new;

		SynthDef(\test_getn, { |control1 = 2, control2 = 22.2, control3 = 222| }).add;
		server.sync;
		setnValues = [888, 88.8, 8.88];
		node = Synth(\test_getn, [
			control1: setnValues[0],
			control2: setnValues[1],
			control3: setnValues[2]
		]);
		server.sync;

		getnValues = 0;
		node.getn(0, 3, { |values|
			getnValues = values;
			condition.test = true;
		});

		this.wait({ condition.test }, "getn response timed out after % seconds.".format(timeout), timeout);

		this.assertArrayFloatEquals(getnValues, setnValues, "Node:getn works", 0.001);
		node.free;
	}

	test_releaseGate {
		var reply, msg;

		SynthDef(\sendReply, { |gate=1, out=0|
			var sig = DC.ar(0.01) * EnvGen.kr(Env.cutoff(1e-3), gate, doneAction: 2);
			SendReply.kr(gate<=0, '/gateVal', gate);
			Out.ar(out, sig);
		}).add;

		OSCFunc({ |replyMsg| reply = replyMsg }, '/gateVal', server.addr);
		server.sync;

		[ [0,-1], [2,-3], [nil,0], [-2,-1] ].do { |releaseGatePair|
			var node = Synth(\sendReply);
			server.sync;

			msg = ['/gateVal', node.nodeID, -1, releaseGatePair[1].asFloat];
			node.release(releaseGatePair[0]);
			server.sync;

			this.assertEquals(reply, msg, "Gate should be set to % in Server".format(releaseGatePair[1]));
			reply = nil;
			node.free;
		};
	}

}
