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
		this.assert(a == 9, "outer lexical function scope should recieve side-effect");
	}

	test_classmethod_scope {
		var obj, func;
		obj = CommonTestClass;
		obj.x = 5;
		func = obj.returnLexicalScopeGetterFunc;
		this.assert(func.value == [5, 7], "function scope should be able to access class variables");
		func = obj.returnLexicalScopeSetterFunc;
		func.value(42);
		this.assert(obj.x == 42, "function should be able to set class variable");
		this.assert(obj.y == 42, "function should be able to set class variable that has no setter");
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
	}

	test_plot {
		var condition = CondVar.new;
		var server = Server(thisMethod.name);
		server.bootSync;
		{ |x| DC.ar(x) }.asBuffer(duration: 0.01, target: server, action: { |b|
			b.get(3, { |val|
				this.assertEquals(val, 0, "unspecified function arguments should pass as 0 when function is written to a buffer");
				b.free;
				condition.signalOne;
			})
		});
		condition.waitFor(2);
		server.quit;
		server.remove;
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

}


