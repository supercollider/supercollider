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

	test_NamedControl_uses_constructor_spec {
		var def;
		var argSpec = [0, 2].asSpec, mdSpec = [0, 5].asSpec;
		def = SynthDef(\test, {
			var control = \name.kr(0, spec: argSpec);
		}, metadata: (
			specs: (
				name: mdSpec
			)
		));
		this.assertEquals(
			def.metadata[\specs][\name].maxval,
			argSpec.maxval,
			"Spec passed into NamedControl.kr should override metadata (testing maxval)"
		)
	}

	test_NamedControl_uses_metadata_spec {
		var def;
		var mdSpec = [0, 5].asSpec;
		def = SynthDef(\test, {
			var control = \name.kr(0);
		}, metadata: (
			specs: (
				// old logic did not replace the ControlSpec object
				// but actually corrupted the object given here!
				// so, without copying, assertEquals passes but with
				// the wrong value
				// copy is necessary for a valid test
				name: mdSpec.copy
			)
		));
		this.assertEquals(
			def.metadata[\specs][\name].maxval,
			mdSpec.maxval,
			"Nil spec in NamedControl.kr should not override metadata (testing maxval)"
		)
	}

}

