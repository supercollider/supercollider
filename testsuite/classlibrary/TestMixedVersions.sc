//# VERSION 3.16
// Classes of different language versions are used together here, in both directions. The language version is a
// property of the class (or extension) only, so inheritance, super calls, class variables, closures and non-local
// returns must not notice where a class was compiled. MixedLegacy.sc holds the Legacy classes, `//# VERSION legacy`.

MixedModernBase {
	var <>tag, <>count;
	classvar <>shared;

	*new { |tag| ^super.new.init(tag) }
	init { |argTag| tag = argTag; count = 0 }
	bump { count = count + 1; var h = this.hook; ^h }
	hook { ^\base }
	run { |func| var wrapped = func; ^wrapped.value(this) }
	*makeShared { shared = [\modern]; var s = shared; ^s }
	*classHook { ^\modernClassHook }
	*viaClassHook { ^this.classHook }
	firstGreater { |list, limit|
		list.do { |item| var big = item > limit; if (big) { ^item } };
		^nil
	}
	pair { var #a, b = [tag, count]; ^[a, b] }
	doubled { |list| ^all {: x * 2, x <- list } }
	adder { ^#{ |a, b| a + b } }
	tagReader { ^#{ |o| o.tag } }
}

MixedModernLeaf : MixedModernBase {
	hook { ^\modernLeaf }
	viaSuper { ^super.hook }
	bumpTwice { this.bump; var second = this.bump; ^second }
}

MixedModernOverLegacy : MixedLegacyBase {
	hook { var h = \modernOverLegacy; ^h }
	viaSuper { var h = super.hook; ^h }
	*classHook { ^\modernClassOverLegacy }
}

MixedModernAlgos {
	// Written the Legacy way: every var at the top of the body. The two classes hold the same text.

	predeclared { |n|
		var a, b = 5, total;
		var result = List.new;
		total = 0;
		n.do { |i|
			var sq, seen;
			sq = i * i;
			seen = seen ? 0;
			total = total + sq;
			result.add(seen);
		};
		^[a, b, total, result.asArray]
	}

	predeclaredInBranch { |flag|
		var x, y = 1;
		if (flag) {
			var inner;
			inner = y + 1;
			x = inner;
		} {
			x = y;
		};
		^[x, y]
	}

	predeclaredReinitialised { |n|
		var counts = List.new, i = 0;
		while { i < n } {
			var c;
			c = c ? 0;
			c = c + 1;
			counts.add(c);
			i = i + 1;
		};
		^counts.asArray
	}

	closures { |n|
		var funcs = List.new;
		n.do { |i|
			var c;
			c = i * 10;
			funcs.add({ c });
		};
		^funcs.collect { |f| f.value }.asArray
	}

	sharedCounter {
		var count, inc, get;
		count = 0;
		inc = { count = count + 1 };
		get = { count };
		inc.value; inc.value;
		^get.value
	}

	shadowing { |tag|
		var count;
		count = tag.size;
		^[count, tag]
	}

	fib { |n|
		if (n < 2) { ^n };
		^this.fib(n - 1) + this.fib(n - 2)
	}

	find { |list, target|
		list.do { |item, i| if (item == target) { ^i } };
		^nil
	}

	nestedReturn { |list|
		list.do { |a|
			list.do { |b|
				if ((a + b) == 7) { ^[a, b] };
			};
		};
		^nil
	}

	classify { |x|
		^switch (x)
			{ 1 } { \one }
			{ 2 } { \two }
			{ \many }
	}

	caseOf { |x|
		^case
			{ x < 0 } { \negative }
			{ x == 0 } { \zero }
			{ \positive }
	}

	andOr { |a, b|
		^[a and: { b }, a or: { b }, a && b, a || b]
	}

	build { |n|
		var str = "", arr = [];
		n.do { |i| str = str ++ i.asString; arr = arr ++ [i * 2] };
		^[str, arr]
	}

	applyTwice { |func, x|
		^func.value(func.value(x))
	}
}

MixedModern316Algos {

	// The methods of MixedLegacyAlgos written the 3.16 way: vars where they are first used.

	predeclared { |n|
		var a;
		var b = 5;
		var total = 0;
		var result = List.new;
		n.do { |i|
			var seen;
			var sq = i * i;
			seen = seen ? 0;
			total = total + sq;
			result.add(seen);
		};
		^[a, b, total, result.asArray]
	}

	predeclaredInBranch { |flag|
		var x;
		var y = 1;
		if (flag) {
			var inner = y + 1;
			x = inner;
		} {
			x = y;
		};
		^[x, y]
	}

	predeclaredReinitialised { |n|
		var counts = List.new;
		var i = 0;
		while { i < n } {
			var c;
			c = c ? 0;
			c = c + 1;
			counts.add(c);
			i = i + 1;
		};
		^counts.asArray
	}

	closures { |n|
		var funcs = List.new;
		n.do { |i|
			var c = i * 10;
			funcs.add({ c });
		};
		^funcs.collect { |f| f.value }.asArray
	}

	sharedCounter {
		var count = 0;
		var inc = { count = count + 1 };
		var get = { count };
		inc.value; inc.value;
		^get.value
	}

	shadowing { |tag|
		var count = tag.size;
		^[count, tag]
	}

	fib { |n|
		if (n < 2) { ^n };
		^this.fib(n - 1) + this.fib(n - 2)
	}

	find { |list, target|
		list.do { |item, i| if (item == target) { ^i } };
		^nil
	}

	nestedReturn { |list|
		list.do { |a|
			list.do { |b|
				var total = a + b;
				if (total == 7) { ^[a, b] };
			};
		};
		^nil
	}

	build { |n|
		var str = "";
		var arr = [];
		n.do { |i| str = str ++ i.asString; arr = arr ++ [i * 2] };
		^[str, arr]
	}

	pack { |list|
		var #first, second = list;
		^[first, second]
	}

	generated { |list|
		^all {: [x, y], x <- list, var y = x * x, y.odd }
	}
}

+ MixedLegacyBase {
	extendedFromModern { var s = tag ++ ".modern"; ^s }
	pair { var #a, b = [tag, count]; ^[a, b] }
}

TestMixedVersions : UnitTest {

	// Inheritance

	test_modern_subclass_of_legacy {
		var o = MixedModernOverLegacy.new("m");
		this.assertEquals(o.tag, "m", "legacy init ran from a modern subclass");
		this.assertEquals(o.bump, \modernOverLegacy, "a modern override is called from a legacy method");
		this.assertEquals(o.count, 1, "the inherited instance variable is shared");
		this.assertEquals(o.viaSuper, \base, "super reaches the legacy method");
	}

	test_legacy_subclass_of_modern {
		var o = MixedLegacyOverModern.new("l");
		this.assertEquals(o.tag, "l");
		this.assertEquals(o.bump, \legacyOverModern, "a legacy override is called from a modern method");
		this.assertEquals(o.count, 1);
		this.assertEquals(o.viaSuper, \base);
	}

	test_three_levels_alternate {
		var o = MixedLegacyLeaf.new("x");
		this.assertEquals(o.bumpTwice, \legacyLeaf);
		this.assertEquals(o.count, 2);
		o = MixedModernLeaf.new("y");
		this.assertEquals(o.bumpTwice, \modernLeaf);
		this.assertEquals(o.count, 2);
	}

	// Class side

	test_class_variable_and_class_methods {
		this.assertEquals(MixedModernOverLegacy.classHook, \modernClassOverLegacy);
		this.assertEquals(MixedModernOverLegacy.viaClassHook, \modernClassOverLegacy,
			"a legacy class method calls the modern override");
		this.assertEquals(MixedLegacyOverModern.viaClassHook, \legacyClassOverModern,
			"a modern class method calls the legacy override");
		this.assertEquals(MixedLegacyBase.makeShared, [\legacy]);
		this.assertEquals(MixedModernBase.makeShared, [\modern]);
		this.assertEquals(MixedLegacyBase.shared, [\legacy], "each class has its own class variable");
		this.assertEquals(MixedModernBase.shared, [\modern]);
	}

	// Extensions

	test_extension_in_other_version {
		this.assertEquals(MixedLegacyBase.new("a").extendedFromModern, "a.modern");
		this.assertEquals(MixedModernBase.new("b").extendedFromLegacy, "b.legacy");
		this.assertEquals(MixedLegacyLeaf.new("c").extendedFromModern, "c.modern", "and it is inherited");
	}

	// Functions and returns across the boundary

	test_function_made_in_one_version_called_in_the_other {
		var fromModern = { |o| o.tag ++ "!" };
		this.assertEquals(MixedLegacyBase.new("q").run(fromModern), "q!");
		this.assertEquals(MixedModernBase.new("r").run(MixedLegacyBase.new("z").run { |o| { |p| p.tag ++ o.tag } }), "rz");
	}

	test_nonlocal_return_inside_each_version {
		var list = [3, 8, 1, 9];
		this.assertEquals(MixedLegacyBase.new("a").firstGreater(list, 5), 8);
		this.assertEquals(MixedModernBase.new("a").firstGreater(list, 5), 8);
		this.assertEquals(MixedLegacyBase.new("a").firstGreater(list, 50), nil);
		this.assertEquals(MixedModernBase.new("a").firstGreater(list, 50), nil);
	}

	test_nonlocal_return_from_function_passed_across_versions {
		// The ^ returns from this method, whichever version runs the function.
		var result = this.findFirstEven(MixedLegacyBase.new("a"), [1, 3, 4, 5]);
		this.assertEquals(result, 4);
		result = this.findFirstEven(MixedModernBase.new("a"), [1, 3, 4, 5]);
		this.assertEquals(result, 4);
	}

	findFirstEven { |holder, list|
		holder.run { list.do { |x| if (x.even) { ^x } } };
		^\none
	}

	// The same code in both versions

	assertSame316Style { |selector ...args|
		var legacy = MixedLegacyAlgos.new.performList(selector, args);
		var modern = MixedModern316Algos.new.performList(selector, args);
		this.assertEquals(modern, legacy, "% % written the 3.16 way gives the same as Legacy".format(selector, args));
		^legacy
	}

	prepareBoth { |selector ...args|
		^[MixedLegacyAlgos.new.performList(selector, args), MixedModernAlgos.new.performList(selector, args)]
	}

	assertSame { |selector ...args|
		var pair = this.prepareBoth(selector, *args);
		this.assertEquals(pair[1], pair[0], "% % gives the same in both versions".format(selector, args));
		^pair[0]
	}

	test_predeclared_vars {
		this.assertEquals(this.assertSame(\predeclared, 4), [nil, 5, 14, [0, 0, 0, 0]],
			"nil, the initial value, and a var inside the loop that starts at nil every pass");
		this.assertSame(\predeclared, 0);
		this.assertEquals(this.assertSame(\predeclaredInBranch, true), [2, 1]);
		this.assertEquals(this.assertSame(\predeclaredInBranch, false), [1, 1]);
		this.assertEquals(this.assertSame(\predeclaredReinitialised, 3), [1, 1, 1]);
	}

	test_closures_capture {
		this.assertEquals(this.assertSame(\closures, 3), [0, 10, 20]);
		this.assertEquals(this.assertSame(\sharedCounter), 2);
	}

	test_shadowing_an_instance_variable {
		this.assertEquals(this.assertSame(\shadowing, "abc"), [3, "abc"]);
	}

	test_recursion_and_returns {
		this.assertEquals(this.assertSame(\fib, 12), 144);
		this.assertEquals(this.assertSame(\find, [4, 5, 6], 6), 2);
		this.assertEquals(this.assertSame(\find, [4, 5, 6], 7), nil);
		this.assertEquals(this.assertSame(\nestedReturn, [1, 2, 3, 4, 5, 6]), [1, 6]);
		this.assertEquals(this.assertSame(\nestedReturn, [1, 2]), nil);
	}

	test_switch_case_and_or {
		this.assertEquals(this.assertSame(\classify, 1), \one);
		this.assertEquals(this.assertSame(\classify, 2), \two);
		this.assertEquals(this.assertSame(\classify, 9), \many);
		this.assertEquals(this.assertSame(\caseOf, -4), \negative);
		this.assertEquals(this.assertSame(\caseOf, 0), \zero);
		this.assertEquals(this.assertSame(\caseOf, 4), \positive);
		this.assertEquals(this.assertSame(\andOr, true, false), [false, true, false, true]);
		this.assertEquals(this.assertSame(\andOr, true, true), [true, true, true, true]);
	}

	test_building_values {
		this.assertEquals(this.assertSame(\build, 4), ["0123", [0, 2, 4, 6]]);
		this.assertEquals(this.assertSame(\applyTwice, { |x| x * 3 }, 2), 18);
	}

	// The same, with vars where they are first used

	test_316_style_gives_the_legacy_results {
		this.assertEquals(this.assertSame316Style(\predeclared, 4), [nil, 5, 14, [0, 0, 0, 0]]);
		this.assertEquals(this.assertSame316Style(\predeclaredInBranch, true), [2, 1]);
		this.assertEquals(this.assertSame316Style(\predeclaredInBranch, false), [1, 1]);
		this.assertEquals(this.assertSame316Style(\predeclaredReinitialised, 3), [1, 1, 1]);
		this.assertEquals(this.assertSame316Style(\closures, 3), [0, 10, 20]);
		this.assertEquals(this.assertSame316Style(\sharedCounter), 2);
		this.assertEquals(this.assertSame316Style(\shadowing, "abc"), [3, "abc"]);
		this.assertEquals(this.assertSame316Style(\fib, 12), 144);
		this.assertEquals(this.assertSame316Style(\find, [4, 5, 6], 6), 2);
		this.assertEquals(this.assertSame316Style(\nestedReturn, [1, 2, 3, 4, 5, 6]), [1, 6]);
		this.assertEquals(this.assertSame316Style(\nestedReturn, [1, 2]), nil);
		this.assertEquals(this.assertSame316Style(\build, 4), ["0123", [0, 2, 4, 6]]);
	}

	// 3.16 only features in classes that cross the boundary

	test_var_pack_on_both_versions {
		var legacy = MixedLegacyBase.new("a");
		var modern = MixedModernBase.new("b");
		legacy.bump;
		this.assertEquals(legacy.pair, ["a", 1], "an extension with var #a, b on a Legacy class");
		this.assertEquals(modern.pair, ["b", 0]);
		this.assertEquals(MixedModern316Algos.new.pack([1, 2, 3]), [1, 2]);
	}

	test_generator_and_closed_function_in_modern_class {
		this.assertEquals(MixedModernBase.new("a").doubled([1, 2, 3]), [2, 4, 6]);
		this.assertEquals(MixedLegacyOverModern.new("a").doubled([4]), [8], "inherited by a Legacy subclass");
		this.assertEquals(MixedModern316Algos.new.generated([1, 2, 3, 4, 5]), [[1, 1], [3, 9], [5, 25]]);
		this.assertEquals(MixedModernBase.new("a").adder.value(1, 2), 3);
		this.assertEquals(MixedLegacyBase.new("z").run(MixedModernBase.new("a").tagReader), "z", "a closed function run by a Legacy method");
	}
}
