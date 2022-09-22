(
var makeRed, makeYellow, makeGreen, makeBold;
var passed = 0, failed = 0, skipped = 0;
var results, path, tmpPath, file;
var useColors = \Document.asClass.isNil; // use colors if we're running sclang directly
var exitWhenDone = \Document.asClass.isNil; // exit if we're running sclang directly

if(thisProcess.argv.size > 0) {
	path = thisProcess.argv[0];
} {
	path = PathName(thisProcess.nowExecutingPath).pathOnly +/+ "run" +/+ "test_result.scxtar";
};

if(File.exists(path).not) {
	"No file found at %".format(path).error;
	if(exitWhenDone) {2.exit; this.halt};
};

results = Object.readArchive(path);

makeRed = {|str|
	if(useColors) {
		str = "%[31m%%[0m".format(27.asAscii, str, 27.asAscii);
	};
	str;
};
makeYellow = {|str|
	if(useColors) {
		str = "%[33m%%[0m".format(27.asAscii, str, 27.asAscii);
	};
	str;
};
makeGreen = {|str|
	if(useColors) {
		str = "%[32m%%[0m".format(27.asAscii, str, 27.asAscii);
	};
	str;
};
makeBold = {|str|
	if(useColors) {
		str = "%[1m%%[0m".format(27.asAscii, str, 27.asAscii);
	};
	str;
};

results.do({|dict|
	var allResults, passedResults, header, name;
	name = makeBold.("%:%".format(dict[\suite], dict[\test]));
	if(dict[\skip].asBoolean) {
		// "skipped: %".format(dict[\skipReason]).postln;
		header = makeYellow.("[-]");
		"\t%\t% Skipped (reason: %)".format(header, name, dict[\skipReason]).postln;
		skipped = skipped + 1;
	} {
		allResults = dict[\results].size;
		passedResults = dict[\results].select({|resDict| resDict[\pass] == true}).size;
		// [12/12] TestSymbol:test_isIdentifier (0.0s)
		header = "[%/%]".format(passedResults, allResults);
		if(allResults > 0) {
			if(passedResults == allResults) {
				header = makeGreen.(header);
			} {
				header = makeRed.(header);
			};
			"\t%\t% (%s)".format(header, name, (dict[\duration] ? 0).round(0.01)).postln;
			dict[\results].do({|resDict|
				if(resDict[\pass]) {
					header = makeGreen.("*");
					passed = passed + 1;
				} {
					header = makeRed.("!");
					failed = failed + 1;
				};
				"\t\t % %".format(header, resDict[\test].asString).postln;
			})
		} {
			// "\t%\t% (%s)".format(makeRed.("[!]"), name, (dict[\duration] ? 0).round(0.01)).postln;
			// failed = failed + 1;
			// tests that didn't produce any results didn't necessarily fail (e.g. TestSerialPort on macOS)
			"\t%\t% did not produce any results (%s)".format(makeYellow.("[?]"), name, (dict[\duration] ? 0).round(0.01)).postln;
		}
	};
});

if(failed > 0) {
	"\t%, %, out of %".format(makeRed.("% TESTS FAILED".format(failed)), makeYellow.("% skipped".format(skipped)), passed + skipped + failed).postln;
	"Search for ! to find failed tests".postln;
} {
	"\t%, %".format(makeGreen.("% TESTS PASSED".format(passed)), makeYellow.("% skipped".format(skipped))).postln;
};

if(exitWhenDone) {(failed > 0).asInteger.exit};

)
