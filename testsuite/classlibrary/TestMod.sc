TestMod : UnitTest {
	test_mod_integer {
		var array = [1, (2 ** 30).asInteger, -10, 11, -2, (2 ** 30).asInteger.neg];
		var modulus = [1, (2 ** 30 + 1).asInteger, 12, -3, -6, (2 ** 30 + 100).asInteger.neg];
		this.assertEquals(array.mod(modulus), [0, 1073741824, 2, -1, -2, -1073741824], "mod should give expected results for Integer");
	}

	test_modSeaside_integer {
		var array = [1, (2 ** 30).asInteger, -10, 11, -2, (2 ** 30).asInteger.neg];
		var modulus = [1, (2 ** 30 + 1).asInteger, 12, -3, -6, (2 ** 30 + 100).asInteger.neg];
		this.assertEquals(array.modSeaside(modulus), [0, 1073741824, 2, 2, 4, 100], "modSeaside should give expected results for Integer");
	}

	test_mod_float {
		var array = [1.0, -10.7, 11.3, -13.4, (2 ** -53)];
		var modulus = [1.0, 12.0, -11.4, -6.2, (2 ** -53 - 0.1)];
		var result = array.mod(modulus);
		this.assertArrayFloatEquals(result, [0.0, 1.3, -0.1, -1.0, -0.1], "mod should give expected results for Float");
		this.assertEquals(result, array.modSeaside(modulus), "modSeaside should match mod for Floats");

		this.assertFloatEquals((2 ** 53).mod(2 ** 53 + 1.1), 9.007199254741e+15, "mod should give relatively accurate results for large Floats", 10);
	}

	test_modSeaside_combined {
		var array = [1, (2 ** 30), -12.0, 13.2, 40];
		var modulus = [1, (2 ** 30 + 2).asInteger, -9, -15, -17];
		this.assertArrayFloatEquals(array.modSeaside(modulus), [0, 1073741824.0, -3.0, -1.8, 6], "modSeaside should exhibit desired behaviors for SequenceableCollections with both Floats and Ints");
	}
}
