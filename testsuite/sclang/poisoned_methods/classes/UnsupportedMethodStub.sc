//# VERSION 3.16
// The class library posts an error for the method that can't be compiled, and the rest of the class still works.
UnsupportedMethodStub {
	foo { |... a| ^a }
	// Calls with two variadic arguments aren't lowered yet, so this method can't be compiled.
	twoVariadic { |a, b| ^this.foo(*a, *b) }
	fine { ^1 }
}
