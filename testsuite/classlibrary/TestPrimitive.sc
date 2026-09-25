TestPrimitive_Obj {
	noKwargsNoVarArgs { |a, b, c|
		_PrimitiveTestNoKwargsNoVarArg;
		1 + 1;
		^this.primitiveFailed
	}

	noKwargsWithVarArgs { |a, b, c... args|
		_PrimitiveTestNoKwargsWithVarArgs;
		1 + 1;
		^this.primitiveFailed
	}

	kwargsWithVarArgs { |a, b, c... args, kwargs|
		_PrimitiveTestWithKwargs;
		1 + 1;
		^this.primitiveFailed
	}

	//  This should still compile and not crash even though _Meow does not exist
	undefinedPrimitive {
		_Meow
	}
}

TestPrimitive_Obj_NoError {
	noKwargsNoVarArgs { |a, b, c| _PrimitiveTestNoKwargsNoVarArg }
	noKwargsWithVarArgs { |a, b, c... args| _PrimitiveTestNoKwargsWithVarArgs }
	kwargsWithVarArgs { |a, b, c... args, kwargs| _PrimitiveTestWithKwargs }
}

TestPrimitives : UnitTest {
	no_var_args { |obj|
		this.assertEquals(
			obj.noKwargsNoVarArgs(1, 2, 3),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.noKwargsNoVarArgs(a: 1, b: 2, c: 3),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.noKwargsNoVarArgs(1, 2, c: 3),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.noKwargsNoVarArgs(1, 2, 3, 4, 5),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.noKwargsNoVarArgs(1, 2, 3, 4, f: 5),
			[obj, 1, 2, 3]
		);
	}

	with_var_args { |obj| 
		this.assertEquals(
			obj.noKwargsWithVarArgs(1, 2, 3),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.noKwargsWithVarArgs(a: 1, b: 2, c: 3),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.noKwargsWithVarArgs(1, 2, c: 3),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.noKwargsWithVarArgs(1, 2, 3, 4, 5),
			[obj, 1, 2, 3, 4, 5]
		);
		this.assertEquals(
			obj.noKwargsWithVarArgs(1, 2, 3, 4, f: 5),
			[obj, 1, 2, 3, 4]
		);
	}

	with_kw_and_var_args { |obj|
		this.assertEquals(
			obj.kwargsWithVarArgs(1, 2, 3),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.kwargsWithVarArgs(a: 1, b: 2, c: 3),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.kwargsWithVarArgs(1, 2, c: 3),
			[obj, 1, 2, 3]
		);
		this.assertEquals(
			obj.kwargsWithVarArgs(1, 2, 3, 4, 5),
			[obj, 1, 2, 3, 4, 5]
		);
		this.assertEquals(
			obj.kwargsWithVarArgs(1, 2, 3, 4, f: 5),
			[obj, 1, 2, 3, 4, f: 5]
		);
	}

	test_no_kw_no_var_args {
		this.no_var_args(TestPrimitive_Obj());
		this.no_var_args(TestPrimitive_Obj_NoError());
	}

	// basic, with var args
	test_no_kw_with_var_args {
		this.with_var_args(TestPrimitive_Obj());
		this.with_var_args(TestPrimitive_Obj_NoError());
	}

	test_kw_with_var_args {
		this.with_kw_and_var_args(TestPrimitive_Obj());
		this.with_kw_and_var_args(TestPrimitive_Obj_NoError());
	}
	test_undefined {
		var a = TestPrimitive_Obj();
		// should not crash, will post error.
		a.undefinedPrimitive;
	}
}
