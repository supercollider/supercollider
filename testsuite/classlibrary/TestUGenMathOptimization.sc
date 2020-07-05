TestUGenMathOptimization : UnitTest {
	test_optimizeToSum3_handles_duplicate_operand {
		var def = SynthDef(\testSum3, {
			var sig = SinOsc.kr(0.1) + Rand(-0.02, 0.02);
			Out.kr(0, sig + sig);
		});  // don't need to add
		this.assertEquals(
			def.children.collect(_.class),
			[SinOsc, Rand, Sum4, Out],
			"(a+b) + (a+b) should optimize to a Sum4"
		);
	}
}
