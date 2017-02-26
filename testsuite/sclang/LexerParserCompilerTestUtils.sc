/* LexerParserCompilerTestUtils.sc
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

LexerParserCompilerTestUtils {
	const <compileErrorString = "!cErr";
	const <runtimeErrorString = "!rErr";
	const <validatedOutputFilenameSuffix = "_expected";

	const <maxline = 1024;
	// may need to update this, some strings like `0!99` can produce very long output

	// tests all possible combinations-with-replacement of members of a given alphabet,
	// writes the results to a file, and checks it against validated results.
	// The client may also specify an optional prefix and/or suffix to be added to each
	// string before testing. Only strings of length `len` are tested.
	*testAllPossibleStrings {
		arg alphabet, // Array of strings to test
		len, // String length
		prefix = "", suffix = "",
		testID = "", // unique filename ID
		technique = \compile, // best
		createValidationFile = false, // change to true to generate a validation file
		                              // (doesn't validate or delete)
		compressRepeatResults = true;

		var alphabetSize = alphabet.size;
		var filename, file;
		var header;
		// intro content to be written to the file
		var diffs;
		// holds a list (empty under sucessful execution) of
		// differences between expected and actual output
		var testString;
		// holds the string that will be tested with testOneString
		var prevResult, repeatedResultCount = 0;
		// hold the previous result for compressing repeat results
		var counter = 0!len;
		// [0, 0, ..., 0] which controls indexing into the alphabet to generate a string
		var dataLine;
		// will hold the result stored to the file
		var isPrefixEmpty = prefix.isEmpty;
		var isSuffixEmpty = suffix.isEmpty;

		// sort the alphabet to start
		alphabet = alphabet.sort;

		filename = this.mkFilename(testID);
		if(createValidationFile) {
			filename = filename ++ validatedOutputFilenameSuffix;
		};

		postln("testAllPossibleStrings: Creating file: %".format(filename));
		if(File.exists(filename)) {
			Error("testAllPossibleStrings: File % already exists: "
				"please delete before continuing".format(filename.quote)).throw;
		};

		file = File.new(filename, "w");
		if(file.isOpen.not) {
			Error("testAllPossibleStrings: Failed to open file").throw;
		};

		protect {
			postln("testAllPossibleStrings: Writing header");
			this.writeHeader(
				file, alphabet, len, prefix, suffix, technique,
				alphabetSize ** len /* string count */
			);

			postln("testAllPossibleStrings: Writing data");

			// write the first result
			testString = alphabet[counter].reduce('++')?""; // reduce on an empty array returns nil
			dataLine = this.testOneString(this.mkTestString(prefix,testString,suffix,isPrefixEmpty,isSuffixEmpty), technique);
			file.write(this.stringToHexString(testString) ++ "\t" ++ dataLine);

			prevResult = dataLine;

			while {this.incrementAlphabetCount(counter, len, alphabetSize)} {

				testString = alphabet[counter].reduce('++'); // we couldn't get here if we had an empty array
				dataLine = this.testOneString(this.mkTestString(prefix,testString,suffix,isPrefixEmpty,isSuffixEmpty), technique);

				if(compressRepeatResults) {
					if(dataLine == prevResult) {
						// if we just saw this result, don't print it, but keep track of it
						repeatedResultCount = repeatedResultCount + 1;
					} {
						if(repeatedResultCount > 0) {
							// give a count if it's more than 0. 0 is the assumed default
							file.write("\t" ++ repeatedResultCount.asString);
							repeatedResultCount = 0;
						};

						file.write("\n" ++ this.stringToHexString(testString) ++ "\t" ++ dataLine);
						prevResult = dataLine; // only update if it changed
					}
				} {
					// just write the line normally
					file.write("\n" ++ this.stringToHexString(testString) ++ "\t" ++ dataLine);
				}

			};

			if(compressRepeatResults && (repeatedResultCount > 0)) {
				// "flush the buffer"
				file.write("\t" ++ repeatedResultCount.asString ++ "\n");
			};
		} {
			file.close;
		};

		if(createValidationFile.not) {
			protect {
				postln("testAllPossibleStrings: Validating against standard output");
				diffs = this.validate(filename);
			} {
				postln("testAllPossibleStrings: Deleting test file");
				File.delete(filename);
			}
		};

		postln("testAllPossibleStrings: Success!");

		^diffs;
	}

	*mkTestString {
		arg prefix, string, suffix, isPrefixEmpty, isSuffixEmpty;

		if(isPrefixEmpty) {
			if(isSuffixEmpty) {
				^string
			} {
				^string++suffix
			}
		} {
			if(isSuffixEmpty) {
				^prefix++string
			} {
				^prefix++string++suffix
			}
		}
	}

	*getEncodingString {
		arg technique;

		^switch(technique,
			\compile,
			"<hex-format input string><tab>"
			"<\"!cErr\" (compile error) | \"!rErr\" (runtime error) | hex-format output string>"
			"<opt `:` & class name of output>"
			"<opt tab & count of consecutive repeats of this result (no number means no repetitions)>",
			\bytecode,
			"<hex-format input string><tab>"
			"<\"!cErr\" (compile error) | \"!rErr\" (runtime error) | hex-format output string>"
			"<opt tab & count of consecutive repeats of this result (no number means no repetitions)>",
			{Error("getEncodingString: unrecognized technique: %".format(technique.quote))}
		);
	}

	// returns `false` if there is an overflow
	// modifies the array in-place!
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

	*testOneString {
		arg string, technique;
		var r, rclass;

		// clear all variables (a-z)
		thisProcess.interpreter.clearAll;

		switch(technique,
			\compile, {
				r = string.compile;
				if(r.isNil) {
					// compile error
					r = compileErrorString;
				} {
					// interpret using .value
					try {
						r = r.value;
						rclass = r.class;
						r = this.stringToHexString(r.asString)++":"++rclass;
					} {
						// catch any runtime error and make note of it
						r = runtimeErrorString;
					}
				}
			},
			\bytecode, {
				r = string.compile;
				if(r.isNil) {
					// compile error
					r = compileErrorString;
				} {
					r = this.bytecodeToHexString(r.def.code);
				}
			}, {
				Error("testOneString: invalid technique option: %".format(technique)).throw;
			}
		);

		^r;
	}

	*validate {
		arg filename;

		var validated = filename ++ validatedOutputFilenameSuffix;

		var efile, afile; // expected file, actual file
		var diffs; // array of pairs of lines that are not identical

		File.exists(validated).not.if {
			Error("validate: couldn't find file for validation: %".format(validated.quote)).throw;
		};
		File.exists(filename).not.if {
			Error("validate: couldn't find file for validation: %".format(filename.quote)).throw;
		};

		efile = File(validated, "r");
		afile = File(filename, "r");

		if(efile.isOpen.not) {
			Error("validate: failed to open validation file: %".format(validated.quote)).throw;
		};
		if(afile.isOpen.not) {
			Error("validate: failed to open validation file: %".format(filename.quote)).throw;
		};

		protect {
			var eheader, aheader, keySet, areAlphabetsEqual;

			eheader = this.readHeader(efile);
			aheader = this.readHeader(afile);

			// compare the headers, warn if there are issues
			keySet = union(aheader.keys, eheader.keys);
			keySet.do {
				arg key;
				if(aheader[key] != eheader[key]) {
					"validate: headers differ on key %\n"
					"\texpected header has: %\n"
					"\tactual header has: %".format(
						key, aheader[key], eheader[key]
					).warn;
				}
			};

			if(aheader[\stringLength] != eheader[\stringLength]) {
				Error("validate: string lengths are not equal: % != %".format(aheader[\stringLength], eheader[\stringLength]));
			};

			areAlphabetsEqual = aheader[\alphabet] == eheader[\alphabet];

			"validate: comparing data".postln;
			diffs = this.compareData(afile, efile, aheader[\alphabet], eheader[\alphabet], aheader[\stringLength]);
		} {
			afile.close;
			efile.close;
		};

		^diffs;
	}

	// finds differences between data files with no knowledge of
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
		var inputs = alphs.collect({ |alph, i| alph[ctrs[i]] });

		// master state variables
		var alphM = alphs.flatten(1).collect(_.asSymbol).asSet.asArray.collect(_.asString).sort;
		var alphSizeM = alphM.size;
		var ctrM = 0!strlen;
		var inputM;

		// overall control
		var hasNext = true;
		var diffs = [];

		// inputs.postln;
		// alphM.postln;

		while { hasNext } {
			inputM = alphM[ctrM];

			outputs = inputs.collect({
				arg input, i;
				var result = nil;

				// postf("here: % %\n", input, inputM);
				if(inputM == input) {
					// postf("in inner if: %\n", i);
					this.incrementAlphabetCount(ctrs[i], strlen, alphSizes[i]);

					if(reps[i] > 0) {
						reps[i] = reps[i]-1;
					} {
						var line, data;

						line = files[i].getLine(this.maxline);
						if(line.size >= (this.maxline-1)) {
							"compareData: maxline characters read from file %: %".format(files[i], line).warn;
						};

						data = this.parseDataLine(line);
						reps[i] = data[\reps];
						prevs[i] = data;
					};

					inputs[i] = alphs[i][ctrs[i]];

					result = prevs[i];
				};

				result;
			});

			// outputs.postln;

			if(this.doOutputsMatch(*outputs).not) {
				// "got here 1".postln;
				diffs = diffs.add(
					this.mkDataDiff(
						this.stringToHexString(inputM.reduce('++')),
						*outputs
					)
				);
			};

			hasNext = this.incrementAlphabetCount(ctrM, strlen, alphSizeM);
		};

		^diffs;
	}

	*printDiffs {
		arg diffs;

		var missingFromFile1 = [];
		var missingFromFile2 = [];
		var realDiffs = [];

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

		"% entries were missing from file 1".format(missingFromFile1.size).underlined.postln;

		missingFromFile1.do {
			arg diff;
			this.explainDiff(diff).postln;
		};

		"% entries were missing from file 2".format(missingFromFile2.size).underlined.postln;

		missingFromFile2.do {
			arg diff;
			this.explainDiff(diff).postln;
		};

		"% entries were different between files".format(realDiffs.size).underlined.postln;

		realDiffs.do {
			arg diff;
			this.explainDiff(diff).postln;
		};
	}

	/////////////////////////////////////
	///// SMALL CONVENIENCE METHODS /////
	/////////////////////////////////////

	*explainDiff {
		arg diff;

		var explain = {
			arg entry;
			if(entry.isNil) {
				"(no entry)";
			} {
				switch(entry[0],
					this.compileErrorString, "compile-error",
					this.runtimeErrorString, "runtime-error",
					{
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

	*parseDataLine {
		arg line;

		var result = Dictionary[];

		line = line.split($\t);

		result[\in] = line[0];
		result[\out] = line[1].split($:);
		result[\hadError] = this.isErrorString(result[\out][0]);
		result[\reps] = line[2]!?_.asInteger?0;

		^result;
	}

	*mkDataDiff {
		arg input, data1, data2;

		data1 = data1!?_[\out];
		data2 = data2!?_[\out];

		^[input, data1, data2];
	}

	*doOutputsMatch {
		arg a, b;

		^((a!?_[\out]) == (b!?_[\out]));
	}

	*isErrorString {
		arg str;

		^(str == compileErrorString) || (str == runtimeErrorString);
	}

	*mkFilename {
		arg testID;
		^testID.resolveRelative;
	}

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

	// converts an input String to a String representing its hex ASCII values.
	// The output string is twice the length of the input.
	*stringToHexString {
		arg string;

		// var hexString = "";
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

	// inverse of stringToHexString
	*stringFromHexString {
		arg hexString;

		var string = String.newClear(hexString.size.div(2));

		string.size.do {
			arg i;
			string[i] = (hexString[i*2].digit*16 + hexString[i*2+1].digit).asAscii;
		}

		^string;
	}

	///////////////////////////////////
	///// FILE FORMATTING METHODS /////
	///////////////////////////////////

	// given proper input, creates a header for the test file format used in these tests
	*writeHeader {
		arg file, alphabet, stringLength, prefix, suffix, technique, stringCount;

		var alphabetString = "", encoding;

		alphabetString = alphabet.collect(this.stringToHexString(_)).join(",");
		encoding = this.getEncodingString(technique);

		// data validation: BEGIN
		if(alphabet.isKindOf(Array).not) {
			Error("formatHeader: alphabet should be an array").throw;
		};
		if(alphabet.isEmpty) {
			Error("formatHeader: alphabet should be non-empty").throw;
		};
		if(alphabet.every(_.isKindOf(String)).not) {
			Error("formatHeader: alphabet should be composed of strings").throw;
		};
		if(stringLength < 0) {
			Error("formatHeader: string length must be at least 0").throw;
		};
		if(prefix.isKindOf(String).not) {
			Error("formatHeader: prefix should be a string").throw;
		};
		if(suffix.isKindOf(String).not) {
			Error("formatHeader: suffix should be a string").throw;
		};
		if(technique.isKindOf(Symbol).not) {
			Error("formatHeader: technique should be a symbol").throw;
		};
		if(stringCount <= 0) {
			Error("formatHeader: stringCount must be a positive number").throw;
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
			"encoding:%\n"
			"DATA\n"
			"%\n".format(
				alphabet.size,
				alphabetString,
				stringLength,
				this.stringToHexString(prefix),
				this.stringToHexString(suffix),
				technique,
				encoding,
				stringCount
			)
		);
	}

	// does pretty hefty validation on the header format
	// validation is performed in the parsing subroutines
	// returns results in a Dictionary
	*readHeader {
		arg file;

		var result = Dictionary[];

		if(file.isOpen.not) {
			Error("readHeader: given file is not open: %".format(file.path)).throw;
		};

		// parse blocks and fields
		this.parseBlockName(file.getLine(this.maxline), "HEAD");
		result[\alphabetSize] = this.parseAlphabetSize(file.getLine(this.maxline));
		result[\alphabet] = this.parseAlphabet(file.getLine(this.maxline));
		result[\stringLength] = this.parseStringLength(file.getLine(this.maxline));
		result[\prefix] = this.parsePrefix(file.getLine(this.maxline));
		result[\suffix] = this.parseSuffix(file.getLine(this.maxline));
		result[\technique] = this.parseTechnique(file.getLine(this.maxline));
		result[\encoding] = this.parseEncoding(file.getLine(this.maxline));

		this.parseBlockName(file.getLine(this.maxline), "DATA");
		result[\stringCount] = this.parseStringCount(file.getLine(this.maxline));

		^result;
	}

	////////////////////////////////////////////
	/// HEADER READING HELPER METHODS: BEGIN ///
	////////////////////////////////////////////

	*verifyFieldName {
		arg str, expected;

		var len = expected.size;

		if(str.isNil) {
			Error("parseHeader: unexpectedly reached end of document while parsing %".format(expected.quote)).throw;
		};

		if(str[..len-1] != expected) {
			Error("parseHeader: expected %: got %".format(expected.quote, str[..len-1].quote)).throw;
		};

		^str[len..];
	}

	*parseBlockName {
		arg line, blockName;

		if(line != blockName) {
			Error("readHeader: expected % block, got %".format(blockName.quote, line.quote)).throw;
		};
	}

	*parseAlphabetSize {
		arg str;
		var size;

		str = this.verifyFieldName(str, "alphabet size:");
		size = str.asInteger;

		if(size <= 0) {
			Error("parseHeader: alphabet size must be > 0: got %".format(size.quote)).throw;
		};

		^size;
	}

	*parseAlphabet {
		arg str;
		var alphabet;

		str = this.verifyFieldName(str, "alphabet:");
		alphabet = str.split($,);

		if(alphabet.isEmpty) {
			Error("parseHeader: alphabet is empty: got %".format(str.quote)).throw;
		};

		try {
			^alphabet.collect(this.stringFromHexString(_));
		} {
			arg e;
			Error("parseHeader: error while decoding alphabet %: %".format(alphabet, e.errorString)).throw;
		};
	}

	*parseStringLength {
		arg str;
		var len;

		str = this.verifyFieldName(str, "string length:");
		len = str.asInteger;

		if(len < 0) {
			Error("parseHeader: string length must be nonnegative: got %".format(len)).throw;
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
			Error("parseHeader: error while decoding prefix: %".format(e.errorString)).throw;
		};
	}

	*parseSuffix {
		arg str;

		str = this.verifyFieldName(str, "suffix:");

		try {
			^this.stringFromHexString(str);
		} {
			arg e;
			Error("parseHeader: error while decoding suffix: %".format(e.errorString)).throw;
		};
	}

	*parseTechnique {
		arg str;

		str = this.verifyFieldName(str, "technique:");

		if(str.isEmpty) {
			Error("parseHeader: must provide a technique name").throw;
		}

		^str.asSymbol;
	}

	*parseEncoding {
		arg str;

		str = this.verifyFieldName(str, "encoding:");

		^str;
	}

	*parseStringCount {
		arg str;
		var cnt = str.asInteger;
		if(cnt <= 0) {
			Error("parseHeader: string count must be > 0: got %".format(cnt)).throw;
		};
		^cnt;
	}

	//////////////////////////////////////////
	/// HEADER READING HELPER METHODS: END ///
	//////////////////////////////////////////

}