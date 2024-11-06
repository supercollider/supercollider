TestSubclassResponsibility : UnitTest {
	throwsSubclassResponsibilityError1 { this.subclassResponsibility(thisMethod) }
	// You don't need the '^' here as it throws, so both bytecodes are needed.
	throwsSubclassResponsibilityError2 { ^this.subclassResponsibility(thisMethod) }

	test_all {
		// This test takes a while as it has to iterate over all methods in every class.
		// It compares the byte code of each method against that from throwsSubclassResponsibilityError[1&2],
		//    therefore it isn't full proof, but in practice, this is how you'd call this method.
		//    Further, this could break if the bytecode changes.
		// This test only tests classes with no subclasses, if a subclassResponsility method is present in a class that
		//    can be instanitated and in the subclass the error is only reported in the subclass.
		//    This is because SuperCollider has no concept of an abstract or interface class.
		//    While this is a draw back, this test still catches many cases so is better than nothing.
		var meth1 = TestSubclassResponsibility.findMethod(\throwsSubclassResponsibilityError1).code;
		var meth2 = TestSubclassResponsibility.findMethod(\throwsSubclassResponsibilityError2).code;

		Class.allClasses
		.reject{ |c|
			c == TestSubclassResponsibility // Don't test this class as we need to get the byte code.
			or: { c.subclasses.size != 0 } // Only test instance classes not abstract class or interfaces.
		}
		.do { |c|
			var methods = ();
			c.collectMethods(methods);
			methods.reject{ |m| m.isNil }.do { |m|
				this.assert(
					boolean: m.code != meth1 and: { m.code != meth2 },
					message: format(
						"%:% throws a subclass responsibility error, please implement it. "
						"Use this.shouldNotImplement(thisMethod) if it isn't possible to implement the method.",
						c.name, m.name
					),
					report: false
				)
			}
		}
	}
}
