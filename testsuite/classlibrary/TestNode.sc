TestNode : UnitTest {

	test_get {
		var s, node, set_value, get_value;
		this.bootServer;
		s = this.s;

		SynthDef(\test_get, {
			|control = 8|
		}).add;
		s.sync;
		set_value = 888;
		node = Synth(\test_get, [control: set_value]);
		s.sync;

		get_value = 0;
		node.get(\control, { |value|
			get_value = value;
		});
		0.1.wait;

		this.assertFloatEquals(get_value, set_value, "Node:get works", 0.001);
		node.free;
	}

	test_getn {
		var s, node, setn_values, getn_values;
		this.bootServer;
		s = this.s;

		SynthDef(\test_getn, {
			|control1 = 2, control2 = 22.2, control3 = 222|
		}).add;
		s.sync;
		setn_values = [888, 88.8, 8.88];
		node = Synth(\test_getn, [
			control1: setn_values[0],
			control2: setn_values[1],
			control3: setn_values[2]
		]);
		s.sync;

		getn_values = 0;
		node.getn(0, 3, { |values|
			getn_values = values;
		});
		0.1.wait;

		this.assertArrayFloatEquals(getn_values, setn_values, "Node:getn works", 0.001);
		node.free;
	}

}
