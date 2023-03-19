TestSynthDefOptimizations : UnitTest {
	test_optimizeAddNeg {
		var def = SynthDef(\addneg, {
			var a = DC.kr(0), b = DC.kr(1);
			Out.kr(0, a + b.neg)
		}),
		op = def.children.detect(_.isKindOf(BinaryOpUGen));
		this.assert(
			def.children.any(_.isKindOf(UnaryOpUGen)).not and: {
				op.notNil and: { op.operator == '-' }
			},
			"UGens a + b.neg should optimize to a - b"
		);
		def = SynthDef(\addneg, {
			var a = DC.kr(0), b = DC.kr(1).neg;
			Out.kr(0, a + b);
			Out.kr(2, b);
		});
		op = def.children.detect(_.isKindOf(BinaryOpUGen));
		this.assert(
			op.notNil and: { op.operator == '+' },
			"a + b.neg, b.neg should be kept if it has multiple descendants"
		);
	}
	test_optimizeSub {
		var def = SynthDef(\sub, {
			var a = DC.kr(0), b = DC.kr(1);
			Out.kr(0, a - b.neg)
		}),
		op = def.children.detect(_.isKindOf(BinaryOpUGen));
		this.assert(
			def.children.any(_.isKindOf(UnaryOpUGen)).not and: {
				op.notNil and: { op.operator == '+' }
			},
			"UGens a - b.neg should optimize to a + b"
		);
		def = SynthDef(\addneg, {
			var a = DC.kr(0), b = DC.kr(1).neg;
			Out.kr(0, a - b);
			Out.kr(2, b);
		});
		op = def.children.detect(_.isKindOf(BinaryOpUGen));
		this.assert(
			op.notNil and: { op.operator == '-' },
			"a - b.neg, b.neg should be kept if it has multiple descendants"
		);
	}
	test_optimizeToMulAdd {
		var def;
		[
			"ugen * 2 + 1" -> { SinOsc.ar * 2 + 1 },
			"2 * ugen + 1" -> { 2 * SinOsc.ar + 1 },
			"1 + (ugen * 2)" -> { 1 + (SinOsc.ar * 2) },
			"1 + (2 * ugen)" -> { 1 + (2 * SinOsc.ar) }
		].do { |assn|
			def = SynthDef(\test, {
				Out.ar(0, assn.value.value)
			});
			this.assert(
				def.children.any(_.isKindOf(BinaryOpUGen)).not and: {
					def.children.count(_.isKindOf(MulAdd)) == 1
				},
				"Operator sequence '%' should optimize to MulAdd".format(assn.key)
			)
		};
		def = SynthDef(\test, {
			var osc = SinOsc.ar(440, 0, 0.1);
			Out.ar(0, osc);
			Out.ar(2, osc + 1);
		});
		this.assert(
			def.children.any(_.isKindOf(MulAdd)).not,
			"ugen * 0.1 + 1, reuse of '*' result prevents optimization"
		);
	}
	test_optimizeToSum3 {
		var def = SynthDef(\sum3, {
			var ugens = DC.kr((0..2));
			Out.kr(0, ugens.sum)
		});
		this.assert(
			def.children.any(_.isKindOf(BinaryOpUGen)).not
			and: { def.children.count(_.isKindOf(Sum3)) == 1 },
			"UGens a + b + c, both '+' should be replaced by one Sum3"
		);
		def = SynthDef(\sum3, {
			var ugens = DC.kr((0..2)),
			aplusb = ugens[0] + ugens[1];
			Out.kr(0, aplusb + ugens[2]);
			Out.kr(2, aplusb)
		});
		this.assert(
			def.children.count(_.isKindOf(BinaryOpUGen)) == 2
			and: { def.children.any(_.isKindOf(Sum3)).not },
			"UGens a + b + c, reuse of a + b prevents optimization"
		);
	}
	test_optimizeToSum4 {
		var def = SynthDef(\sum4, {
			var ugens = DC.kr((0..3));
			Out.kr(0, ugens.sum)
		});
		this.assert(
			def.children.any(_.isKindOf(BinaryOpUGen)).not
			and: { def.children.count(_.isKindOf(Sum4)) == 1 },
			"UGens a + b + c + d, all '+' should be replaced by one Sum4"
		);
		def = SynthDef(\sum4, {
			var ugens = DC.kr((0..3)),
			abc = ugens[0..2].sum;
			Out.kr(0, abc + ugens.last);
			Out.kr(2, abc);
		});
		this.assert(
			def.children.count(_.isKindOf(Sum3)) == 1 and: {
				def.children.count(_.isKindOf(BinaryOpUGen)) == 1 and: {
					def.children.any(_.isKindOf(Sum4)).not
				}
			},
			"UGens a + b + c + d, reuse of a + b + c prevents optimization"
		);
	}
}
