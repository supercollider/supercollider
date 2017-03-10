TestLPCTestUtils : UnitTest {
	const <testDir = "/Users/brianheim/Desktop/";

	var afile, efile;

	///////////////////////////
	/// TESTING CompareData ///
	///////////////////////////

	/**** HELPERS ****/

	setUpComparisonFiles {
		arg entries1, entries2;

		afile = File(testDir +/+ "afile", "w");
		efile = File(testDir +/+ "efile", "w");

		entries1.do({
			arg entry;
			afile.write(entry);
			afile.putChar($\n);
		});
		entries2.do({
			arg entry;
			efile.write(entry);
			efile.putChar($\n);
		});

		afile.close;
		efile.close;

		afile = File(testDir +/+ "afile", "r");
		efile = File(testDir +/+ "efile", "r");
	}

		tearDownComparisonFiles {
		afile.close;
		efile.close;
		File.delete(testDir +/+ "afile");
		File.delete(testDir +/+ "efile");
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

	/**** NO-DIFF ****/

	test_compareData_noDiff_oneEntry {
		// equality tests for a single entry
		var input = "6567";
		var alph = [LPCTestUtils.stringFromHexString(input).postln];
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
		afile = File(testDir +/+ "afile", "w+");
	}

	tearDownParserFile {
		afile.close;
		File.delete(testDir +/+ "afile");
	}

	test_writeHeader_success {
		this.setUpParserFile;
		LPCTestUtils.writeHeader(afile, ["AB","CD"], 3, "pre", "suf", \compile, 4);
		this.tearDownParserFile;
	}

	test_writeHeader_failure {
		this.setUpParserFile;
		protect {
			try {
				LPCTestUtils.writeHeader(afile, "ABCD", 1, "pre", "suf", \compile, 100);
				this.failed(thisMethod, "Should not accept non-array alphabet");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, [], 2, "pre", "suf", \compile, 100);
				this.failed(thisMethod, "Should not accept empty alphabet");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, [\sym, \bol], 2, "pre", "suf", \compile, 100);
				this.failed(thisMethod, "Should not accept non-string array alphabet");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB","CD"], -1, "pre", "suf", \compile, 100);
				this.failed(thisMethod, "Should not accept negative string length");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB", "CD"], 2, 3, "suf", \compile, 100);
				this.failed(thisMethod, "Should not accept non-string prefix");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB", "CD"], 2, "pre", 3, \compile, 100);
				this.failed(thisMethod, "Should not accept non-string suffix");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB", "CD"], 2, "pre", "suf", \notcompile, 100);
				this.failed(thisMethod, "Should not accept bad technique");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB", "CD"], 2, "pre", "suf", \compile, 0);
				this.failed(thisMethod, "Should not accept zero stringCount");
			} {};
			try {
				LPCTestUtils.writeHeader(afile, ["AB", "CD"], 2, "pre", "suf", \compile, -1);
				this.failed(thisMethod, "Should not accept negative stringCount");
			} {};
		} {
			this.tearDownParserFile;
		}
	}

	test_readHeader_success {
		var header = Dictionary[];
		var readHeader;

		header[\alphSize] = 2;
		header[\alph] = ["AB", "CD"];
		header[\strlen] = 4;
		header[\prefix] = "testPre";
		header[\suffix] = "testSuf";
		header[\technique] = \compile;
		header[\encoding] = LPCTestUtils.getEncodingString(\compile);
		header[\strcnt] = 4;

		this.setUpParserFile;
		LPCTestUtils.writeHeader(afile,
			header[\alph],
			header[\strlen],
			header[\prefix],
			header[\suffix],
			header[\technique],
			header[\strcnt]
		);

		afile.seek(0);

		readHeader = LPCTestUtils.readHeader(afile);

		header.keys.do {
			arg key;
			this.assertEquals(header[key], readHeader[key], "Written and read header should match for each key");
			readHeader.removeAt(key);
		};
		this.assertEquals(readHeader.keys, Set[], "Header read from file should have no remaining keys");
		this.tearDownParserFile;
	}

	//////////////////////////////
	/// TESTING HELPER METHODS ///
	//////////////////////////////

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

		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [0,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,0], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,1], report:false);

		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n).not);
		this.assertEquals(ctr, [0,0]);
	}

	test_incrementAlphabetCount_3x3 {
		var ctr = [0,0,0];
		var len = 3;
		var n = 3;

		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [0,0,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [0,0,2], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [0,1,0], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [0,1,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [0,1,2], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [0,2,0], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [0,2,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [0,2,2], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,0,0], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,0,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,0,2], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,1,0], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,1,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,1,2], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,2,0], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,2,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [1,2,2], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [2,0,0], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [2,0,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [2,0,2], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [2,1,0], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [2,1,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [2,1,2], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [2,2,0], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [2,2,1], report:false);
		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n), report:false);
		this.assertEquals(ctr, [2,2,2], report:false);

		this.assert(LPCTestUtils.incrementAlphabetCount(ctr, len, n).not);
		this.assertEquals(ctr, [0,0,0]);
	}
}