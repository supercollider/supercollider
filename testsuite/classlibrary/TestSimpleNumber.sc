
TestSimpleNumber : UnitTest {

	test_snap {
		var sourceVal = [ 0, 0.2, 0.4, 0.6, 0.8, 1, 1.2, 1.4, 1.6, 1.8, 2 ];
		var testF = {|g, t, s| val.collect({|num| num.snap(g, t, s)})};

		this.assertEquals(testF.(1,0,1), [ 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2 ], "Snap test 1 failed.");

		this.assertEquals(testF.(0.5,0,1), [ 0, 0, 0.5, 0.5, 1, 1, 1, 1.5, 1.5, 2, 2 ], "Snap test 2 failed.");

		this.assertEquals(testF.(1,0.3,1), [ 0, 0.2, 0, 1, 0.8, 1, 1.2, 1, 2, 1.8, 2 ], "Snap test 3 failed.");

		this.assertEquals(testF.(1,0,0.5), [ 0, 0.1, 0.2, 0.8, 0.9, 1, 1.1, 1.2, 1.8, 1.9, 2 ], "Snap test 4 failed.");

		this.assertEquals(testF.(1,0,0.5), [ 0, 0.2, 0.2, 0.8, 0.8, 1, 1.2, 1.2, 1.8, 1.8, 2 ], "Snap test 4 failed.");

	}

}