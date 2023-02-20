TestUGenMathOptimization : UnitTest {
	test_optimizeToSum3 {
		this.verifyTestCases([
			"a + b + c should optimize to Sum3",
			{ SinOsc.kr(1) + LFNoise0.kr(1) + DC.kr(2) } -> [SinOsc, LFNoise0, DC, Sum3, Out],
			"a + (b + c) should optimize to Sum3",
			{ SinOsc.kr(1) + (LFNoise0.kr(1) + DC.kr(2)) } -> [SinOsc, LFNoise0, DC, Sum3, Out],
			"a + b + c should not optimize to Sum3 if a + b has multiple descendants",
			{
				var a = SinOsc.kr(1);
				var b = LFNoise0.kr(1);
				var c = DC.kr(2);
				var ab = a + b;
				(ab + c) * ab
			} -> [SinOsc, LFNoise0, '+', DC, '+', '*', Out],
			"a + (b + c) should not optimize to Sum3 if b + c has multiple descendants",
			{
				var a = SinOsc.kr(1);
				var b = LFNoise0.kr(1);
				var c = DC.kr(2);
				var bc = b + c;
				(a + bc) * bc
			} -> [SinOsc, LFNoise0, DC, '+', '+', '*', Out],
		]);
	}

	test_optimizeToSum4 {
		this.verifyTestCases([
			"a + b + c + d should optimize to Sum4",
			{
				SinOsc.kr(1) + LFNoise0.kr(1) + DC.kr(2) + Impulse.kr(3)
			} -> [SinOsc, LFNoise0, DC, Impulse, Sum4, Out],
			"a + (b + c) + d should optimize to Sum4",
			{
				SinOsc.kr(1) + (LFNoise0.kr(1) + DC.kr(2)) + Impulse.kr(3)
			} -> [SinOsc, LFNoise0, DC, Impulse, Sum4, Out],
			// currently this is not recognized as Sum4
			// postpone for a later fix
			// "a + b + (c + d) should optimize to Sum4",
			// {
			// 	SinOsc.kr(1) + LFNoise0.kr(1) + (DC.kr(2) + Impulse.kr(3))
			// } -> [SinOsc, LFNoise0, DC, Impulse, Sum4, Out],
			"a + (b + c + d) should optimize to Sum4",
			{
				SinOsc.kr(1) + (LFNoise0.kr(1) + DC.kr(2) + Impulse.kr(3))
			} -> [SinOsc, LFNoise0, DC, Impulse, Sum4, Out],
			"a + b + c + d should not optimize if (a + b + c) has multiple descendants",
			{
				var a = SinOsc.kr(1);
				var b = LFNoise0.kr(1);
				var c = DC.kr(2);
				var d = Impulse.kr(3);
				var abc = a + b + c;
				(abc + d) * (abc / d);
			} -> [SinOsc, LFNoise0, DC, Sum3, Impulse, '+', '/', '*', Out],
			"a + (b + c + d) should not optimize if (b + c + d) has multiple descendants",
			{
				var a = SinOsc.kr(1);
				var b = LFNoise0.kr(1);
				var c = DC.kr(2);
				var d = Impulse.kr(3);
				var bcd = b + c + d;
				(a + bcd) * (a / bcd);
			} -> [SinOsc, LFNoise0, DC, Impulse, Sum3, '+', '/', '*', Out],
			"a + b when a === b should not optimize",
			{
				var x = SinOsc.ar([100, 200, 300]).sum();
				x + x
			} -> [SinOsc, SinOsc, SinOsc, Sum3, '+', Out],
		]);
	}

	test_optimizeToMulAdd {
		this.verifyTestCases([
			"(a * b) + c should optimize to MulAdd",
			{ (SinOsc.kr(1) * LFNoise0.kr(1)) + DC.kr(2) } -> [SinOsc, LFNoise0, DC, MulAdd, Out],
			"a + (b * c) should optimize to MulAdd",
			{ SinOsc.kr(1) + (LFNoise0.kr(1) * DC.kr(2)) } -> [SinOsc, LFNoise0, DC, MulAdd, Out],
			"(a + b) * c should not optimize to MulAdd",
			{ (SinOsc.kr(1) + LFNoise0.kr(1)) * DC.kr(2) } -> [SinOsc, LFNoise0, '+', DC, '*', Out],
			"(a * b) + c should not optimize if 'a * b' has multiple descendants",
			{
				var a = SinOsc.kr(1);
				var b = LFNoise0.kr(1);
				var c = DC.kr(2);
				var ab = a * b;
				// max is contrived to avoid Sum3 or other optimizations
				max(ab + c, ab / c)
			} -> [SinOsc, LFNoise0, '*', DC, '+', '/', 'max', Out],
			"a + b when a === b should not optimize",
			{
				var x = SinOsc.ar(100) * 0.1;
				x + x
			} -> [SinOsc, '*', '+', Out],
		])
	}

	test_optimizeAddNeg {
		this.verifyTestCases([
			"a + b.neg should optimize to a - b",
			{ SinOsc.kr(0.1) + Rand(0, 1).neg } -> [SinOsc, Rand, '-', Out],
			"a.neg + b should optimize to b - a",
			{ SinOsc.kr(0.1).neg + Rand(0, 1) } -> [SinOsc, Rand, '-', Out],
			"a + b.neg should not optimize if neg has multiple descendants",
			{
				var a = SinOsc.kr(0.1);
				var b = Rand(0, 1).neg;
				(a + b) * b
			} -> [SinOsc, Rand, 'neg', '+', '*', Out],
			"a + b when a === b should not optimize",
			{
				var x = SinOsc.ar(100).neg;
				x + x
			} -> [SinOsc, 'neg', '+', Out],
		])
	}

	test_optimizeSub {
		this.verifyTestCases([
			"a - b.neg should optimize to a + b",
			{ SinOsc.kr(0.1) - Rand(0, 1).neg } -> [SinOsc, Rand, '+', Out],
			"a - b.neg should not optimize if neg has multiple descendants",
			{
				var a = SinOsc.kr(0.1);
				var b = Rand(0, 1).neg;
				(a - b) * b
			} -> [SinOsc, Rand, 'neg', '-', '*', Out],
			"a - b when a === b results in a - a",
			{
				var x = SinOsc.ar(100).neg;
				x - x
			} -> [SinOsc, '-', Out],
		])
	}

	test_optimizeToSum3_handles_duplicate_operand {
		this.verifyTestCases([
			"(a+b) + (a+b) should optimize to a Sum4",
			{
				var sig = SinOsc.kr(0.1) + Rand(-0.02, 0.02);
				sig + sig
			} -> [SinOsc, Rand, Sum4, Out]
		]);
	}

	verifyTestCases { |cases|
		cases.pairsDo { |message, test|
			// test is function -> array
			var def = SynthDef(\testMathOp, {
				var sig = test.key.value;
				Out.perform(sig.methodSelectorForRate, 0, sig);
			}),  // don't need to add
			ugens = def.children.collect { |ugen|
				// for math ops, not enough to check class
				if(ugen.isKindOf(BasicOpUGen)) {
					ugen.operator
				} {
					ugen.class
				}
			};
			this.assertEquals(ugens, test.value, message);
		};
	}
}
