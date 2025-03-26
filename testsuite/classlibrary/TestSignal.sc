/*
Signal.test
UnitTest.gui
*/
TestSignal : UnitTest {

	test_cheby {
		var lin = Signal.chebyFill(1000, [1]);
		var sqr = Signal.chebyFill(101, [0, 1]);
		var norm = Signal.chebyFill(256, [3, 2, -1.5, -4.5, -2.2, 1], normalize: true, zeroOffset: false);
		var offset = Signal.chebyFill(15, [0, 1, 0.5, -0.25], normalize: false, zeroOffset: true);

		this.assert((lin.first == -1) && this.getSignalMidValue(lin).equalWithPrecision(0) && (lin.last == 1), "Order 1 Cheby Signal should go from -1 to 1 by default (normalized, not zero-offset)");

		this.assert((sqr.first == 1) && this.getSignalMidValue(sqr).equalWithPrecision(-1) && (sqr.last == 1), "Order 2 Cheby Signal should be a parabola between -1 and 1 by default (normalized, not zero-offset)");

		this.assert(norm.abs.maxItem.equalWithPrecision(1), "Normalized, non-offset Cheby Signal should have maximum absolute value of 1");

		this.assert(this.getSignalMidValue(offset).equalWithPrecision(0), "A zeroOffset Cheby Signal should have value 0 for its middle value");

	}

	test_hanningWindow_max1 {
		var window = Signal.hanningWindow(1024);
		this.assertFloatEquals(window.maxItem, 1.0, "Signal.hanningWindow has maximum of 1.0 for even window size");
	}

	test_hammingWindow_max1 {
		var window = Signal.hammingWindow(1024);
		this.assertFloatEquals(window.maxItem, 1.0, "Signal.hammingWindow has maximum of 1.0 for even window size");
	}

	test_rectWindow_max1 {
		var window = Signal.rectWindow(1024);
		this.assertFloatEquals(window.maxItem, 1.0, "Signal.rectWindow has maximum of 1.0");
	}

	test_welchWindow_max1 {
		var window = Signal.welchWindow(1024);
		this.assertFloatEquals(window.maxItem, 1.0, "Signal.welchWindow has maximum of 1.0 for even window size");
	}

	// This is just a helper method
	getSignalMidValue { arg sig;
		var midIndex = sig.size >> 1;
		if (sig.size.odd) {
			^sig[midIndex]
		};
		^( sig[midIndex-1] + sig[midIndex] / 2)
	}

	test_equality_true {
		var test = 10.collect { |i| Signal.sineFill(64, [i]) == Signal.sineFill(64, [i]) }.asSet;
		this.assertEquals(test, Set[true], "Identical signals are equal");
	}

	test_equality_false {
		var test = 10.collect { |i| Signal.sineFill(64, [i]) != Signal.sineFill(64, [-1]) }.asSet;
		this.assertEquals(test, Set[true], "Non-identical signals are not equal");
	}

	test_pow_xf {
		var signal = Signal[1.5, 3.0, -2.0, 0.0];
		var array = FloatArray[1.5, 3.0, -2.0, 0.0];
		var expo = 2.1;
		var arraysign = array.collect { |base| if (base.isPositive, {1}, {-1}) };

		var signalresult = signal.pow(expo);
		var arrayresult = array.abs.pow(expo) * arraysign;
		this.assertEquals(signalresult.as(FloatArray), arrayresult.as(FloatArray), "Signal to power of Float should match 32-bit result of FloatArray to power of Float while preserving sign of bases");

		signal = Signal[10.5, 30.0, -71.0, 50.1];
		array = FloatArray[10.5, 30.0, -71.0, 50.1];
		expo = 4.1;
		arraysign = array.collect { |base| if (base.isPositive, {1}, {-1}) };

		signalresult = signal.pow(expo);
		arrayresult = array.abs.pow(expo) * arraysign;
		this.assertEquals(signalresult.as(FloatArray), arrayresult.as(FloatArray), "For large values, Signal to power of Float should match 32-bit result of FloatArray to power of Float while preserving sign of bases");
	}

	test_pow_fx {
		var signal = Signal[1.0, -3.0, -2.0, 0.0];
		var array = FloatArray[1.0, -3.0, -2.0, 0.0];
		var base = -3.1;
		var basesign = if (base.isPositive, {1}, {-1});

		var signalresult = base.pow(signal);
		var arrayresult = base.abs.pow(array) * basesign;
		this.assertEquals(signalresult.as(FloatArray), arrayresult.as(FloatArray), "Float to power of Signal should match 32-bit result of Float to power of FloatArray while preserving sign of base");

		signal = Signal[70.5, -32.0, -70.5, 32.0, 628.0];
		array = FloatArray[70.5, -32.0, -70.5, 32.0, 628.0];
		base = 3.1;
		basesign = if (base.isPositive, {1}, {-1});

		signalresult = base.pow(signal);
		arrayresult = base.abs.pow(array) * basesign;
		this.assertEquals(signalresult.as(FloatArray), arrayresult.as(FloatArray), "For large values, Float to power of Signal should match 32-bit result of Float to power of FloatArray while preserving sign of base");
	}

	test_pow_xx {
		var signal = Signal[1.5, 3.0, -2.0, 0.0];
		var array = FloatArray[1.5, 3.0, -2.0, 0.0];
		var expo = FloatArray[1.8, 3.1, -2.0, 0.0];
		var arraysign = array.collect { |base| if (base.isPositive, {1}, {-1}) };

		var signalresult = signal.pow(expo.as(Signal));
		var arrayresult = array.abs.pow(expo) * arraysign;
		this.assertEquals(signalresult.as(FloatArray), arrayresult.as(FloatArray), "Signal to power of Signal should match 32-bit result of FloatArray to power of FloatArray while preserving sign of bases");

		signal = Signal[1.5, 3.0, 2.0, -1.1];
		array = FloatArray[1.5, 3.0, 2.0, -1.1];
		expo = FloatArray[20.0, 70.1, -30.2, 22.1]; 
		arraysign = array.collect { |base| if (base.isPositive, {1}, {-1}) };

		signalresult = signal.pow(expo.as(Signal));
		arrayresult = array.abs.pow(expo) * arraysign;
		this.assertEquals(signalresult.as(FloatArray), arrayresult.as(FloatArray), "For large values, Signal to power of Signal should match 32-bit result of FloatArray to power of FloatArray while preserving sign of bases");
	}

	test_mod_xf {
		var signal = Signal[1.0, 3.1, 5.0, -8.0, 2.1];
		var array = FloatArray[1.0, 3.1, 5.0, -8.0, 2.1];
		var modulus = 2.1;

		var signalresult = signal.mod(modulus);
		var arrayresult = array.mod(modulus);
		this.assertEquals(signalresult.as(FloatArray), arrayresult.as(FloatArray), "Signal mod Float should match 32-bit result of FloatArray mod Float");
	}

	test_mod_fx {
		var signal = Signal[1.0, 3.1, 5.0, -8.0, 2.1];
		var array = FloatArray[1.0, 3.1, 5.0, -8.0, 2.1];
		var value = 2.1;

		var signalresult = value.mod(signal);
		var arrayresult = value.mod(array);
		this.assertEquals(signalresult.as(FloatArray), arrayresult.as(FloatArray), "Float mod Signal should match 32-bit result of Float mod FloatArray"); 
	}

	test_mod_xx {
		var signal = Signal[1.0, 3.1, 5.0, -8.0, 2.4];
		var array = FloatArray[1.0, 3.1, 5.0, -8.0, 2.4];
		var modulus = FloatArray[2.1, 8.9, -10.0, -5.0, 1.9];

		var signalresult = signal.mod(modulus.as(Signal));
		var arrayresult = array.mod(modulus);
		this.assertEquals(signalresult.as(FloatArray), arrayresult.as(FloatArray), "Signal mod Signal should match 32-bit result of FloatArray mod FloatArray");
	}
}
