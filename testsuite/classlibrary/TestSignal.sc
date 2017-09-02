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

	// This is just a helper method
	getSignalMidValue { arg sig;
		var midIndex = sig.size >> 1;
		if (sig.size.odd) {
			^sig[midIndex]
		};
		^( sig[midIndex-1] + sig[midIndex] / 2)
	}

}
