/* LPCTestUtils.sc
*
* Utilities for testing the SuperCollider 3 lexer, parser, compiler
* Original author: Brian Heim 2017-02-19
* This code is licensed under the GPLv3
*
* Note: performance really matters for some of these things. I
* did a lot of intense benchmarking and came up with low-level
* functions and loops that are blazingly fast for SC. You, the
* reader, are welcome to find faster methods if you desire.
*/

/****** LAYOUT OF THIS FILE ******/
/*
----main methods----

evaluateAllStrings
evaluateString
validate
compareData

----diff helper methods----

writeDiffs
explainDiff
mkDataDiff

----misc helper methods----

mkTestString
safeOpenFile
parseTestResult
doOutputsMatch
mkFilename
incrementAlphabetCount

----data conversion methods----

bytecodeToHexString
bytecodeFromHexString
stringToHexString
stringFromHexString

----header I/O methods----

writeHeader
readHeader

----header parsing helper methods----

verifyFieldName
parseBlockName
parseAlphabetSize
parseAlphabet
parseStringLength
parsePrefix
parseSuffix
parseTechnique
parseStringCount
*/

LPCTestUtils {
	const <compileErrorString = "!cErr";
	const <runtimeErrorString = "!rErr";
	const <validatedOutputFilenameSuffix = "_correct";
	const <diffOutputFilenameSuffix = "_diff";

	// May need to update this, some strings like `0!99` can produce very long output
	const <maxline = 1024;

	// Tests all possible combinations-with-replacement of members of a given alphabet,
	// writes the results to a file, and checks it against validated results.
	// The client may also specify an optional prefix and/or suffix to be added to each
	// string before testing. Only strings of length `len` are tested.
	// Note that the given alphabet will be sorted before beginning.
	*evaluateAllStrings {
		arg alphabet, len, prefix, suffix, testID, technique,

		// If true, don't validate or delete the file, but give it the validation suffix
		doValidate = false,

		// If true, record consecutive identical results with a count, saving space
		compress = true;

		var alphabetSize = alphabet.size;
		var file, filename, toTest, testResult;
		// For use with compressed output.
		var prevResult, repeatCount = 0;
		// An array counter used to generate all possible strings deterministically.
		// Has to be this way because a 32-bit int might overflow.
		var counter = 0!len;

		// Sorting the alphabet beforehand provides uniformity
		alphabet = alphabet.sort;

		filename = this.mkFilename(testID, doValidate);
		file = this.safeOpenFile(filename, "w");

		protect {
			// postf("%: Writing header\n", thisMethod);
			this.writeHeader(
				file, alphabet, len, prefix, suffix, technique,	alphabetSize ** len
			);

			postf("%: Writing data\n", thisMethod);

			// Write the first result. We have to save `\n` because of the possibility
			// for repeats. Also, reduce on an empty array returns nil, thus `?""`.
			toTest = this.mkTestString(alphabet, counter);
			testResult = this.evaluateString(prefix++toTest++suffix, technique);
			file.write(this.stringToHexString(toTest)++"\t"++testResult);

			prevResult = testResult;

			while { this.incrementAlphabetCount(counter, len, alphabetSize) } {
				// counter.postln;

				// no way to get here if we had an empty array, so we can discard `?""`
				toTest = this.mkTestString(alphabet, counter);
				testResult = this.evaluateString(prefix++toTest++suffix, technique);

				// Only do fancy compression techniques if asked. Otherwise just print
				// a normal line.
				if(compress) {
					// If this is a new result, write out the previous repeat count (if
					// there was one). Otherwise, just increment the repeat counter.
					if(testResult != prevResult) {
						if(repeatCount > 0) {
							file.write("\t"++repeatCount);
							repeatCount = 0;
						};

						file.write("\n"++this.stringToHexString(toTest)++"\t"++testResult);
						prevResult = testResult;
					} {
						repeatCount = repeatCount + 1;
					};
				} {
					file.write("\n"++this.stringToHexString(toTest)++"\t"++testResult);
				};
			}; // end while

			// Flush any leftover count.
			if(compress && (repeatCount > 0)) {
				file.write("\t"++repeatCount);
			};

			file.putChar($\n);
		} {
			// If anything bad happened while writing, protect by closing the file
			file.close;
		};

		// If validating, record the diffs and delete the file.
		if(doValidate.not) {
			var diffs;

			protect {
				postf("%: Validating against expected output\n", thisMethod);
				diffs = this.validate(filename);
			} {
				postf("%: Deleting test file\n", thisMethod);
				File.delete(filename);
			};

			^diffs ? []
		}

		^[]
	}

	*evaluateString {
		arg string, technique;
		var r;

		// clear all variables (a-z)
		thisProcess.interpreter.clearAll;

		^switch(technique,
			\interpret, {
				r = string.compile;
				if(r.isNil) {
					// compile error
					compileErrorString;
				} {
					// interpret using .value
					try {
						r = r.value;
						this.stringToHexString(r.asString)++":"++r.class;
					} {
						// catch any runtime error and make note of it
						runtimeErrorString;
					}
				}
			},
			\bytecode, {
				r = string.compile;
				if(r.isNil) {
					compileErrorString;
				} {
					this.bytecodeToHexString(r.def.code);
				}
			}, {
				Error("%: invalid technique option: %".format(thisMethod, technique)).throw;
			}
		);
	}

	*validate {
		arg filename;

		var validated = filename ++ validatedOutputFilenameSuffix;

		var efile, afile; // expected file, actual file
		var diffs; // array of pairs of lines that are not identical

		afile = this.safeOpenFile(filename, "r");
		efile = this.safeOpenFile(filename, "r");

		protect {
			var eheader, aheader;

			eheader = this.readHeader(efile);
			aheader = this.readHeader(afile);

			// compare the headers, warn if there are issues
			union(aheader.keys, eheader.keys).do {
				arg key;
				if(aheader[key] != eheader[key]) {
					"%: headers differ on key %\n"
					"\texpected header has: %\n"
					"\tactual header has: %".format(
						thisMethod, key, aheader[key], eheader[key]
					).warn;
				}
			};

			if(aheader[\strlen] != eheader[\strlen]) {
				Error("%: string lengths are not equal: % != %"
					.format(thisMethod, aheader[\strlen], eheader[\strlen])).throw;
			};

			// "%: comparing data\n".postf(thisMethod);
			diffs = this.compareData(
				afile, efile, aheader[\alph], eheader[\alph], aheader[\strlen]
			);
		} {
			// if anything fails, close the files
			afile.close;
			efile.close;
		};

		^diffs;
	}

	// Finds differences between data files with no knowledge of
	// 'correct' or 'experimental'.
	*compareData {
		arg file1, file2, alph1, alph2, strlen;

		// individual file states
		var files = [file1, file2];
		var alphs = [alph1, alph2];
		var alphSizes = alphs.collect(_.size);
		var ctrs = (0!strlen)!2;
		var prevs = nil!2;
		var reps = 0!2;
		var outputs = nil!2;
		var inputs;

		// master state variables

		// alphM is the "master alphabet" - a sorted array of the unique
		// strings from the alphabet.
		var alphM = alphs.flatten(1).collect(_.asSymbol).asSet.asArray
		              .collect(_.asString).sort;
		var alphSizeM = alphM.size;
		var ctrM = 0!strlen;
		var inputM;

		// overall control
		var hasNext = true;
		var diffs = [];

		// we can make some time-saving assumptions if alphabets are equal
		var areAlphabetsEqual = alph1 == alph2;

		// inputs.postln;
		// alphM.postln;

		while { hasNext } {
			// Load in the input strings for all three alphabets.
			inputs = alphs.collect({ |alph, i| alph[ctrs[i]] });
			inputM = alphM[ctrM];

			// Determine what each file has for its output for the current
			// master alphabet state. The master alphabet will always take
			// at least as many cycles to get to one file's state as that
			// file itself will, so if they don't match right now, we just
			// have to wait for it to catch up. This is guaranteed by
			// sorting the alphabets.
			outputs = inputs.collect({
				arg input, i;

				// postf("here: % %\n", input, inputM);
				// If this is an input for which this file has a recorded,
				// gather it, else return nil.
				if(inputM == input) {
					// "inFirstIf".postln;
					this.incrementAlphabetCount(ctrs[i], strlen, alphSizes[i]);

					// If we're repeating an input, just decrease the counter
					// and return the last result. If not, get the next line
					// and parse it.
					if(reps[i] > 0) {
						reps[i] = reps[i]-1;
					} {
						var line, data;

						line = files[i].getLine(this.maxline);
						if(line.size >= (this.maxline-1)) {
							"compareData: maxline characters read from file %: %"
							  .format(files[i], line).warn;
						};

						data = this.parseTestResult(line);
						reps[i] = data[\reps];
						prevs[i] = data;
					};

					prevs[i];
				} {
					nil;
				}
			});

			// outputs.postln;

			// If the outputs don't match (either the values are wrong or
			// one input is missing from the other file) then record it.
			if(this.doOutputsMatch(*outputs).not) {
				// "no match".postln;
				diffs = diffs.add(
					this.mkDataDiff(
						this.stringToHexString(inputM.reduce('++')),
						*outputs
					)
				);
			} {
				// "match".postln;
				// if the outputs matched and alphabets are equal
				if(areAlphabetsEqual) {
					// "inAlphabetsAreEqual".postln;
					while { (reps[0] > 0) && (reps[1] > 0) } {
						// While both reps are greater than 0, we know that the
						// next outputs are going to be equal. Decrease the reps
						// and increment the counters.
						reps = reps - 1;
						this.incrementAlphabetCount(ctrs[0], strlen, alphSizes[0]);
						this.incrementAlphabetCount(ctrs[1], strlen, alphSizes[1]);
						this.incrementAlphabetCount(ctrM, strlen, alphSizeM);
					};
					// "endOfIf".postln;
				}
			};

			// Stop when the master alphabet has been exhausted.
			hasNext = this.incrementAlphabetCount(ctrM, strlen, alphSizeM);
		};

		^diffs;
	}

	///////////////////////////////
	///// DIFF HELPER METHODS /////
	///////////////////////////////

	// Given a full list of diffs, arranges and writes them by what is missing
	// from the first and/or second files, and then writes what was different
	// in existing entries.
	*writeDiffs {
		arg diffs, filename;

		var file = this.safeOpenFile(this.mkFilename(filename, false), "w");
		var missingFromFile1 = [];
		var missingFromFile2 = [];
		var realDiffs = [];
		var headers = [
			"% entries were missing from file 1",
			"% entries were missing from file 2",
			"% entries were different between files"
		];

		diffs.do {
			arg diff;
			case(
				{ diff[1] == nil }, {
					missingFromFile1 = missingFromFile1.add(diff);
				},
				{ diff[2] == nil }, {
					missingFromFile2 = missingFromFile2.add(diff);
				}, {
					realDiffs = realDiffs.add(diff);
				}
			)
		};

		[missingFromFile1, missingFromFile2, realDiffs].do {
			arg diffType, i;

			file.write("\n"++headers[i].format(diffType.size).underlined++"\n");

			diffType.do {
				arg diff;
				file.write(this.explainDiff(diff)++"\n");
			}
		};

		file.close;
	}

	// For a given diff, returns a short human-readable description
	// of how the two sides compare.
	*explainDiff {
		arg diff;

		// The `entry` is one of:
		// - compile error string
		// - runtime error string
		// - bytecode string
		// - string followed by class name
		var explain = {
			arg entry;

			if(entry.isNil) {
				"(no entry)";
			} {
				switch(entry[0],
					this.compileErrorString, "compile-error",
					this.runtimeErrorString, "runtime-error",
					{
						// If there are two elements, reconstruct the first element to
						// get the output as a string. `entry[1]` is the output class.
						if(entry.size == 2) {
							this.stringFromHexString(entry[0])++":"++entry[1]
						} {
							entry[0];
						}
					}
				);
			};
		};

		^"%: % vs %".format(
			this.stringFromHexString(diff[0]),
			explain.(diff[1]),
			explain.(diff[2]);
		);
	}

	// Formats input and results data into a 3-tuple of input and outputs
	*mkDataDiff {
		arg input, data1, data2;

		data1 = data1!?_[\out];
		data2 = data2!?_[\out];

		^[input, data1, data2];
	}

	////////////////////////////////////////
	///// MISCELLANEOUS HELPER METHODS /////
	////////////////////////////////////////

	// Given an alphabet and indexing counter, return a string to test.
	// ex.: ["A", "B", "C"] -> [2,0,1] => "CAB"
	*mkTestString {
		arg alphabet, counter;
		// Since reduce on an empty array returns nil, `?""` is a safeguard.
		^alphabet[counter].reduce('++')?"";
	}

	// Opens a file, throwing errors if there are failures at any point.
	*safeOpenFile {
		arg filename, mode;

		var file;

		postln("%: Creating file: %".format(thisMethod, PathName(filename).fileName));

		if((mode == "w") && File.exists(filename)) {
			Error("%: File % already exists\n"
				"\tPlease delete before continuing".format(thisMethod, filename.quote)).throw;
		} {
			if(mode.isNil) {
				Error("%: No mode given".format(thisMethod)).throw;
			}
		};

		try {
			file = File.new(filename, mode);
		} {
			Error("%: Failed while opening file: %.".format(thisMethod, filename.quote)).throw;
		};

		if(file.isOpen.not) {
			Error("%: Failed to open file: %".format(thisMethod, filename.quote)).throw;
		};

		^file;
	}

	// Breaks a line into input, output, and (optional) number of repetitions
	*parseTestResult {
		arg line;

		line = line.split($\t);

		^Dictionary[
			\in -> line[0],
			\out -> line[1].split($:),
			\reps -> (line[2]!?_.asInteger?0)
		];
	}

	// Returns true IFF the two results are equal in their result outputs.
	*doOutputsMatch {
		arg a, b;

		^((a!?_[\out]) == (b!?_[\out]));
	}

	// Adds the `_correct` suffix IFF generating validation files.
	*mkFilename {
		arg testID, doValidate;

		^if(doValidate, {
			testID ++ validatedOutputFilenameSuffix
		}, {
			testID;
		}).resolveRelative;
	}

	// Given an alphabet counter (array of indexing integers), increment by 1.
	// Returns `false` if there is an overflow. Modifies the array in-place!
	*incrementAlphabetCount {
		arg ctr, len, n; // the counter array, string length, and alphabet size (n)

		// I benchmarked this pretty hard, but there might be a better way! - Brian
		while {len > 0} {
			len = len-1;
			ctr[len] = ctr[len] + 1;
			/*if(len == 0) {
			"incrementAlphabetCount: at %\n".postf(ctr);
			};*/
			if(ctr[len] != n) {^true} {ctr[len] = 0};
		};

		^false;
	}

	///////////////////////////////////
	///// DATA CONVERSION METHODS /////
	///////////////////////////////////

	// Converts an input 8-bit value to an ASCII string representing its hex value.
	*bytecodeToHexString {
		arg bytes;

		var hexString = String.newClear(bytes.size*2);

		bytes.do {
			arg byte, i;
			hexString[i*2] = byte.mod(256).div(16).asDigit;
			hexString[i*2+1] = byte.mod(16).asDigit;
		};

		^hexString;
	}

	// Inverse of bytecodeToHexString
	*bytecodeFromHexString {
		arg hexString;

		var bytes = Int8Array.newClear(hexString.size.div(2));

		bytes.size.do {
			arg i;
			bytes[i] = hexString[i*2].digit*16 + hexString[i*2+1].digit;
		};

		^bytes;
	}

	// Converts an input String to a String representing its hex ASCII values.
	// The output string is twice the length of the input.
	*stringToHexString {
		arg string;

		var hexString = String.newClear(string.size*2);

		// I was originally using .asHexString; this is much faster (250+%) - Brian
		string.do {
			arg char, i;
			var hx = char.ascii.mod(256);
			hexString[i*2] = hx.div(16).asDigit;
			hexString[i*2+1] = hx.mod(16).asDigit;
		};

		^hexString;
	}

	// Inverse of stringToHexString
	*stringFromHexString {
		arg hexString;

		var string = String.newClear(hexString.size.div(2));

		string.size.do {
			arg i;
			string[i] = (hexString[i*2].digit*16 + hexString[i*2+1].digit).asAscii;
		}

		^string;
	}

	///////////////////////////////////////
	///// HEADER INPUT/OUTPUT METHODS /////
	///////////////////////////////////////

	// given proper input, creates a header for the test file format used in these tests
	*writeHeader {
		arg file, alphabet, stringLength, prefix, suffix, technique, stringCount;

		var alphabetString = "";

		alphabetString = alphabet.collect(this.stringToHexString(_)).join(",");

		// data validation: BEGIN
		if(alphabet.isKindOf(Array).not) {
			Error("%: alphabet should be an array".format(thisMethod)).throw;
		};
		if(alphabet.isEmpty) {
			Error("%: alphabet should be non-empty".format(thisMethod)).throw;
		};
		if(alphabet.every(_.isKindOf(String)).not) {
			Error("%: alphabet should be composed of strings".format(thisMethod)).throw;
		};
		if(stringLength < 0) {
			Error("%: string length must be at least 0".format(thisMethod)).throw;
		};
		if(prefix.isKindOf(String).not) {
			Error("%: prefix should be a string".format(thisMethod)).throw;
		};
		if(suffix.isKindOf(String).not) {
			Error("%: suffix should be a string".format(thisMethod)).throw;
		};
		if(technique.isKindOf(Symbol).not) {
			Error("%: technique should be a symbol".format(thisMethod)).throw;
		};
		if(stringCount <= 0) {
			Error("%: stringCount must be a positive number".format(thisMethod)).throw;
		};
		// data validation: END

		file.write(
			"HEAD\n"
			"alphabet size:%\n"
			"alphabet:%\n"
			"string length:%\n"
			"prefix:%\n"
			"suffix:%\n"
			"technique:%\n"
			"DATA\n"
			"%\n".format(
				alphabet.size,
				alphabetString,
				stringLength,
				this.stringToHexString(prefix),
				this.stringToHexString(suffix),
				technique,
				stringCount
			)
		);
	}

	// Does pretty hefty validation on the header format. Validation is performed
	// in the parsing subroutines. Returns results in a Dictionary.
	*readHeader {
		arg file;

		var result = Dictionary[];

		if(file.isOpen.not) {
			Error("%: given file is not open: %".format(thisMethod, file.path)).throw;
		};

		// parse blocks and fields
		this.parseBlockName(file.getLine(this.maxline), "HEAD");
		result[\alphSize] = this.parseAlphabetSize(file.getLine(this.maxline));
		result[\alph] = this.parseAlphabet(file.getLine(this.maxline));
		result[\strlen] = this.parseStringLength(file.getLine(this.maxline));
		result[\prefix] = this.parsePrefix(file.getLine(this.maxline));
		result[\suffix] = this.parseSuffix(file.getLine(this.maxline));
		result[\technique] = this.parseTechnique(file.getLine(this.maxline));

		this.parseBlockName(file.getLine(this.maxline), "DATA");
		result[\strcnt] = this.parseStringCount(file.getLine(this.maxline));

		^result;
	}

	/////////////////////////////////////
	/// HEADER READING HELPER METHODS ///
	/////////////////////////////////////

	*verifyFieldName {
		arg str, expected;

		var len = expected.size;

		if(str.isNil) {
			Error("%: unexpectedly reached end of document while"
				"parsing %".format(thisMethod, expected.quote)).throw;
		};

		if(str[..len-1] != expected) {
			Error("%: expected %: got %"
				.format(thisMethod, expected.quote, str[..len-1].quote)).throw;
		};

		^str[len..];
	}

	*parseBlockName {
		arg line, blockName;

		if(line != blockName) {
			Error("%: expected % block, got %"
				.format(thisMethod, blockName.quote, line.quote)).throw;
		};
	}

	*parseAlphabetSize {
		arg str;
		var size;

		str = this.verifyFieldName(str, "alphabet size:");
		size = str.asInteger;

		if(size <= 0) {
			Error("%: alphabet size must be > 0: got %"
				.format(thisMethod, size.quote)).throw;
		};

		^size;
	}

	*parseAlphabet {
		arg str;
		var alphabet;

		str = this.verifyFieldName(str, "alphabet:");
		alphabet = str.split($,);

		if(alphabet.isEmpty) {
			Error("%: alphabet is empty: got %"
				.format(thisMethod, str.quote)).throw;
		};

		try {
			^alphabet.collect(this.stringFromHexString(_));
		} {
			arg e;
			Error("%: error while decoding alphabet %: %"
				.format(thisMethod, alphabet, e.errorString)).throw;
		};
	}

	*parseStringLength {
		arg str;
		var len;

		str = this.verifyFieldName(str, "string length:");
		len = str.asInteger;

		if(len < 0) {
			Error("%: string length must be nonnegative: got %"
				.format(thisMethod, len)).throw;
		};

		^len;
	}

	*parsePrefix {
		arg str;

		str = this.verifyFieldName(str, "prefix:");

		try {
			^this.stringFromHexString(str);
		} {
			arg e;
			Error("%: error while decoding prefix: %"
				.format(thisMethod, e.errorString)).throw;
		};
	}

	*parseSuffix {
		arg str;

		str = this.verifyFieldName(str, "suffix:");

		try {
			^this.stringFromHexString(str);
		} {
			arg e;
			Error("%: error while decoding suffix: %"
				.format(thisMethod, e.errorString)).throw;
		};
	}

	*parseTechnique {
		arg str;

		str = this.verifyFieldName(str, "technique:");

		if(str.isEmpty) {
			Error("%: must provide a technique name".format(thisMethod)).throw;
		}

		^str.asSymbol;
	}

	*parseStringCount {
		arg str;
		var cnt = str.asInteger;

		if(cnt <= 0) {
			Error("%: string count must be > 0: got %".format(thisMethod, cnt)).throw;
		};

		^cnt;
	}
}
