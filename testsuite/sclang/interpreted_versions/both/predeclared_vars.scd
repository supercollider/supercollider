// Interpreted code with every var at the top, the way Legacy requires. It must behave the same under every
// --interpreted-version, so CMakeLists.txt runs it with each. Each case is a string, as interpreted code is a string.
var failures = 0;
var check = { |source, expected|
	var got = try { source.interpret } { |e| e.errorString };
	if (got != expected) {
		"FAILED: % gave %, expected %".format(source.quote, got, expected).postln;
		failures = failures + 1;
	};
};

check.value("var a; a", nil);
check.value("var a = 5, b; [a, b]", [5, nil]);
check.value("var a, b = 5; [a, b]", [nil, 5]);
check.value("var a, b; a = 1; b = a + 1; [a, b]", [1, 2]);
check.value("var a = 1; var b = a + 1; b", 2);
check.value("var r = List.new; 3.do { |i| var c; c = c ? 0; r.add(c + i) }; r.asArray", [0, 1, 2]);
check.value("var r = List.new, i = 0; while { i < 3 } { var c; c = c ? 10; r.add(c); i = i + 1 }; r.asArray", [10, 10, 10]);
check.value("var fs = List.new; 3.do { |i| var c; c = i * 10; fs.add({ c }) }; fs.collect { |f| f.value }.asArray", [0, 10, 20]);
check.value("var count = 0, inc; inc = { count = count + 1 }; inc.value; inc.value; count", 2);
check.value("var x = 1; if (true) { var y; y = x + 1; x = y }; x", 2);
check.value("var x = 3; { var x; x = 10; x }.value + x", 13);
check.value("var f; f = { |n| if (n < 2) { n } { f.value(n - 1) + f.value(n - 2) } }; f.value(10)", 55);
check.value("var x = 2; switch (x) { 1 } { \\one } { 2 } { \\two } { \\many }", \two);
check.value("var x = 0; case { x < 0 } { \\neg } { x == 0 } { \\zero } { \\pos }", \zero);
check.value("var a = true, b = false; [a and: { b }, a or: { b }, a && b, a || b]", [false, true, false, true]);
check.value("var s = \"\"; 4.do { |i| s = s ++ i.asString }; s", "0123");
check.value("var f = { |a, b = 3| a + b }; [f.value(1), f.value(1, 5)]", [4, 6]);
check.value("var f = { |a ...rest| rest.size }; f.value(1, 2, 3)", 2);
check.value("var a = 1; a = a + 1; a = a * 3; a", 6);
check.value("var o = Object.new; o.isNil", false);

if (failures > 0) { "% interpreted cases failed".format(failures).postln; 1.exit } { 0.exit };
