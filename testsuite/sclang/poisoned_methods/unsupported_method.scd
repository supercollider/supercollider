// A method the compiler can't compile yet throws a PoisonedMethodError when called, rather than being left out and
// failing with doesNotUnderstand. It exits with 1 if a case fails.
var failures = 0;
var check = { |name, got, expected|
	if (got != expected) {
		"FAILED: % gave %, expected %".format(name, got, expected).postln;
		failures = failures + 1;
	};
};
var stub = UnsupportedMethodStub.new;
var caught;

check.value("a method without the problem", stub.fine, 1);
check.value("the method exists", UnsupportedMethodStub.findRespondingMethodFor(\twoVariadic).notNil, true);

caught = try { stub.twoVariadic([1], [2]) } { |e| e };
check.value("the error class", caught.class, PoisonedMethodError);
check.value("the error's method", caught.method, UnsupportedMethodStub.findMethod(\twoVariadic));
check.value("the message names the method", caught.errorString.find("UnsupportedMethodStub:twoVariadic").notNil, true);

// Its arguments aren't read, so however many it is called with is fine.
check.value("no arguments", try { stub.twoVariadic } { |e| e.class }, PoisonedMethodError);
check.value("many arguments", try { stub.twoVariadic(1, 2, 3, 4) } { |e| e.class }, PoisonedMethodError);

if (failures > 0) { 1.exit };
0.exit;
