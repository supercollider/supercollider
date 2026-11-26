// Run by CMake from a directory with a non-ASCII name, on a script inside it, with a non-ASCII argument.
// sclang works in UTF-8, so the argument, the working directory and every source path must arrive as UTF-8.
try {
	var failures = List[];
	var check = { |name, ok| if(ok.not) { failures.add(name) } };
	var cwd = File.getcwd;
	var here = thisProcess.nowExecutingPath;
	var compiledPath = { |path| thisProcess.interpreter.compile("{}", path).value.def.filenameSymbol };

	check.("argv", thisProcess.argv == ["ärg ü"]);
	check.("working directory", cwd.contains("ünïcödé"));
	check.("script path", here.notNil and: { File.exists(here) } and: { here.basename == "unicode_paths.scd" });

	// A relative path is made absolute against the working directory; an absolute one is kept as given.
	check.("relative path", compiledPath.('sub/../relative.scd') == (cwd +/+ "relative.scd").asSymbol);
	check.("absolute path", compiledPath.(here.asSymbol) == here.asSymbol);
	check.("this file", {}.def.filenameSymbol == here.asSymbol);
	check.("no file", "{}".interpret.def.filenameSymbol.isNil);

	if(failures.isEmpty) { 0.exit } { "FAILED: %".format(failures.join(", ")).postln; 1.exit };
} { |error|
	error.reportError;
	1.exit
};
