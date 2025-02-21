(
var makeRed, makeYellow, makeGreen, makeBold;
var passed = 0, failed = 0, skipped = 0;
var results, path, tmpPath, file;
var useColors = \Document.asClass.isNil; // use colors if we're running sclang directly
var exitWhenDone = \Document.asClass.isNil; // exit if we're running sclang directly
var failedList = List();

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
	var alreadyAddedToFailed, groupMsg;
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
			alreadyAddedToFailed = false;
			if(passedResults == allResults) {
				header = makeGreen.(header);
			} {
				header = makeRed.(header);
			};
			groupMsg = "\t%\t% (%s)".format(header, name, (dict[\duration] ? 0).round(0.01)).postln;
			dict[\results].do({|resDict|
				if(resDict[\pass]) {
					header = makeGreen.("*");
					passed = passed + 1;
				} {
					header = makeRed.("!");
					failed = failed + 1;
					if(alreadyAddedToFailed.not) {failedList.add(groupMsg); alreadyAddedToFailed = true};
					failedList.add("\t\t % %".format(header, resDict[\test].asString));
				};
				"\t\t % %".format(header, resDict[\test].asString).postln;
			})
		};
		if(dict[\completed] != true) {
			"\t%\t% did not complete%".format(makeRed.("[!]"), name, if(dict[\duration].notNil) {" (%s)".format(dict[\duration].round(0.01))} {""}).postln;
			failed = failed + 1;
			failedList.add("\t%\t% did not complete%".format(makeRed.("[!]"), name, if(dict[\duration].notNil) {" (%s)".format(dict[\duration].round(0.01))} {""}));
		} {
			if(allResults == 0) { // marked as completed, but no results were recorded...
				// "\t%\t% (%s)".format(makeRed.("[!]"), name, (dict[\duration] ? 0).round(0.01)).postln;
				// failed = failed + 1;
				// tests that didn't produce any results didn't necessarily fail (e.g. TestSerialPort on macOS)
				"\t%\t% did not produce any results%".format(makeYellow.("[?]"), name, if(dict[\duration].notNil) {" (%s)".format(dict[\duration].round(0.01))} {""}).postln;
			}
		}
	};
});

if(failed > 0) {
	"\nFailed tests:\n".postln;
	failedList.do({ |str| str.postln; });
	"".postln;
	"\t----------------------------------------".postln;
	"\t%, %, out of %".format(makeRed.("% TEST% FAILED".format(failed, (failed > 1).if({"S"},{""}))), makeYellow.("% skipped".format(skipped)), passed + skipped + failed).postln;
	"\t----------------------------------------".postln;
} {
	"".postln;
	"\t------------------------------".postln;
	"\t%, %".format(makeGreen.("% TESTS PASSED".format(passed)), makeYellow.("% skipped".format(skipped))).postln;
	"\t------------------------------".postln;
};

if(exitWhenDone) {(failed > 0).asInteger.exit};

)
