// lir
// bytecode
// A poisoned method throws PoisonedMethodError(thisMethod), when the library defines it, whatever it is called with.
PoisonedMethodError {
	*new { |method| ^super.new }
	throw { }
}

Foo {
	// expect: poisoned
	// expect-error: Missing Expression.
	broken { ^(1 + ) }
	// expect: poisoned
	// expect-error: Unexpected Token.
	brokenWithArguments { |a, b| var x = ; ^a }
	// expect: -> 1
	fine { ^1 }
}
