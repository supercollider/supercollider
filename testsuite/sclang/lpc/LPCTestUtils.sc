/* LPCTestUtils.sc
*
* Utilities for testing the SuperCollider 3 lexer, parser, compiler
* Original author: Brian Heim 2017-02-19
* This code is licensed under the GPLv3
*
* Note: throughout this file the terms `alphabet` and `string` are used in their
* formal-language senses. An `alphabet` is a set of abstract symbols (in this case,
* SC Strings) and a `string` is a sequence of those symbols.
*
* Note: performance really matters for some of these things. I did a lot of
* benchmarking and came up with low-level functions and loops that are blazingly
* fast for SC. You are welcome to find faster methods.
*/

/****** LAYOUT OF THIS FILE *******

----major methods----

evaluateAllStrings                 // main method: take an alphabet, evaluate all
                                   //   strings in its language, and write to a file.
evaluateString                     // evaluate a string; called by evaluateAllStrings
compareFiles                       // compare two files (typically test/validation)
compareHeaders                     // compare the headers; called by compareFiles
compareData                        // compare the data; called by compareFiles

----diff helper methods----

writeDiffs                         // write an array of diffs to a file
explainDiff                        // convert a diff to easily readable format
makeDiff                           // construct a diff from two dictionaries

----misc helper methods----

makeTestString                     // make a test string given an alphabet and counter
safeOpenFile                       // open a file, throwing errors immediately on fail
safeMkdir                          // mkdir if no dir exists, throw error on fail
parseTestResult                    // turn a line read from a file into structured data
doOutputsMatch                     // return true iff two test results are considered
                                   //   matching. see implementation for special cases.
doFloatOutputsMatch                // return true if floats match for a more accepting
                                   //   cross-platform interpretation
incrementAlphabetCount             // increment an alphabet counter in-place
debug                              // ordinary debugging method

----data conversion methods----

bytecodeToHexString                // Int8Array -> String of hex characters
bytecodeFromHexString              // inverse of `bytecodeToHexString`
stringToHexString                  // String of chars -> String of hex chars
stringFromHexString                // inverse of `stringToHexString`

----header I/O methods----

writeHeader                        // write a header with test parameter info
parseHeader                        // recover test data from a file's header

----header parsing helper methods----

verifyFieldName                    // these methods all perform specialized minor data
parseBlockName                     //   validation. see implementations for details.
parseAlphabetSize
parseAlphabet
parseStringLength
parsePrefix
parseSuffix
parseTechnique

************************************/

LPCTestUtils {
	const <compileErrorString = "!cErr";
	const <runtimeErrorString = "!rErr";

	// Values used in `doOutputsMatch`
	const <floatEqualityPrec = 1e-13;
	const <nanString_win =     "312E23494E44";   // hex string for "1.#IND", NaN on windows
	const <nanString =         "6E616E";         // hex string for "nan"
	const <infString_pos =     "696E66";         // hex string for "inf"
	const <infString_neg =     "2D696E66";       // hex string for "-inf"
	const <infString_pos_win = "312E23494E46";   // hex string for "1.#INF", +inf on windows
	const <infString_neg_win = "2D312E23494E46"; // hex string for "-1.#INF", -inf on windows
	const <lidString =         "LID";            // string for "LID"

	classvar <>doDebug = true;

	// Defaults to false to avoid accidentally overwriting diffs/output
	classvar <>overwriteFiles = false;

	// Set to `true` to turn off the more lenient checking done in `doOutputsMatch`
	classvar <>strictOutputChecking = false;

	// At the time of writing, the current maximum line size is generated by
	// TestCompilerBrutal's \infix alphabet, at 2508 characters.
	const <maxline = 4096;

	// Evaluates all strings in the language L = S^n, where S is the alphabet and
  // n is the given string length `len`. For x in L, the string evaluated is
	// {prefix}{x}{suffix}. Writes the results to a file. Note that the alphabet
	// is sorted before beginning to ensure lexicographical order.
	*evaluateAllStrings {
		// If compress=true, record consecutive identical results with a count, saving space
		arg alphabet, len, prefix, suffix, filename, technique, compress = true;

		var alphabetSize = alphabet.size; // for optimization
		var file, toTest, testResult;
		var prevResult, repeatCount = 0; // For use with compressed output.
		var counter = 0!len; // Used to traverse the language in lexicographical order

		alphabet = alphabet.sort;
		file = this.safeOpenFile(filename, "w");

		protect {
			// postf("%: Writing header\n", thisMethod);
			this.writeHeader(file, alphabet, len, prefix, suffix, technique);

			this.debug("%: Writing data".format(thisMethod.name));

			// Written-out do-while.
			toTest = this.makeTestString(alphabet, counter);
			testResult = this.evaluateString(prefix++toTest++suffix, technique);
			file.write(this.stringToHexString(toTest)++"\t"++testResult);
			prevResult = testResult;

			while { this.incrementAlphabetCount(counter, len, alphabetSize) } {
				// counter.postln;

				toTest = this.makeTestString(alphabet, counter);
				testResult = this.evaluateString(prefix++toTest++suffix, technique);

				if(compress) {
					// If this is a new result, write out the previous repeat count (if
					// there was one). Otherwise, increment the repeat counter.
					if(testResult != prevResult) {
						if(repeatCount > 0) {
							file.write("\t"++repeatCount);
							repeatCount = 0;
						};

						file.write("\n"++this.stringToHexString(toTest)++"\t"++testResult);
						prevResult = testResult;
					} {
						repeatCount = repeatCount+1;
					};
				} {
					file.write("\n"++this.stringToHexString(toTest)++"\t"++testResult);
				}; // if(compress)

			}; // while

			// Flush leftover count.
			if(compress && (repeatCount > 0)) {
				file.write("\t"++repeatCount);
			};

			file.putChar($\n);
		} {
			file.close;
		};
	}

	*evaluateString {
		arg string, technique;
		var r;

		// clear all interpreter variables (a-z)
		thisProcess.interpreter.clearAll;

		^switch(technique,
			\interpret, {
				r = string.compile;
				if(r.isNil) {
					// .compile returns nil if there was a compile error
					compileErrorString;
				} {
					try {
						r = r.value;
						// if successful, record as string + class of produced object. This
						// has drawbacks (see `doOutputsMatch`) but gives us enough resolution.
						this.stringToHexString(r.asString)++":"++r.class;
					} {
						// .value may produce a runtime error
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

	// Compare the contents of two files, and return a list of what was different.
	*compareFiles {
		arg filename1, filename2;

		var file1, file2;
		var diffs;

		file1 = this.safeOpenFile(filename1, "r");
		file2 = this.safeOpenFile(filename2, "r");

		protect {
			var header1, header2;

			header1 = this.parseHeader(file1);
			header2 = this.parseHeader(file2);

			this.compareHeaders(header1, header2);

			// "%: Comparing data\n".postf(thisMethod);
			diffs = this.compareData(file1, file2, header1[\alph], header2[\alph], header1[\strlen]);
		} {
			file1.close;
			file2.close;
		};

		^diffs;
	}

	// Warn if any fields differ; fail if strlens are not equal.
	*compareHeaders {
		arg header1, header2;

		union(header1.keys, header2.keys).do {
			arg key;

			if(header1[key] != header2[key]) {
				"%: headers differ on key %\n"
				"\tfirst header has: %\n"
				"\tsecond header has: %".format(thisMethod, key, header1[key], header2[key]).warn;
			}
		};

		if(header1[\strlen] != header2[\strlen]) {
			Error("%: string lengths are not equal: %, %"
				.format(thisMethod, header1[\strlen], header2[\strlen])).throw;
		};
	}

	*compareData {
		arg file1, file2, alph1, alph2, strlen;

		// Component states
		var files = [file1, file2];
		var alphs = [alph1, alph2];
		var alphSizes = alphs.collect(_.size);
		var ctrs = (0!strlen)!2; // string production counters
		var outputs = nil!2;
		var prevOutputs = nil!2;
		var reps = 0!2; // repetition counters

		// Master state (suffix `M` = `master`)
		// Necessary for the case when alphabets differ in size.
		var alphM = alphs.flatten(1).collect(_.asSymbol).asSet.asArray
		              .collect(_.asString).sort;
		var alphSizeM = alphM.size;
		var ctrM = 0!strlen;

		// Overall control
		var hasNext = true;
		var diffs = [];
		var areAlphabetsEqual = alph1 == alph2; // Can optimize if equal

		// alphM.postln;

		// Stop when the master alphabet has been exhausted.
		while { hasNext } {
			var inputs = alphs.collect({ |alph, i| alph[ctrs[i]] });
			var inputM = alphM[ctrM];

			// Get each file's output for the current master input. If a file's alphabet doesn't
			// include the string, use `nil`. Since alphabets are sorted in lexicographical order,
			// component alphabets can only skip ahead of the master alphabet's order.
			inputs.do({
				arg input, i;

				// postf("here: % %\n", input, inputM);
				if(inputM == input) {
					// "inFirstIf".postln;
					this.incrementAlphabetCount(ctrs[i], strlen, alphSizes[i]);

					// If we're repeating an input, just decrease the repetition counter and return
					// the last result. If not, parse the next line.
					if(reps[i] > 0) {
						reps[i] = reps[i]-1;
					} {
						var line = files[i].getLine(this.maxline);

						if(line.size >= (this.maxline-1)) {
							this.debug("compareData: maxline characters read; increase maxline in LPCTestUtils.\n"
								"File, line: %, %".format(files[i], line));
						};

						prevOutputs[i] = this.parseTestResult(line);
						reps[i] = prevOutputs[i][1];
					};

					outputs[i] = prevOutputs[i][0];
				} {
					outputs[i] = nil;
				}
			});

			// outputs.postln;

			if(this.doOutputsMatch(inputM, *outputs).not) {
				// "no match".postln;
				diffs = diffs.add(this.makeDiff(inputM, *outputs));
			} {
				// "match".postln;
				if(areAlphabetsEqual) {
					// Eat up extra repetitions when possible.
					var min = reps.minItem;
					reps = reps - min;
					min.do {
						this.incrementAlphabetCount(ctrs[0], strlen, alphSizes[0]);
						this.incrementAlphabetCount(ctrs[1], strlen, alphSizes[1]);
						this.incrementAlphabetCount(ctrM, strlen, alphSizeM);
					};
				}
			};

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

		var file = this.safeOpenFile(filename, "w");
		var missingFromFile1 = [];
		var missingFromFile2 = [];
		var realDiffs = [];
		var headers = [
			"% entries were missing from the test file",
			"% entries were missing from the validation file",
			"% entries were different (test vs validation)"
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

			if(entry.isNil, {
				"(no entry)";
			}, {
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
			}).quote;
		};

		^"%: % vs %".format(
			this.stringFromHexString(diff[0]),
			explain.(diff[1]),
			explain.(diff[2]);
		);
	}

	*makeDiff {
		arg input, data1, data2;

		input = this.stringToHexString(input.reduce('++'));
		data1 = data1!?_.split($:);
		data2 = data2!?_.split($:);

		^[input, data1, data2];
	}

	////////////////////////////////////////
	///// MISCELLANEOUS HELPER METHODS /////
	////////////////////////////////////////

	// ex.: ["A", "B", "C"] -> [2,0,1] => "CAB"
	*makeTestString {
		arg alphabet, counter;
		// Since reduce on an empty array returns nil, `?""` is a safeguard.
		^alphabet[counter].reduce('++')?"";
	}

	// Opens a file, throwing errors if there are failures at any point.
	*safeOpenFile {
		arg filename, mode;

		var file;

		this.debug("% file: %".format(
			if(mode == "w", "Writing to", "Reading from"),
			PathName(filename).fileName)
		);

		if((mode == "w") && File.exists(filename) && overwriteFiles.not) {
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
			arg e;
			"ERROR: %: Failed while opening file: %\n".postf(thisMethod, filename.quote);
			e.throw;
		};

		if(file.isOpen.not) {
			Error("%: Failed to open file: %".format(thisMethod, filename.quote)).throw;
		};

		^file;
	}

	*safeMkdir {
		arg dirname;

		if(File.exists(dirname).not) {
			"%: Creating directory %".format(thisMethod, dirname.quote).postln;
			try {
				File.mkdir(dirname);
			} {
				Error("%: Could not create directory %".format(thisMethod, this.outputDir.quote)).throw;
			}
		}
	}

	*parseTestResult {
		arg line;

		line = line.split($\t);

		if(line.size == 3) {
			^[line[1], line[2].asInteger];
		} {
			^[line[1], 0];
		}
	}

	*doOutputsMatch {
		arg input, a, b;

		a = a!?_.split($:);
		b = b!?_.split($:);

		// strict checking: pure equality test
		if(this.strictOutputChecking) {
			^a == b;
		};

		// The class `LID` is defined on Linux only; ignore it.
		if( input.join.contains(lidString) ) {
			this.debug(
				"Ignoring a result because of LID class."
				"\tInput: " ++ input ++
				"\tOutput 1: " ++ a ++
				"\tOutput 2: " ++ b
			);

			^true;
		};

		// Special floating point quirks
		if( a.notNil && b.notNil && { (a[1] == "Float") && (a[1] == b[1]) } ) {
			^this.doFloatOutputsMatch(input, a[0], b[0]);
		};

		^a == b;
	}

	*doFloatOutputsMatch {
		arg input, a, b; // assumed: a and b are hex strings of floats

		// If both nan of some sort, return true. `endsWith` because some systems
		// may print NaN as `-nan`.
		if( (a.endsWith(nanString) || a.endsWith(nanString_win)) &&
			  (b.endsWith(nanString) || b.endsWith(nanString_win)) ) {
			this.debug(
				"Ignoring a result because of nan."
				"\tInput: " ++ input ++
				"\tOutput 1: " ++ a ++
				"\tOutput 2: " ++ b
			);

			^true;
		};

		// If both +inf, return true.
		if( ((a == infString_pos) || (a == infString_pos_win)) &&
			  ((b == infString_pos) || (b == infString_pos_win)) ) {
			this.debug(
				"Strings matched under cross-platform representations of +inf."
				"\tInput: " ++ input ++
				"\tOutput 1: " ++ a ++
				"\tOutput 2: " ++ b
			);

			^true;
		};

		// If both -inf, return true.
		if( ((a == infString_neg) || (a == infString_neg_win)) &&
			  ((b == infString_neg) || (b == infString_neg_win)) ) {
			this.debug(
				"Strings matched under cross-platform representations of -inf."
				"\tInput: " ++ input ++
				"\tOutput 1: " ++ a ++
				"\tOutput 2: " ++ b
			);

			^true;
		} {
			var a_f = this.stringFromHexString(a).asFloat;
			var b_f = this.stringFromHexString(b).asFloat;

			// Note: `equalWithPrecision` returns false when comparing infinities. Use
			// relative precision because we're only trying to catch rounding errors.
			// If this fails, fall through to simple equality test which will succeed
			// on inf==inf or -inf==-inf.
			if ( equalWithPrecision(a_f, b_f, 0, floatEqualityPrec) ) {
				if( a_f != b_f ) {
					this.debug(
						"Ignoring a result because of float precision."
						"\tInput: %\tOutput 1: % (%) \tOutput 2: % (%)".format(
							input, a, a_f, b, b_f // might as well post floats since we have them
						);
					);
				};

				^true;
			}
		};

		^a == b;
	}

	// Returns `false` if there is an overflow. Modifies the array in-place!
	*incrementAlphabetCount {
		arg ctr, len, n;

		// I benchmarked this pretty hard, but there might be a better way! - Brian
		while {len > 0} {
			len = len-1;
			ctr[len] = ctr[len] + 1;
			/*if(len == 0) {
			"incrementAlphabetCount: at %\n".postf(ctr); // progress updates
			};*/
			if(ctr[len] != n) {^true} {ctr[len] = 0};
		};

		^false;
	}

	*debug {
		arg str;

		if(this.doDebug) { ("[debug] LPCTestUtils:" + str).postln };
	}

	///////////////////////////////////
	///// DATA CONVERSION METHODS /////
	///////////////////////////////////

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

	*bytecodeFromHexString {
		arg hexString;

		var bytes = Int8Array.newClear(hexString.size.div(2));

		bytes.size.do {
			arg i;
			bytes[i] = hexString[i*2].digit*16 + hexString[i*2+1].digit;
		};

		^bytes;
	}

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

	*writeHeader {
		arg file, alphabet, stringLength, prefix, suffix, technique, stringCount;

		var alphabetString = "";

		alphabetString = alphabet.collect(this.stringToHexString(_)).join(",");

		// data validation
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

		file.write(
			"HEAD\n"
			"alphabet size:%\n"
			"alphabet:%\n"
			"string length:%\n"
			"prefix:%\n"
			"suffix:%\n"
			"technique:%\n"
			"DATA\n".format(
				alphabet.size,
				alphabetString,
				stringLength,
				this.stringToHexString(prefix),
				this.stringToHexString(suffix),
				technique
			)
		);
	}

	// Returns results in a Dictionary.
	*parseHeader {
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

		^result;
	}

	/////////////////////////////////////
	/// HEADER READING HELPER METHODS ///
	/////////////////////////////////////

	*verifyFieldName {
		arg str, expected;

		var len = expected.size;

		if(str.size >= (this.maxline-1)) {
			this.debug("maxline characters read while parsing header; increase maxline.");
		};

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

}
