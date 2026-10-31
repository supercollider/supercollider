//# VERSION 3.16
// Classes as the compiler builds them. Short methods become shortcuts the VM runs without code (return this, a
// literal, an argument, an instance or class variable, assign one, or send to this, super or a variable), so each
// shortcut is called with too few, too many and keyword arguments, and through subclasses and extensions. Also class
// variables from class methods, metaclasses, *initClass, primitives that fail into their code, and the methods that
// must not be shortcuts (doesNotUnderstand, Interpreter:functionCompileContext).
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerClasses : UnitTest {

	isShortcut { |class, name| ^class.findMethod(name).code.size == 0 }

	////////////////////////////////////////////////////////////////////////////////
	// Shortcuts
	////////////////////////////////////////////////////////////////////////////////

	test_shortcuts_are_made {
		var c = TestCompilerClassesBase;
		[\x, \x_, \y, \returnSelf, \literal, \symbol, \negative, \argument, \second, \getZ, \setZ, \redirect,
			\redirectSuper, \forward, \forwardClassVariable].do { |name|
			this.assert(this.isShortcut(c, name), "% is a shortcut".format(name));
		};
		[\freshArray, \className, \function, \reset, \notOnlyArgument, \primitive].do { |name|
			this.assert(this.isShortcut(c, name).not, "% has code".format(name));
		};
		this.assert(this.isShortcut(c.class, \classVariable), "*classVariable is a shortcut");
		this.assert(this.isShortcut(c.class, \setClassVariable), "*setClassVariable is a shortcut");
	}

	test_return_self_and_literals {
		var o = TestCompilerClassesBase.new;
		this.assert(o.returnSelf === o, "^this");
		this.assert(o.returnSelf(1, 2, 3) === o, "^this ignores its arguments");
		this.assertEquals([o.literal, o.symbol, o.negative, o.literal(1, 2)], [1.5, \sym, -1, 1.5]);
		this.assertEquals(o.className, Array);
		this.assertEquals(o.function.value, 1);
		this.assert(o.immutableArray === o.immutableArray, "a literal array is the same object each time");
	}

	test_mutable_literal_is_fresh_each_call {
		var o = TestCompilerClassesBase.new, a = o.freshArray, b = o.freshArray;
		this.assert(a !== b, "a new array each call");
		a.put(0, 99);
		this.assertEquals(o.freshArray, [1, 2]);
	}

	test_return_argument {
		var o = TestCompilerClassesBase.new;
		this.assertEquals([o.argument, o.argument(1), o.argument(a: 2), o.argument(1, 2, 3)], [5, 1, 2, 1]);
		this.assertEquals([o.second(1), o.second(1, 2), o.second(b: 3), o.second(a: 3)], [6, 2, 3, 6]);
	}

	test_accessors {
		var o = TestCompilerClassesBase.new;
		this.assertEquals([o.x, o.y], [1, 2]);
		this.assert(o.x_(5) === o, "a setter answers this");
		this.assertEquals(o.x, 5);
		o.x_;
		this.assertEquals(o.x, nil, "a setter called without an argument sets nil");
		o.x = 7;
		this.assertEquals(o.x, 7);
	}

	test_assign_shortcut {
		var o = TestCompilerClassesBase.new;
		this.assert(o.setZ(4) === o, "answers this");
		this.assertEquals(o.getZ, 4);
		this.assertEquals(o.setZ.getZ, 30, "the default");
		this.assertEquals(o.setZ(v: 8).getZ, 8, "a keyword");
		this.assertEquals(o.setZ(1, 2).getZ, 1, "an extra argument");
		this.assertEquals(o.reset.getZ, 0, "storing a literal is not a shortcut, but works");
		this.assertEquals(o.notOnlyArgument(3).getZ, 4);
	}

	test_redirect {
		var o = TestCompilerClassesBase.new;
		this.assertEquals(o.redirect(1), [1, 10, \cdef], "the default of the redirecting method");
		this.assertEquals(o.redirect(1, 2), [1, 2, \cdef]);
		this.assertEquals(o.redirect(1, 2, 3), [1, 2, \cdef], "an extra argument is not passed on");
		this.assertEquals(o.redirect(b: 5), [nil, 5, \cdef]);
		this.assertEquals(o.redirect(a: 4, b: 5), [4, 5, \cdef]);
	}

	test_redirect_finds_the_subclass_method {
		this.assertEquals(TestCompilerClassesBase.new.redirect(1), [1, 10, \cdef]);
		this.assertEquals(TestCompilerClassesDerived.new.redirect(1), [\derived, 1, 10]);
	}

	test_redirect_to_super {
		this.assertEquals(TestCompilerClassesBase.new.redirectSuper(\redirect), true);
		this.assertEquals(TestCompilerClassesBase.new.redirectSuper(\notAMethod), false);
		this.assertEquals(TestCompilerClassesDerived.new.superTarget(1), [1, 10, \cdef]);
		this.assertEquals(TestCompilerClassesDerived.new.superTarget(b: 2), [nil, 2, \cdef]);
	}

	test_forward_to_instance_variable {
		var o = TestCompilerClassesBase.new;
		this.assertEquals(o.forward(1), [\inner, 1, 7, \icdef]);
		this.assertEquals(o.forward(1, 2, 3), [\inner, 1, 2, \icdef]);
		this.assertEquals(o.forward(b: 5), [\inner, nil, 5, \icdef]);
		this.assertEquals(o.forward(a: 4), [\inner, 4, 7, \icdef]);
	}

	test_forward_to_class_variable {
		var o = TestCompilerClassesBase.new;
		TestCompilerClassesBase.cv = 10;
		this.assertEquals([o.forwardClassVariable(3), o.forwardClassVariable(30)], [10, 30]);
		this.assertEquals(TestCompilerClassesBase.forwardClassVariable(40), 40);
	}

	test_inherited_instance_variables_have_the_right_slots {
		var d = TestCompilerClassesDerived.new;
		this.assertEquals([d.x, d.y, d.w, d.getZ], [1, 2, \w, 3]);
		d.w = 9;
		d.setZ(8);
		this.assertEquals([d.x, d.y, d.w, d.getZ], [1, 2, 9, 8]);
		this.assertEquals(d.derivedForward(1), [\inner, 1, \idef, \icdef]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Extensions over shortcuts
	////////////////////////////////////////////////////////////////////////////////

	test_extension_overrides_a_shortcut {
		var o = TestCompilerClassesBase.new;
		this.assertEquals(o.overriddenLiteral, \fromExtension);
		this.assertEquals(o.overriddenGetter, \fromExtension);
		this.assertEquals(o.overriddenRedirect, \fromExtension);
		this.assert(this.isShortcut(TestCompilerClassesBase, \overriddenLiteral), "the extension's ^\\sym is a shortcut too");
	}

	test_extension_method_reads_instance_variables {
		var o = TestCompilerClassesBase.new;
		this.assertEquals(o.extensionGetY, 2);
		this.assertEquals(TestCompilerClassesDerived.new.extensionGetY, 2);
		this.assertEquals(o.extensionSetY(5).y, 5);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Class variables, metaclasses and initClass
	////////////////////////////////////////////////////////////////////////////////

	test_class_variables_from_class_and_instance_methods {
		TestCompilerClassesBase.setClassVariable(5);
		this.assertEquals(TestCompilerClassesBase.classVariable, 5);
		this.assertEquals(TestCompilerClassesBase.new.instanceReadsClassVariable, 5);
		TestCompilerClassesBase.new.instanceWritesClassVariable(6);
		this.assertEquals(TestCompilerClassesBase.classVariable, 6);
		this.assert(TestCompilerClassesBase.setClassVariable(7) === TestCompilerClassesBase, "the setter answers the class");
	}

	test_inherited_class_variables {
		TestCompilerClassesBase.setClassVariable(11);
		this.assertEquals(TestCompilerClassesDerived.classVariable, 11, "inherited class method");
		this.assertEquals(TestCompilerClassesDerived.derivedReadsClassVariable, 11, "subclass's own class method");
		this.assertEquals(TestCompilerClassesDerived.new.derivedInstanceReadsClassVariable, 11);
		TestCompilerClassesDerived.derivedWritesClassVariable(12);
		this.assertEquals(TestCompilerClassesBase.classVariable, 12, "one variable, shared");
		this.assertEquals([TestCompilerClassesBase.ownCv, TestCompilerClassesDerived.ownCv, TestCompilerClassesDerived.derivedCv], [\base, \base, \derived]);
	}

	test_constants {
		this.assertEquals(TestCompilerClassesBase.constant, 42);
		this.assertEquals(TestCompilerClassesBase.new.instanceReadsConstant, 42);
	}

	test_metaclasses {
		var c = TestCompilerClassesBase;
		this.assert(c.returnClass === c, "this in a class method is the class");
		this.assert(c.returnClass.class === c.class, "its class is the metaclass");
		this.assertEquals(c.class.name, \Meta_TestCompilerClassesBase);
		this.assert(c.class.findMethod(\returnClass).notNil, "class methods live in the metaclass");
		this.assert(c.findMethod(\returnClass).isNil, "not in the class");
		this.assert(TestCompilerClassesDerived.returnClass === TestCompilerClassesDerived, "inherited class method");
		this.assertEquals(c.class.superclass, Meta_Object);
		this.assertEquals(TestCompilerClassesDerived.class.superclass, c.class);
	}

	test_new_through_super_in_class_methods {
		var d = TestCompilerClassesDerived.new;
		this.assertEquals(d.class, TestCompilerClassesDerived);
		this.assertEquals(d.forward(1), [\inner, 1, 7, \icdef], "Base's *new ran init");
	}

	test_init_class {
		this.assertEquals(TestCompilerClassesBase.initLog, [\base, \derived], "the base first, through initClassTree");
	}

	////////////////////////////////////////////////////////////////////////////////
	// Primitives
	////////////////////////////////////////////////////////////////////////////////

	test_primitive_succeeds_or_runs_its_code {
		this.assertEquals([10, 20].testCompilerClassesAt(0), 10);
		this.assertEquals([10, 20].testCompilerClassesAt(\notAnIndex), [\fallback, \notAnIndex, 3]);
		this.assertEquals([10, 20].testCompilerClassesAt, 20, "the default reaches the primitive");
		this.assertEquals([10, 20].testCompilerClassesAt(i: 0), 10, "a keyword reaches the primitive");
		this.assertEquals([10, 20].testCompilerClassesAt(i: \no), [\fallback, \no, 3], "and its code");
	}

	test_primitive_code_with_hoisted_variables {
		this.assertEquals([10, 20].testCompilerClassesAtHoisted(\no), 6);
		this.assertEquals([10, 20].testCompilerClassesAtHoisted(\no).class, Integer);
		this.assertEquals([10, 20].testCompilerClassesAtHoisted(1), 20);
	}

	test_primitive_method_on_an_instance {
		var o = TestCompilerClassesBase.new;
		this.assertEquals(o.primitive(o), true);
		this.assertEquals(o.primitive(1), false);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Methods that must keep their code
	////////////////////////////////////////////////////////////////////////////////

	test_does_not_understand_is_never_a_shortcut {
		this.assertEquals(TestCompilerClassesDnuArgument.new.foo, \foo);
		this.assertEquals(TestCompilerClassesDnuArgument.new.bar(1, 2), \bar);
		this.assertEquals(TestCompilerClassesDnuLiteral.new.foo, 42);
		this.assertEquals(TestCompilerClassesDnuEmpty.new.foo.class, TestCompilerClassesDnuEmpty);
		this.assertEquals(TestCompilerClassesDnuRedirect.new.foo(3), [\handled, \foo]);
		[TestCompilerClassesDnuArgument, TestCompilerClassesDnuLiteral, TestCompilerClassesDnuEmpty,
			TestCompilerClassesDnuRedirect].do { |class|
			this.assert(this.isShortcut(class, \doesNotUnderstand).not, "%:doesNotUnderstand has code".format(class));
		};
	}

	test_function_compile_context_is_not_a_shortcut {
		this.assert(this.isShortcut(Interpreter, \functionCompileContext).not, "it has code");
		this.assertEquals("1 + 1".interpret, 2);
	}
}

TestCompilerClassesBase {
	var <>x = 1, <y = 2, z = 3, inner;
	classvar <>cv = 10, classVar = 0, <initLog, <ownCv = \base;
	const <constant = 42;

	*initClass { initLog = initLog.add(\base) }
	*new { ^super.new.init }
	init { inner = TestCompilerClassesInner.new }

	returnSelf { ^this }
	literal { ^1.5 }
	symbol { ^\sym }
	negative { ^-1 }
	immutableArray { ^#[1, 2] }
	freshArray { ^[1, 2] }
	className { ^Array }
	function { ^{ 1 } }
	argument { |a = 5| ^a }
	second { |a, b = 6| ^b }
	getZ { ^z }
	setZ { |v = 30| z = v }
	reset { z = 0 }
	notOnlyArgument { |v| z = v + 1 }
	redirect { |a, b = 10| ^this.target(a, b) }
	target { |a, b = \tdef, c = \cdef| ^[a, b, c] }
	redirectSuper { |a| ^super.respondsTo(a) }
	forward { |a, b = 7| ^inner.target(a, b) }
	forwardClassVariable { |a| ^cv.max(a) }
	primitive { |other| _Identical; ^\failed }

	overriddenLiteral { ^\fromClass }
	overriddenGetter { ^x }
	overriddenRedirect { ^this.literal }

	instanceReadsClassVariable { ^classVar }
	instanceWritesClassVariable { |v| classVar = v }
	instanceReadsConstant { ^constant }

	*classVariable { ^classVar }
	*setClassVariable { |v| classVar = v }
	*forwardClassVariable { |a| ^cv.max(a) }
	*returnClass { ^this }
}

TestCompilerClassesDerived : TestCompilerClassesBase {
	var <>w = \w;
	classvar <derivedCv = \derived;

	*initClass {
		Class.initClassTree(TestCompilerClassesBase);
		TestCompilerClassesBase.prAddToInitLog(\derived);
	}

	target { |a, b, c| ^[\derived, a, b] }
	superTarget { |a, b = 10| ^super.target(a, b) }
	derivedForward { |a| ^inner.target(a) }
	derivedInstanceReadsClassVariable { ^classVar }
	*derivedReadsClassVariable { ^classVar }
	*derivedWritesClassVariable { |v| classVar = v }
}

TestCompilerClassesInner {
	target { |a, b = \idef, c = \icdef| ^[\inner, a, b, c] }
}

TestCompilerClassesDnuArgument { doesNotUnderstand { |selector| ^selector } }
TestCompilerClassesDnuLiteral { doesNotUnderstand { ^42 } }
TestCompilerClassesDnuEmpty { doesNotUnderstand { } }
TestCompilerClassesDnuRedirect {
	doesNotUnderstand { |selector| ^this.handle(selector) }
	handle { |selector| ^[\handled, selector] }
}

+ TestCompilerClassesBase {
	overriddenLiteral { ^\fromExtension }
	overriddenGetter { ^\fromExtension }
	overriddenRedirect { ^\fromExtension }
	extensionGetY { ^y }
	extensionSetY { |v| y = v }
	*prAddToInitLog { |tag| initLog = initLog.add(tag) }
}

+ Array {
	testCompilerClassesAt { |i = 1| _BasicAt; ^[\fallback, i, 3] }
	testCompilerClassesAtHoisted { |i| _BasicAt; var v = 1; if (i.isKindOf(Symbol)) { var h = 5; v = v + h }; ^v }
}
