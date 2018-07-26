TestNamedControl : UnitTest {

	test_defaultValue_inSingleInstance_isZero {
		var defaultValues;
		SynthDef(\test, {
			var a = NamedControl(\x);
			defaultValues = a.source.values;
		});
		this.assertEquals(defaultValues, [0.0])
	}

	test_defaultValue_inMultipleInstances_isZero {
		var defaultValues;
		SynthDef(\test, {
			var a = NamedControl(\x);
			var b = NamedControl(\x);
			defaultValues = a.source.values;
		});
		this.assertEquals(defaultValues, [0.0])
	}

	values_acrossMultipleInstances_areSame {
		var defaultValues;
		SynthDef(\test, {
			var a = NamedControl(\x, [1, 2, 3]);
			var b = NamedControl(\x);
			defaultValues = a.source.values;
		});
		this.assertEquals(defaultValues, [1, 2, 3])
	}

	values_acrossMultipleInstances_withEquivalentValues_areSame {
		var defaultValues;
		SynthDef(\test, {
			var a = NamedControl(\x, [1, 2, 3]);
			var b = NamedControl(\x, [1.0, 2.0, 3.0]);
			defaultValues = a.source.values;
		});
		this.assertEquals(defaultValues, [1, 2, 3])
	}

	test_incompatibleValues_acrossMultipleInstances_throwsError {
		var defaultValues, error;
		this.assertException({
			SynthDef(\test, {
				var a = NamedControl(\x, [1, 2, 3]);
				var b = NamedControl(\x, [1, 2]);
				defaultValues = a.source.values;
			});
		}, Error)


	}


}

