//# VERSION legacy
// Classes compiled as Legacy, used by TestMixedVersions to check that classes of different language versions work
// together. MixedModernAlgos in TestMixedVersions.sc has the same method text as MixedLegacyAlgos.

MixedLegacyBase {
	var <>tag, <>count;
	classvar <>shared;

	*new { |argTag| ^super.new.init(argTag) }
	init { |argTag| tag = argTag; count = 0 }
	bump { count = count + 1; ^this.hook }
	hook { ^\base }
	run { |func| ^func.value(this) }
	*makeShared { shared = [\legacy]; ^shared }
	*classHook { ^\legacyClassHook }
	*viaClassHook { ^this.classHook }
	firstGreater { |list, limit|
		list.do { |item| if (item > limit) { ^item } };
		^nil
	}
}

MixedLegacyLeaf : MixedLegacyBase {
	hook { ^\legacyLeaf }
	viaSuper { ^super.hook }
	bumpTwice { this.bump; ^this.bump }
}

MixedLegacyOverModern : MixedModernBase {
	hook { ^\legacyOverModern }
	viaSuper { ^super.hook }
	*classHook { ^\legacyClassOverModern }
}

MixedLegacyAlgos {
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

+ MixedModernBase {
	extendedFromLegacy { ^tag ++ ".legacy" }
}
