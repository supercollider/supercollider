TestNamedControl : UnitTest {

	test_default_value_in_single_instance {
		var defaultValues;
		SynthDef(\test, {
			var a = NamedControl(\x);
			defaultValues = a.source.values;
		});
		this.assertEquals(defaultValues, [0.0], "NamedControl: default value is zero")
	}

	test_default_value_in_multiple_instances {
		var defaultValues;
		SynthDef(\test, {
			var a = NamedControl(\x);
			var b = NamedControl(\x);
			defaultValues = a.source.values;
		});
		this.assertEquals(defaultValues, [0.0], "NamedControl: default value is zero in multiple instances within the same UGen graph")
	}

	test_unique_value_in_multiple_instances {
		var defaultValues;
		SynthDef(\test, {
			var a = NamedControl(\x, [1, 2, 3]);
			var b = NamedControl(\x);
			defaultValues = a.source.values;
		});
		this.assertEquals(defaultValues, [1, 2, 3], "NamedControl: multiple instances should have the same value, if one is given")
	}

	test_equivalent_value_in_multiple_instances {
		var defaultValues;
		SynthDef(\test, {
			var a = NamedControl(\x, [1, 2, 3]);
			var b = NamedControl(\x, [1.0, 2.0, 3.0]);
			defaultValues = a.source.values;
		});
		this.assertEquals(defaultValues, [1, 2, 3], "NamedControl: multiple instances should have the same value, if multiple equivalent ones are given")
	}

	test_incompatible_values {
		var defaultValues, error;
		try {
			SynthDef(\test, {
				var a = NamedControl(\x, [1, 2, 3]);
				var b = NamedControl(\x, [1, 2]);
				defaultValues = a.source.values;
			});
		} { |e| error = e };

		this.assert(error.isException, "NamedControl: incompatible multiple values should throw an error")
	}


}

