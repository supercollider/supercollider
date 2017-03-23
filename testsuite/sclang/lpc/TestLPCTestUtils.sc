// TestLPCTestUtils.sc
// Brian Heim
//
// Tests for the LPCTestUtils class. Does not have full coverage yet.

TestLPCTestUtils : UnitTest {
	var afile, efile;

	setUp {
		// never use strict output checking, always turn on debugging
		LPCTestUtils.strictOutputChecking = false;
		LPCTestUtils.doDebug = true;
	}

	///////////////////////////
	/// TESTING CompareData ///
	///////////////////////////

	/**** HELPERS ****/

	openComparisonFiles {
		arg mode;
		afile = File("afile".resolveRelative, mode);
		efile = File("efile".resolveRelative, mode);
	}

	closeComparisonFiles {
		afile.close;
		efile.close;
	}

	deleteComparisonFiles {
		File.delete("afile".resolveRelative);
		File.delete("efile".resolveRelative);
	}

	writeEntriesToFile {
		arg file, entries;

		entries.do {
			arg entry;
			file.write(entry);
			file.putChar($\n);
		}
	}

	doCompareDataEqualityTest {
		arg entries1, entries2, alph, strlen;
		var diffs;

		this.setUpComparisonFiles(entries1, entries2);
		diffs = LPCTestUtils.compareData(afile, efile, alph, alph, strlen);
		this.assert(diffs.isEmpty, "There should be no reported diffs: %".format(diffs));
		this.tearDownComparisonFiles();
	}

	doCompareDataDiffTest {
		arg entries1, entries2, alph1, alph2, strlen, expDiffs;
		var diffs;

		this.setUpComparisonFiles(entries1, entries2);
		diffs = LPCTestUtils.compareData(afile, efile, alph1, alph2, strlen);
		this.assertEquals(diffs, expDiffs, "Should find diffs: %".format(expDiffs));
		this.tearDownComparisonFiles();
	}

	/**** SETUP & TEARDOWN ****/

	setUpComparisonFiles {
		arg entries1, entries2;

		this.openComparisonFiles("w");

		this.writeEntriesToFile(afile, entries1);
		this.writeEntriesToFile(efile, entries2);

		this.closeComparisonFiles();

		this.openComparisonFiles("r");
	}

	tearDownComparisonFiles {
		this.closeComparisonFiles();
		this.deleteComparisonFiles();
	}

	/**** NO-DIFF ****/

	test_compareData_noDiff_oneEntry {
		// equality tests for a single entry
		var input = "6567";
		var alph = [LPCTestUtils.stringFromHexString(input)];
		var strlen = 1;
		var singleEntryList = [
			"6567\t0123:Symbol",
			"6567\t"++LPCTestUtils.compileErrorString,
			"6567\t"+LPCTestUtils.runtimeErrorString,
			"6567\t:String",
			// these are okay even though they don't make sense right now
			"6567\t0123:Symbol\t0",
			"6567\t0123:Symbol\t1",
			"6567\t0123:Symbol\t1000",
			"6567\t"++LPCTestUtils.compileErrorString++"\t300",
			"6567\t"+LPCTestUtils.runtimeErrorString++"\t300"
		];

		singleEntryList.do {
			|entry|
			this.doCompareDataEqualityTest([entry], [entry], alph, strlen);
		};
	}

	test_compareData_noDiff_fourEntries {
		var alph = ["A", "B"];
		var trAlph = alph.collect(LPCTestUtils.stringToHexString(_));
		var strlen = 2;

		var entries = [
			trAlph[0]++trAlph[0]++"\t01:Integer",
			trAlph[0]++trAlph[1]++"\t02:Float",
			trAlph[1]++trAlph[0]++"\t03:Number",
			trAlph[1]++trAlph[1]++"\t04:Magnitude",
		];

		this.doCompareDataEqualityTest(entries, entries, alph, 2);
	}

	test_compareData_noDiff_fourEntries_rolled {
		var alph = ["A", "B"];
		var trAlph = alph.collect(LPCTestUtils.stringToHexString(_));
		var strlen = 2;
		var output = "\t01:Sassafrass";

		var entries_unrolled = [
			trAlph[0]++trAlph[0]++output,
			trAlph[0]++trAlph[1]++output,
			trAlph[1]++trAlph[0]++output,
			trAlph[1]++trAlph[1]++output
		];

		var entries_rolled = [trAlph[0]++trAlph[0]++output++"\t3"];

		this.doCompareDataEqualityTest(entries_unrolled, entries_rolled, alph, 2);
	}

	/**** DIFF ****/

	test_compareData_diff_oneEntry {
		var alph = ["A"];
		var trAlph = alph.collect(LPCTestUtils.stringToHexString(_));
		var strlen = 1;

		var entry1 = [trAlph[0]++"\t01:Integer"];
		var entry2 = [trAlph[0]++"\t02:Integer"];

		var diffs, expDiffs;

		expDiffs = [[trAlph[0], ["01", "Integer"], ["02", "Integer"]]];

		this.doCompareDataDiffTest(entry1, entry2, alph, alph, strlen, expDiffs);
	}

	test_compareData_diff_fourEntries {
		var alph = ["q", "x"];
		var trAlph = alph.collect(LPCTestUtils.stringToHexString(_));
		var strlen = 2;
		var output1 = "\t01:Sassafrass";
		var output2 = "\t" ++ LPCTestUtils.compileErrorString;

		var inputs = [
			trAlph[0]++trAlph[0],
			trAlph[0]++trAlph[1],
			trAlph[1]++trAlph[0],
			trAlph[1]++trAlph[1]
		];

		var entries1 = inputs.collect(_++output1);

		var entries2 = inputs.collect({
			arg in, i;

			if(i == 2) {
				in++output2;
			} {
				in++output1;
			};
		});

		var diffs, expDiffs;

		expDiffs = [[trAlph[1]++trAlph[0], ["01", "Sassafrass"], [LPCTestUtils.compileErrorString]]];

		this.doCompareDataDiffTest(entries1, entries2, alph, alph, strlen, expDiffs);
	}

		test_compareData_diff_fourEntries_multiCharAlphabet {
		var alph = ["qrazy", "xclusive"];
		var trAlph = alph.collect(LPCTestUtils.stringToHexString(_));
		var strlen = 2;
		var output1 = "\t01:Sassafrass";
		var output2 = "\t" ++ LPCTestUtils.compileErrorString;

		var inputs = [
			trAlph[0]++trAlph[0],
			trAlph[0]++trAlph[1],
			trAlph[1]++trAlph[0],
			trAlph[1]++trAlph[1]
		];

		var entries1 = inputs.collect(_++output1);

		var entries2 = inputs.collect({
			arg in, i;

			if(i == 2) {
				in++output2;
			} {
				in++output1;
			};
		});

		var diffs, expDiffs;

		expDiffs = [[trAlph[1]++trAlph[0], ["01", "Sassafrass"], [LPCTestUtils.compileErrorString]]];

		this.doCompareDataDiffTest(entries1, entries2, alph, alph, strlen, expDiffs);
	}

	test_compareData_diff_fourEntries_rolled {
		var alph = ["q", "x"];
		var trAlph = alph.collect(LPCTestUtils.stringToHexString(_));
		var strlen = 2;
		var output1 = "\t01:Sassafrass";
		var output2 = "\t" ++ LPCTestUtils.compileErrorString;
		var output3 = "\t05:Jigglypuff";
		var output4 = "\t04:Sassafrass";

		var entries1 = [
			trAlph[0]++trAlph[0]++output1,
			trAlph[0]++trAlph[1]++output2,
			trAlph[1]++trAlph[0]++output3,
			trAlph[1]++trAlph[1]++output4
		];

		var entries2 = [
			trAlph[0]++trAlph[0]++output3++"\t3";
		];

		var diffs, expDiffs;

		expDiffs = [
			[trAlph[0]++trAlph[0], output1[1..].split($:), output3[1..].split($:)],
			[trAlph[0]++trAlph[1], output2[1..].split($:), output3[1..].split($:)],
			[trAlph[1]++trAlph[1], output4[1..].split($:), output3[1..].split($:)],
		];

		this.doCompareDataDiffTest(entries1, entries2, alph, alph, strlen, expDiffs);
	}

	test_compareData_diff_fourEntries_diffAlphabets {
		var alph1 = ["var", "{}"];
		var alph2 = ["while", "{}"];

		var trAlph1 = alph1.collect(LPCTestUtils.stringToHexString(_));
		var trAlph2 = alph2.collect(LPCTestUtils.stringToHexString(_));
		var strlen = 2;
		var output1 = "01:Sassafrass";
		var output2 = LPCTestUtils.compileErrorString;
		var output3 = "05:Jigglypuff";

		var entries1 = [
			trAlph1[0]++trAlph1[0]++"\t"++output1, // varvar
			trAlph1[0]++trAlph1[1]++"\t"++output2, // var{}
			trAlph1[1]++trAlph1[0]++"\t"++output1, // {}var
			trAlph1[1]++trAlph1[1]++"\t"++output2  // {}{}
		];

		var entries2 = [
			trAlph2[0]++trAlph2[0]++"\t"++output1, // whilewhile
			trAlph2[0]++trAlph2[1]++"\t"++output2, // while{}
			trAlph2[1]++trAlph2[0]++"\t"++output3, // {}while
			trAlph2[1]++trAlph2[1]++"\t"++output2  // {}{}
		];

		var diffs, expDiffs;

		// entries1.postln;
		// entries2.postln;

		// vv, vw, v{, wv, ww, w{, {v, {w, {{
		// d,  _,  d,  _,  d,  d,  d,  d,  m
		expDiffs = [
			[trAlph1[0]++trAlph1[0], output1.split($:), nil],
			[trAlph1[0]++trAlph1[1], output2.split($:), nil],
			[trAlph2[0]++trAlph2[0], nil, output1.split($:)],
			[trAlph2[0]++trAlph2[1], nil, output2.split($:)],
			[trAlph1[1]++trAlph1[0], output1.split($:), nil],
			[trAlph2[1]++trAlph2[0], nil, output3.split($:)],
		];

		this.doCompareDataDiffTest(entries1, entries2, alph1, alph2, strlen, expDiffs);
	}

	//////////////////////////////
	/// TESTING HEADER METHODS ///
	//////////////////////////////

	setUpParserFile {
		afile = File("afile".resolveRelative, "w+");
	}

	tearDownParserFile {
		afile.close;
		File.delete("afile".resolveRelative);
	}

	test_writeHeader_success {
		this.setUpParserFile;
		LPCTestUtils.writeHeader(afile, ["AB","CD"], 3, "pre", "suf", \interpret, 4);
		this.tearDownParserFile;
	}

	test_writeHeader_failure {
		this.setUpParserFile;
		protect {
			try {
				LPCTestUtils.writeHeader(afile, "ABCD", 1, "pre", "suf", \interpret, 100);
				this.failed(thisMethod, "Should not accept non-array alphabet");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, [], 2, "pre", "suf", \interpret, 100);
				this.failed(thisMethod, "Should not accept empty alphabet");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, [\sym, \bol], 2, "pre", "suf", \interpret, 100);
				this.failed(thisMethod, "Should not accept non-string array alphabet");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB","CD"], -1, "pre", "suf", \interpret, 100);
				this.failed(thisMethod, "Should not accept negative string length");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB", "CD"], 2, 3, "suf", \interpret, 100);
				this.failed(thisMethod, "Should not accept non-string prefix");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB", "CD"], 2, "pre", 3, \interpret, 100);
				this.failed(thisMethod, "Should not accept non-string suffix");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB", "CD"], 2, "pre", "suf", "compile", 100);
				this.failed(thisMethod, "Should not accept bad technique");
			} {};
		} {
			this.tearDownParserFile;
		}
	}

	test_parseHeader_success {
		var header = Dictionary[];
		var parsedHeader;

		header[\alphSize] = 2;
		header[\alph] = ["AB", "CD"];
		header[\strlen] = 4;
		header[\prefix] = "testPre";
		header[\suffix] = "testSuf";
		header[\technique] = \interpret;

		this.setUpParserFile;
		LPCTestUtils.writeHeader(afile,
			header[\alph],
			header[\strlen],
			header[\prefix],
			header[\suffix],
			header[\technique]
		);

		afile.seek(0);

		parsedHeader = LPCTestUtils.parseHeader(afile);

		header.keys.do {
			arg key;
			this.assertEquals(header[key], parsedHeader[key], "Written and read header should match for each key");
			parsedHeader.removeAt(key);
		};
		this.assertEquals(parsedHeader.keys, Set[], "Header read from file should have no remaining keys");
		this.tearDownParserFile;
	}

	//////////////////////////////
	/// TESTING HELPER METHODS ///
	//////////////////////////////

	/**** evaluateString ****/

	test_evaluateString_error {
		var string = "a string";
		var technique = \badTechnique;

		try {
			LPCTestUtils.evaluateString(string, technique);
			this.failed(thisMethod, "passing a bad technique to evaluateString should throw an error");
		} {};
	}

	/**** DATA CONVERSION ****/

	test_stringToFromHexString {
		var input = (-256..256).collect(_.asAscii).reduce('++');
		var hexString = LPCTestUtils.stringToHexString(input);
		var output = LPCTestUtils.stringFromHexString(hexString);
		this.assertEquals(input, output, "stringToHexString and stringFromHexString should be perfect inverses.");
	}

	test_bytecodeToFromHexString {
		var input = Int8Array.newFrom((-256..256));
		var hexString = LPCTestUtils.bytecodeToHexString(input);
		var output = LPCTestUtils.bytecodeFromHexString(hexString);
		this.assertEquals(input, output, "stringToHexString and stringFromHexString should be perfect inverses.");
	}

	/**** doOutputsMatch ****/

	test_doOutputsMatch_nil {
		var in = "";
		var a = nil;
		var b = nil;

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b), "doOutputsMatch: nil should match nil");
	}

	test_doOutputsMatch_oneElementSame {
		var in = "";
		var a = "0123";
		var b = "0123";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b), "doOutputsMatch: equivalent outputs should match (one element)");
	}

	test_doOutputsMatch_twoElementSame {
		var in, a, b;
		in = "";
		a = b = "0123:Nil";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b), "doOutputsMatch: equivalent outputs should match (two elements)");
	}

	test_doOutputsMatch_oneElementDifferent {
		var in = "";
		var a = "1234";
		var b = "5678";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b).not, "doOutputsMatch: different outputs should not match (one element)");
	}

	test_doOutputsMatch_twoElementDifferent {
		var in = "";
		var a = "0123:Nil";
		var b = "0123:Object";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b).not, "doOutputsMatch: different outputs should not match (two elements)");
	}

	// whatever the output of the other is, if input is LID, doOutputsMatch should return true
	test_doOutputsMatch_LID {
		var in = ["L", "I", "D"];
		var a = "4567:Nil";
		var b = "4568:Object";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b), "doOutputsMatch: input with class LID should be ignored");
	}

	test_doOutputsMatch_MetaLID {
		var in = ["LID"];
		var a = "4567:Nil";
		var b = "4568:Meta_LID";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b), "doOutputsMatch: output with class Meta_LID should be ignored");
	}

	test_doOutputsMatch_LID_compileError {
		var in = ["L", "I", "D"];
		var a = "!cErr";
		var b = "4568:LID";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b), "doOutputsMatch: output with class LID should be ignored");
	}

	test_doOutputsMatch_MetaLID_compileError {
		var in = ["L", "I", "D"];
		var a = "!cErr";
		var b = "4568:Meta_LID";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b), "doOutputsMatch: output with class Meta_LID should be ignored");
	}

	test_doOutputsMatch_NaN {
		var in = "0/0";
		var a = LPCTestUtils.stringToHexString("-nan") ++ ":Float";
		var b = LPCTestUtils.stringToHexString("nan") ++ ":Float";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b), "doOutputsMatch: -nan and nan should be treated as equivalent");
	}

	test_doOutputsMatch_NaNInteger {
		var in = "0 div: 0";
		var a = LPCTestUtils.stringToHexString("-nan") ++ ":Integer";
		var b = LPCTestUtils.stringToHexString("nan") ++ ":Integer";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b).not, "doOutputsMatch: -nan and nan should be treated as equivalent only with Floats");
	}

	// Windows tests
	test_doOutputsMatch_windows_NaN {
		var in = "0/0";
		var nanStrings = ["nan", "-nan", "1.#IND", "-1.#IND"];

		nanStrings = nanStrings.dup.allTuples;

		nanStrings.do {
			arg pair;
			var testPair = pair.collect { |x| LPCTestUtils.stringToHexString(x) ++ ":Float" };
			this.assert(LPCTestUtils.doOutputsMatch(in, *testPair), "doOutputsMatch: nan (\"%\") and nan (\"%\") should be treated as equivalent".format(*pair));
		}
	}

	test_doOutputsMatch_windows_inf {
		var in = "0/0";
		var infStrings = ["inf", "1.#INF"];

		infStrings = infStrings.dup.allTuples;

		infStrings.do {
			arg pair;
			var testPair = pair.collect { |x| LPCTestUtils.stringToHexString(x) ++ ":Float" };
			this.assert(LPCTestUtils.doOutputsMatch(in, *testPair), "doOutputsMatch: inf (\"%\") and inf (\"%\") should be treated as equivalent".format(*pair));
		}
	}

	test_doOutputsMatch_windows_inf_neg {
		var in = "0/0";
		var infStrings = ["-inf", "-1.#INF"];

		infStrings = infStrings.dup.allTuples;

		infStrings.do {
			arg pair;
			var testPair = pair.collect { |x| LPCTestUtils.stringToHexString(x) ++ ":Float" };
			this.assert(LPCTestUtils.doOutputsMatch(in, *testPair), "doOutputsMatch: -inf (\"%\") and -inf (\"%\") should be treated as equivalent".format(*pair));
		}
	}

	test_doOutputsMatch_windows_inf_noMatch {
		var in = "0/0";
		var infStrings = [["-inf", "-1.#INF"], ["inf", "1.#INF"]];

		infStrings = infStrings.allTuples;

		infStrings.do {
			arg pair;
			var testPair = pair.collect { |x| LPCTestUtils.stringToHexString(x) ++ ":Float" };
			this.assert(LPCTestUtils.doOutputsMatch(in, *testPair).not, "doOutputsMatch: -inf (\"%\") and -inf (\"%\") should be treated as equivalent".format(*pair));
		}
	}

	// Thanks to Nathan Ho
	test_doOutputsMatch_floatPrecision_match {
		var in = ".Q8";
		var a = LPCTestUtils.stringToHexString("0.72839506172839") ++ ":Float";
		var b = LPCTestUtils.stringToHexString("0.7283950617284") ++ ":Float";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b), "doOutputsMatch: very close floats should be treated as equivalent");
	}

	// Test just on the other side of the precision limit
	test_doOutputsMatch_floatPrecision_noMatch {
		var in = "not .Q8";
		var a = LPCTestUtils.stringToHexString("0.7283950617239") ++ ":Float";
		var b = LPCTestUtils.stringToHexString("0.728395061724") ++ ":Float";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b).not, "doOutputsMatch: not-so-close floats (distance > %) should not be treated as equivalent".format(LPCTestUtils.floatEqualityPrec));
	}

	test_doOutputsMatch_floatPrecision_noMatch_distant {
		var in = "not .Q8";
		var a = LPCTestUtils.stringToHexString("0.9") ++ ":Float";
		var b = LPCTestUtils.stringToHexString("0.2") ++ ":Float";

		this.assert(LPCTestUtils.doOutputsMatch(in, a, b).not, "doOutputsMatch: not-so-close floats (0.2, 0.9) should not be treated as equivalent");
	}

	/**** incrementAlphabetCount ****/

	test_incrementAlphabetCount_1x1 {
		var ctr = [0];
		var len = 1;
		var n = 1;

		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n).not);
		this.assertEquals(ctr, [0]);
	}

	test_incrementAlphabetCount_1x99 {
		var ctr = [0];
		var len = 1;
		var n = 99;

		98.do {
			arg i;
			this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
			this.assertEquals(ctr, [i+1], report:false);
		};

		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n).not);
		this.assertEquals(ctr, [0]);
	}

	test_incrementAlphabetCount_2x2 {
		var ctr = [0,0];
		var len = 2;
		var n = 2;

		[[0,1], [1,0], [1,1]].do {
			arg tuple;

			this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
			this.assertEquals(ctr, tuple, report:false);
		};

		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n).not);
		this.assertEquals(ctr, [0,0]);
	}

	test_incrementAlphabetCount_3x3 {
		var ctr = [0,0,0];
		var len = 3;
		var n = 3;

		var tuples = ([0,1,2]!3).allTuples;

		tuples = tuples[1..]; // ignoring [0,0,0]

		tuples.do {
			arg tuple;

			this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
			this.assertEquals(ctr, tuple, report:false);
		};

		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n).not);
		this.assertEquals(ctr, [0,0,0]);
	}
}