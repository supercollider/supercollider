TestCorrectArgCountObj {
	var <a, <b, <c;
	*new {|a| ^super.newCopyArgs(a) }
	*newVar {|...a| ^super.newCopyArgs(a) }
	*newVarExpand {|...a| ^super.newCopyArgs(*a) }
}
TestCorrectArgCount : UnitTest {
    test_var_args {
        var o = TestCorrectArgCountObj.newVar(\a, \b, \c);
		this.assertEquals(o.a, [\a, \b, \c], "first argument given in the method definition is passed correctly");
		this.assertEquals(o.b, nil, "arguments absent from method definition should not be passed");
		this.assertEquals(o.c, nil, "arguments absent from method definition should not be passed");
    }
    test_var_args_expanded {
        var o = TestCorrectArgCountObj.newVarExpand(\a, \b, \c);
		this.assertEquals(o.a, \a, "arguments should be expanded.");
		this.assertEquals(o.b, \b, "arguments should be expanded.");
		this.assertEquals(o.c, \c, "arguments should be expanded.");
    }
	test_correct_arg {
		var o = TestCorrectArgCountObj(\a, \b, \c);
		this.assertEquals(o.a, \a, "first argument given in the method definition is passed correctly");
		this.assertEquals(o.b, nil, "arguments absent from method definition should not be passed");
		this.assertEquals(o.c, nil, "arguments absent from method definition should not be passed");
	}
}
