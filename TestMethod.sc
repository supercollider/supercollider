/*
TestMethod.run
UnitTest.gui
*/

TestMethod : UnitTest {

	test_method_argumentString {
		var methodsThatFailed = List.new;
		Object.allSubclasses.do({ arg item;
			item.methods.do({ arg jtem;
				var method = jtem;
				try {
					method.argumentString;
				} {|err|
					".argumentString FAILED for: %".format(method.asString).postln;
					methodsThatFailed.add(method);
				}
			});
		});

		this.assert(
			methodsThatFailed.size == 0,
			"The method argumentString failed for % methods".format(methodsThatFailed.size)
		);
	}

} // End class