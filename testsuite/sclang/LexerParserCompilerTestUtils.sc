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
	const <validatedOutputFilenameSuffix = "_correct";

	// May need to update this, some strings like `0!99` can produce very long output
	const <maxline = 1024;

	// Tests all possible combinations-with-replacement of members of a given alphabet,
	// writes the results to a file, and checks it against validated results.
	// The client may also specify an optional prefix and/or suffix to be added to each
	// string before testing. Only strings of length `len` are tested.
	*testAllPossibleStrings {

		// Basic arguments
		arg alphabet, len, prefix, suffix, testID, technique,

		// If true, don't validate or delete the file, and
		// give it a validation filename suffix.
		doValidate = false,

		// If true, note repeated results with a number instead of actually printing out
		// consecutive lines with the same output.
		compress = true;

		var alphabetSize, filename, file;
		// holds the string that will be tested with testOneString
		var toTest;
		// hold the previous result for compressing repeat results
		var prevResult;
		var repeatCount = 0;
		// a counter [0, 0, ..., 0] which is used to index into the alphabet for string
		// generation
		var counter = 0!len;
		// will hold the line to be stored to the file
		var dataLine;
		var isPrefixEmpty = prefix.isEmpty;
		var isSuffixEmpty = suffix.isEmpty;

		alphabet = alphabet.sort;
		alphabetSize = alphabet.size;

		file = this.mkOutputFileSafe(testID, doValidate);

		protect {
			// postln("testAllPossibleStrings: Writing header");
			this.writeHeader(
				file, alphabet, len, prefix, suffix, technique,
				alphabetSize ** len /* string count */
			);

			postln("testAllPossibleStrings: Writing data");

			// Write the first result.
			// Reduce on an empty array returns nil, thus `?""`.
			toTest = alphabet[counter].reduce('++')?"";
			dataLine = this.testOneString(prefix++toTest++suffix, technique);
			file.write(this.stringToHexString(toTest)++"\t"++dataLine);

			prevResult = dataLine;

			while { this.incrementAlphabetCount(counter, len, alphabetSize) } {

				// we couldn't get here if we had an empty array, so no need to use `?""`
				toTest = alphabet[counter].reduce('++');
				dataLine = this.testOneString(prefix++toTest++suffix, technique);

				if(compress) {
					// if we just saw this result, don't print it, but keep track of it
					if(dataLine == prevResult) {
						repeatCount = repeatCount + 1;
					} {

						if(repeatCount > 0) {
							// give a count if it's more than 0. 0 is the assumed default
							file.write("\t"++repeatCount.asString);
							repeatCount = 0;
						};

						file.write("\n"++this.stringToHexString(toTest)++"\t"++dataLine);
						// only update if it changed
						prevResult = dataLine;
					}
				} {
					// just write the line normally
					file.write("\n"++this.stringToHexString(toTest)++"\t"++dataLine);
				}

			}; // end while

			if(compress && (repeatCount > 0)) {
				// flush the buffer
				file.write("\t" ++ repeatCount);
			};

			file.putChar($\n);
		} {
			file.close;
		};

		if(doValidate.not) {
			// if validating, record the diffs and delete the file
			var diffs;

			protect {
				postln("testAllPossibleStrings: Validating against expected output");
				diffs = this.validate(filename);
			} {
				postln("testAllPossibleStrings: Deleting test file");
				File.delete(filename);
			}
		} {
			// if making validation file, return empty diffs
			^[];
		}
	}

	*mkOutputFileSafe {
		arg testID, doValidate;

		var filename, file;

		filename = this.mkFilename(testID);

		if(doValidate) {
			filename = filename ++ validatedOutputFilenameSuffix;
		};

		postln("%: Creating file: %".format(this.class, PathName(filename).fileName));
		if(File.exists(filename)) {
			Error("%: File % already exists\n"
				"\tPlease delete before continuing".format(this.class, filename.quote)).throw;
		};

		try {
			file = File.new(filename, "w");
		} {
			Error("%: Failed to open file.".format(this.class)).throw;
		};

		if(file.isOpen.not) {
			Error("testAllPossibleStrings: Failed to open file").throw;
		};

		^file;
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
			Error("validate: couldn't find file for validation: %"
				.format(validated.quote)).throw;
		};
		File.exists(filename).not.if {
			Error("validate: couldn't find file for validation: %"
				.format(filename.quote)).throw;
		};

		efile = File(validated, "r");
		afile = File(filename, "r");

		if(efile.isOpen.not) {
			Error("validate: failed to open validation file: %"
				.format(validated.quote)).throw;
		};
		if(afile.isOpen.not) {
			Error("validate: failed to open validation file: %"
				.format(filename.quote)).throw;
		};

		protect {
			var eheader, aheader;

			eheader = this.readHeader(efile);
			aheader = this.readHeader(afile);

			// compare the headers, warn if there are issues
			union(aheader.keys, eheader.keys).do {
				arg key;
				if(aheader[key] != eheader[key]) {
					"validate: headers differ on key %\n"
					"\texpected header has: %\n"
					"\tactual header has: %".format(
						key, aheader[key], eheader[key]
					).warn;
				}
			};

			if(aheader[\strlen] != eheader[\strlen]) {
				Error("validate: string lengths are not equal: % != %"
					.format(aheader[\strlen], eheader[\strlen]));
			};

			// "validate: comparing data".postln;
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
		var inputs = alphs.collect({ |alph, i| alph[ctrs[i]] });

		// master state variables
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
			inputM = alphM[ctrM];

			outputs = inputs.collect({
				arg input, i;
				var result = nil;

				// postf("here: % %\n", input, inputM);
				if(inputM == input) {
					this.incrementAlphabetCount(ctrs[i], strlen, alphSizes[i]);

					if(reps[i] > 0) {
						reps[i] = reps[i]-1;
					} {
						var line, data;

						line = files[i].getLine(this.maxline);
						if(line.size >= (this.maxline-1)) {
							"compareData: maxline characters read from file %: %"
							  .format(files[i], line).warn;
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
				diffs = diffs.add(
					this.mkDataDiff(
						this.stringToHexString(inputM.reduce('++')),
						*outputs
					)
				);
			} {
				// if the outputs matched
				if(areAlphabetsEqual) {
					// and if the alphabets are equal
					while { (reps[0] > 0) && (reps[1] > 0) } {
						// while both reps are greater than 0, we know that the
						// next outputs are going to be equal

						// decrease the reps
						reps = reps - 1;

						// increment the alphabet counters
						this.incrementAlphabetCount(ctrs[0], strlen, alphSizes[0]);
						this.incrementAlphabetCount(ctrs[1], strlen, alphSizes[1]);
						this.incrementAlphabetCount(ctrM, strlen, alphSizeM);
					}
				}
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

		"% entries were missing from file 1"
		  .format(missingFromFile1.size).underlined.postln;

		missingFromFile1.do {
			arg diff;
			this.explainDiff(diff).postln;
		};

		"".postln;
		"% entries were missing from file 2"
		  .format(missingFromFile2.size).underlined.postln;

		missingFromFile2.do {
			arg diff;
			this.explainDiff(diff).postln;
		};

		"".postln;
		"% entries were different between files"
		  .format(realDiffs.size).underlined.postln;

		realDiffs.do {
			arg diff;
			this.explainDiff(diff).postln;
		};
	}

	/////////////////////////////////////
	///// SMALL CONVENIENCE METHODS /////
	/////////////////////////////////////

	// For a given diff, returns a short human-readable description
	// of how the two sides compare.
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
		result[\alphSize] = this.parseAlphabetSize(file.getLine(this.maxline));
		result[\alph] = this.parseAlphabet(file.getLine(this.maxline));
		result[\strlen] = this.parseStringLength(file.getLine(this.maxline));
		result[\prefix] = this.parsePrefix(file.getLine(this.maxline));
		result[\suffix] = this.parseSuffix(file.getLine(this.maxline));
		result[\technique] = this.parseTechnique(file.getLine(this.maxline));
		result[\encoding] = this.parseEncoding(file.getLine(this.maxline));

		this.parseBlockName(file.getLine(this.maxline), "DATA");
		result[\strcnt] = this.parseStringCount(file.getLine(this.maxline));

		^result;
	}

	////////////////////////////////////////////
	/// HEADER READING HELPER METHODS: BEGIN ///
	////////////////////////////////////////////

	*verifyFieldName {
		arg str, expected;

		var len = expected.size;

		if(str.isNil) {
			Error("parseHeader: unexpectedly reached end of document while"
				"parsing %".format(expected.quote)).throw;
		};

		if(str[..len-1] != expected) {
			Error("parseHeader: expected %: got %"
				.format(expected.quote, str[..len-1].quote)).throw;
		};

		^str[len..];
	}

	*parseBlockName {
		arg line, blockName;

		if(line != blockName) {
			Error("readHeader: expected % block, got %"
				.format(blockName.quote, line.quote)).throw;
		};
	}

	*parseAlphabetSize {
		arg str;
		var size;

		str = this.verifyFieldName(str, "alphabet size:");
		size = str.asInteger;

		if(size <= 0) {
			Error("parseHeader: alphabet size must be > 0: got %"
				.format(size.quote)).throw;
		};

		^size;
	}

	*parseAlphabet {
		arg str;
		var alphabet;

		str = this.verifyFieldName(str, "alphabet:");
		alphabet = str.split($,);

		if(alphabet.isEmpty) {
			Error("parseHeader: alphabet is empty: got %"
				.format(str.quote)).throw;
		};

		try {
			^alphabet.collect(this.stringFromHexString(_));
		} {
			arg e;
			Error("parseHeader: error while decoding alphabet %: %"
				.format(alphabet, e.errorString)).throw;
		};
	}

	*parseStringLength {
		arg str;
		var len;

		str = this.verifyFieldName(str, "string length:");
		len = str.asInteger;

		if(len < 0) {
			Error("parseHeader: string length must be nonnegative: got %"
				.format(len)).throw;
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
			Error("parseHeader: error while decoding prefix: %"
				.format(e.errorString)).throw;
		};
	}

	*parseSuffix {
		arg str;

		str = this.verifyFieldName(str, "suffix:");

		try {
			^this.stringFromHexString(str);
		} {
			arg e;
			Error("parseHeader: error while decoding suffix: %"
				.format(e.errorString)).throw;
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
	  ^this.verifyFieldName(str, "encoding:");
	}

	*parseStringCount {
		arg str;
		var cnt = str.asInteger;

		if(cnt <= 0) {
			Error("parseHeader: string count must be > 0: got %".format(cnt)).throw;
		};

		^cnt;
	}
}
