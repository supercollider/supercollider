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

	test_classnameAsMethodCrash {
	    try
	    {	    
	        1 fooBar: 2;
	    }
	    {	
		|error|
		    "1 fooBar: 2 threw an exception (no method)".postln;
		error.errorString.postln;
	    };

	    try
	    {	    
	        1 FooBar: 2;
	    }
	    {	
		|error|
		"1 FooBar: 2 threw an exception (FooBar is not a method)".postln;
		error.errorString.postln;
	    };

	    if( Main.versionAtLeast( 3, 8 ),
		{
		    try
		    {	    
			1 Object: 2;
		    }
		    {	
			|error|
			    "1 Object: 2 threw an exception (Object is a class not a method)".postln;
			error.errorString.postln;
		    };
		},
		{
		    "Don't test '1 Object: 2' until rev 3.8 at least (Issue #669)".postln;
		}
	    );
	}

} // End class
