// Writes every class library method's properties, bytecode, selectors and constants, nested functions after their method, to the
// file given as the first argument, for compare_bytecode.py.
// usage: sclang -l conf.yaml dump_old_bytecode.scd out.txt
(
var hex = { |b| (b & 255).asHexString(2) };
var dumpFn, file = File(thisProcess.argv[0], "w");
dumpFn = { |name, f|
	file.write("FUNC %\n".format(name));
	// needsHeapContext isn't readable, the old compiler set it when a function it creates has a context.
	file.write("  properties:%\n".format(
		(if (f.context.notNil) { " context," } { "" })
		++ (if (f.isClosed == true) { " closed," } { "" })
		++ (if (f.selectors.asArray.any { |s| s.isKindOf(FunctionDef) and: { s.context.notNil } }) { " heap context," } { "" })
	));
	file.write("  code: %\n".format(f.code !? { f.code.collect(hex).join(" ") }));
	file.write("  selectors: %\n".format(f.selectors.asArray.collect({ |s| if (s.isKindOf(FunctionDef)) { "function" } { s.asString } }).join(", ")));
	file.write("  constants: %\n".format(f.constants.asArray.collect({ |c| c.cs.replace("\n", "\\n").replace("\t", "\\t") }).join(", ")));
	f.selectors.asArray.do { |s| if (s.isKindOf(FunctionDef)) { dumpFn.("  nested", s) } };
};
try {
	Class.allClasses.do { |c|
		file.write("CLASS %\n".format(c.name));
		c.methods.do { |m| try { dumpFn.(m.name, m) } { |e| file.write("ERROR %\n".format(e.errorString)) } };
	};
} { |e| e.reportError };
file.close;
0.exit;
)
