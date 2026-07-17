TestNonLocalReturn : UnitTest {
	test_try {
		var before = this.stackDepth;
		var after;
		var f;

		try { f.(1, 2, 3, 4, 5, Error("blah").throw ) };

		after = this.stackDepth;

		this.assertEquals(before, after, "Non local returns should not leave things on the stack");
	}

	test_dont_crash {
		var before = this.stackDepth;
		var after;

		// Caused a stack overflow and segfault.
		// see issue 7448
		1000.do{  try {  ()[nil[\a]] }  };

		after = this.stackDepth;

		this.assertEquals(before, after, "Non local returns should not leave things on the stack");
	}

	test_old_issue_from_2012 {
		// see issue 232
		var before = this.stackDepth;
		var after;
		var x = [nil];
		var r = x.do({ |item|
			try { max(1, x) == 2 }
		});

		this.assertEquals(r, x);
		after = this.stackDepth;
		this.assertEquals(before, after, "Non local returns should not leave things on the stack");
	}

	test_throw_in_tail_call {
		var before = this.stackDepth;
		var after;

		var r = try {this.tail_call};

		this.assertEquals(r, 2);

		after = this.stackDepth;
		this.assertEquals(before, after, "Non local returns should not leave things on the stack");
	}

	test_throw_in_tail_call_that_throw {
		var before = this.stackDepth;
		var after;

		var r = try { this.tail_call_that_throws };

		this.assertEquals(r, nil);

		after = this.stackDepth;
		this.assertEquals(before, after, "Non local returns should not leave things on the stack");
	}

	test_throw_in_tail_call_things_on_stack {
		var before = this.stackDepth;
		var after;

		var r = try { this.tail_call_that_throws_with_things_on_stack };

		this.assertEquals(r, nil);

		after = this.stackDepth;
		this.assertEquals(before, after, "Non local returns should not leave things on the stack");
	}


	test_throw_in_tail_call_1 {
		var before = this.stackDepth;
		var after;

		var r = try { nil.(1, 2, 3, this.tail_call) };

		this.assertEquals(r, nil);

		after = this.stackDepth;
		this.assertEquals(before, after, "Non local returns should not leave things on the stack");
	}

	test_throw_in_tail_call_that_throw_1 {
		var before = this.stackDepth;
		var after;

		var r = try { nil.(1, 2, 3, this.tail_call_that_throws) };

		this.assertEquals(r, nil);

		after = this.stackDepth;
		this.assertEquals(before, after, "Non local returns should not leave things on the stack");
	}

	test_throw_in_tail_call_things_on_stack_1 {
		var before = this.stackDepth;
		var after;

		var r = try { nil.(1, 2, 3, this.tail_call_that_throws_with_things_on_stack) };

		this.assertEquals(r, nil);

		after = this.stackDepth;
		this.assertEquals(before, after, "Non local returns should not leave things on the stack");
	}


	tail_call{ ^1 + 1 }
	tail_call_that_throws { ^Error("throwing").throw }
	tail_call_that_throws_with_things_on_stack {
		// Yes this is weird because 1 is on the stack.
		// As of 2026, Function.forkIfNeeded does this.
		^if(false, 1, {Error("throwing").throw})
	}
}
