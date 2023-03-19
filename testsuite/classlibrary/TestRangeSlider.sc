// TestRangerSlider.sc
// Original Author: Brian Heim (brianlheim@gmail.com)

TestRangeSlider : UnitTest {

	var slider;

	setUp {
		slider = RangeSlider.new;
	}

	tearDown {
		slider.close;
		slider = nil;
	}

	// Sanity check on constructor
	test_defaultRange_is0to1 {
		this.assertFloatEquals(slider.lo, 0);
		this.assertFloatEquals(slider.hi, 1);
	}

	// ------- lo_, hi_ ---------------------------------------

	test_setLo_toHalf {
		slider.lo_(0.5);
		this.assertFloatEquals(slider.lo, 0.5);
	}

	test_setHi_toHalf {
		slider.hi_(0.5);
		this.assertFloatEquals(slider.hi, 0.5);
	}

	test_setLowToHalf_setHighToQuarter_swapsLoAndHi {
		slider.lo_(0.5);
		slider.hi_(0.25);
		this.assertFloatEquals(slider.lo, 0.25);
		this.assertFloatEquals(slider.hi, 0.5);
	}

	// ------- setSpan ----------------------------------------

	test_setSpan_bothHalf {
		slider.setSpan(0.5, 0.5);
		this.assertFloatEquals(slider.lo, 0.5);
		this.assertFloatEquals(slider.hi, 0.5);
	}

	test_setSpan_fromHalfToFullRange {
		slider.setSpan(0.5, 0.5);
		slider.setSpan(0, 1);
		this.assertFloatEquals(slider.lo, 0);
		this.assertFloatEquals(slider.hi, 1);
	}

	// Issue #3616
	test_setSpan_bothEndsCrossingLowerBoundary {
		slider.setSpan(0.5, 0.5);
		slider.setSpan(0, 0.25);
		this.assertFloatEquals(slider.lo, 0);
		this.assertFloatEquals(slider.hi, 0.25);
	}

	test_setSpan_bothEndsCrossingHigherBoundary {
		slider.setSpan(0.5, 0.5);
		slider.setSpan(0.75, 1);
		this.assertFloatEquals(slider.lo, 0.75);
		this.assertFloatEquals(slider.hi, 1);
	}

	// tests using a flipped span
	test_setSpan_fromHalfToFullRange_flipped {
		slider.setSpan(0.5, 0.5);
		slider.setSpan(1, 0);
		this.assertFloatEquals(slider.lo, 0);
		this.assertFloatEquals(slider.hi, 1);
	}

	test_setSpan_bothEndsCrossingLowerBoundary_flipped {
		slider.setSpan(0.5, 0.5);
		slider.setSpan(0.25, 0);
		this.assertFloatEquals(slider.lo, 0);
		this.assertFloatEquals(slider.hi, 0.25);
	}

	test_setSpan_bothEndsCrossingHigherBoundary_flipped {
		slider.setSpan(0.5, 0.5);
		slider.setSpan(1, 0.75);
		this.assertFloatEquals(slider.lo, 0.75);
		this.assertFloatEquals(slider.hi, 1);
	}

	// ------- setDeviation -----------------------------------

	// Similar to issue #3616
	test_setDeviation_bothEndsCrossingLowerBoundary {
		slider.setSpan(0.5, 0.5);
		slider.setDeviation(0.1, 0.1);
		this.assertFloatEquals(slider.lo, 0.0);
		this.assertFloatEquals(slider.hi, 0.2);
	}

	test_setDeviation_bothEndsCrossingHigherBoundary {
		slider.setSpan(0.5, 0.5);
		slider.setDeviation(0.1, 0.8);
		this.assertFloatEquals(slider.lo, 0.7);
		this.assertFloatEquals(slider.hi, 0.9);
	}
}
