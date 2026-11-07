//# VERSION 3.16

TestControlSpec : UnitTest {

	test_equals {
		var a = ControlSpec(0, 1, 'linear', 0, 0.0, "");
		var b = ControlSpec(0, 1, 'linear', 0, 0.0, "");
		var c = ControlSpec(0, 2, 'linear', 0, 0.0, "");

		// no really, this failed !
		this.assert( a == b, "2 identical control specs should be equal");
		this.assert( a != c, "2 different control specs should not be equal");

		this.assert( a == a, "control spec should equal itself");

	}
	test_copy {

		var z = ControlSpec(50,100);
		this.assert(z.warp.spec === z,"the spec's warp's spec should be the spec");

		var w = z.copy;
		this.assert(z.warp !== w.warp,"should not copy the warp object when you copy a control spec");
		this.assert( w.warp.spec === w,"the copied spec's warp should be the copied spec");

		z.minval = 70;
		this.assert(z.minval == 70,"setting minval on spec should work");

		this.assert( w.minval != 70,"setting minval on spec should not affect its copy");

		// and here is the bug itself
		this.assert( w.map(0.0) == 50,"mapping using the copied spec should result in the original mapping behavior of the original spec.");
	}

	test_setFrom {

		var a = ControlSpec(1, 2, \lin, 0.1, 1.5, "test units");
		var b = ControlSpec().setFrom(a);

		this.assertEquals(b.minval, 1, "set minval");
		this.assertEquals(b.maxval, 2, "set maxval");
		this.assert(b.warp.isKindOf(LinearWarp), "set warp");
		this.assertEquals(b.warp.spec, b, "set warp:spec");
		this.assertEquals(b.step, 0.1, "set step");
		this.assertEquals(b.default, 1.5, "set default");
		this.assertEquals(b.units, "test units", "set units");
		this.assertEquals(b.grid.spec, b, "set grid");
	}

	test_defcontrols_args {
		var def;

		var spec = ControlSpec(100, 1000, \exp, 1, 440);

		def = SynthDef(\__unittest_controlspec_namedcontrols, {
			|freq=123|
			freq.spec = spec;
			Out.ar(0, SinOsc.ar(freq));
		}).add;

		this.assertEquals(spec, def.specs[\freq], "spec set correctly on synth argument");
	}

	test_defcontrols_symbols {
		var def;

		var spec = ControlSpec(100, 1000, \exp, 1, 440);

		def = SynthDef(\__unittest_controlspec_namedcontrols, {

			var dur = \dur.ir(spec:spec);
			var freq = \freq.kr(spec:spec);
			var input = \input.ar(spec:spec);
			var gate = \gate.tr(spec:spec);

			Out.ar(0, SinOsc.ar(freq));
		}).add;

		this.assertEquals(spec, def.specs[\dur], "set correctly with named ir control");
		this.assertEquals(spec, def.specs[\freq], "set correctly with named kr control");
		this.assertEquals(spec, def.specs[\input], "set correctly with named ar control");
		this.assertEquals(spec, def.specs[\gate], "set correctly with named tr control");
	}

	test_defcontrols_metadata {

		var spec = ControlSpec(100, 1000, \exp, 1, 440);

		var def = SynthDef(\__unittest_controlspec_namedcontrols, {
			|freq|
			Out.ar(0, SinOsc.ar(freq));
		}, metadata: ( specs: ( freq: spec ))).add;

		this.assertEquals(spec, def.specs[\freq], "set correctly in metadata blob");
	}

	test_defcontrols_nodeproxy {

		var spec = ControlSpec(100, 1000, \exp, 1, 440);

		var proxy = NodeProxy.audio(Server.default, 2);
		proxy.source = {
			SinOsc.ar(\freq.kr(spec:spec));
		};

		this.assertEquals(spec, proxy.specs[\freq], "set correctly in NodeProxy");
	}

	test_def_defaults {
		var def;

		def = SynthDef(\__unittest_controlspec_namedcontrols_c, {
			var freq = \freq.kr(spec:ControlSpec(0, 10000, default:123));
		}).add;

		this.assertEquals(def.desc.controls[0].defaultValue, 123, "set default correctly from SynthDef");
	}


}

TestCurveWarp :UnitTest {

	test_equals {
		var s = ControlSpec(0.3,10.0);
		var c = CurveWarp(s,-2);
		var d = CurveWarp(s,-2);
		var e = CurveWarp(s,-3);

		this.assert( c == d, "CurveWarp should be equal");
		this.assert( c != e, "CurveWarp should not be equal");
		this.assert( c == c, "identical CurveWarp should be equal");
		this.assert( c != "rabbits", "CurveWarp should not equal rabbits");
	}
}
