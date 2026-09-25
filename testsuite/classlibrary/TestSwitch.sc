TestSwitch : UnitTest {
	test_nil {
		// This is indeterminate, but quite likely, 50 was more than enough to trigger this bug in practice.
		50.do {
			this.assertEquals(
				switch([], nil, { $0 }, { $1 }),
				$1,
				"having nil in a switch statement shouldn't cause the nil case to be taken sometimes.",
				report: false
			);
		};

		50.do {
			this.assertEquals(
				switch([], {nil}, { $0 }, { $1 }),
				$1,
				"having nil in a switch statement shouldn't cause the nil case to be taken sometimes.",
				report: false
			);
		};

		this.assertEquals(
			switch(nil, nil, { $0 }, { $1 }),
			$0,
			"Switch case of nil should behave correctly."
		);

		this.assertEquals(
			switch(nil, {nil}, { $0 }, { $1 }),
			$0,
			"Switch case of nil should behave correctly."
		);

		this.assertEquals(
			switch(\bang, {nil}, { $0 }, { $1 }),
			$1,
			"Switch case with nil should behave correctly."
		);
	}
}
