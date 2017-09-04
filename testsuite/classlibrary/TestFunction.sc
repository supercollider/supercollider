TestFunction : UnitTest {

	test_function_scope {
		var generator, list, result, a;
		generator = {
			var x = 0;
			{ |y| a = x = x + y }
		};
		list = 10.collect(generator);
		result = list.collect { |each, i| each.value(i) };
		this.assert(result == (0..9), "lexical function scope should be independent");
		this.assert(a == 9, "outer lexical function scope should receive side-effect");
		this.reset;
	}

	test_classmethod_scope {
		var obj, func;
		obj = CommonTestClass;
		obj.x = 5;
		func = obj.returnLexicalScopeGetterFunc;
		func.value.postln;
		this.assert(
			func.value == [5, 7], "function scope should be able to access class variables");
		func = obj.returnLexicalScopeSetterFunc;
		func.value(42);
		this.assert(obj.x == 42, "function should be able to set class variable");
		this.assert(obj.y == 42, "function should be able to set class variable that has no setter");
		// restore state of object for repeated test
		obj.reset;
	}

	test_instancemethod_scope {
		var obj, func;
		obj = CommonTestClass.new;
		obj.a = 5;
		func = obj.returnLexicalScopeGetterFunc;
		this.assert(func.value == [5, 3], "function scope should be able to access instance variables");
		func = obj.returnLexicalScopeSetterFunc;
		func.value(42);
		this.assert(obj.a == 42, "function should be able to set instance variable");
		this.assert(obj.b == 42, "function should be able to set instance variable that has no setter");
		this.reset;
	}


}

CommonTestClass {

	var <>a, <b = 3;
	classvar <>x, <y = 7;

	*returnLexicalScopeGetterFunc {
		^{ [x, y] }
	}

	*returnLexicalScopeSetterFunc {
		^{ |val|
			x = val;
			y = val;
		}
	}

	returnLexicalScopeGetterFunc {
		^{ [a, b] }
	}

	returnLexicalScopeSetterFunc {
		^{ |val|
			a = val;
			b = val;
		}
	}

	*reset { x = nil; y = 7; }
	reset { a = nil; b = 3; }
}


