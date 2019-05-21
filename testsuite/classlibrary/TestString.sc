TestString : UnitTest {

	// ------- path-like operations ---------------------------------------------

	test_withTrailingSlash_onEmptyString_addsSeparator {
		var expected = thisProcess.platform.pathSeparator.asString;
		this.assertEquals("".withTrailingSlash, expected);
	}

	test_withTrailingSlash_onPathSeparator_isNoop {
		var sep = thisProcess.platform.pathSeparator.asString;
		this.assertEquals(sep.withTrailingSlash, sep);
	}

	test_withoutTrailingSlash_onEmptyString_isNoop {
		this.assertEquals("".withoutTrailingSlash, "");
	}

	test_withoutTrailingSlash_onSomeString_isNoop {
		var str = "hello";
		this.assertEquals(str.withoutTrailingSlash, str);
	}

	test_withoutTrailingSlash_onSeparator_removesSep {
		var sep = thisProcess.platform.pathSeparator.asString;
		this.assertEquals(sep.withoutTrailingSlash, "");
	}

	// Windows should treat slash as a path sep
	test_withoutTrailingSlash_onSlash_removesSep {
		this.assertEquals("/".withoutTrailingSlash, "");
	}

	// operator +/+
	test_appendPathSep_emptyWithEmpty_producesSep {
		var sep = thisProcess.platform.pathSeparator.asString;
		this.assertEquals("" +/+ "", sep);
	}

	test_appendPathSep_nonSepWithNonSep_producesSep {
		var sep = thisProcess.platform.pathSeparator.asString;
		this.assertEquals("abc" +/+ "def", "abc" ++ sep ++ "def");
	}

	test_appendPathSep_sepWithNonSep_onlyOneSep {
		var sep = thisProcess.platform.pathSeparator.asString;
		var result = ("abc" ++ sep) +/+ ("def");
		var expected = "abc" ++ sep ++ "def";
		this.assertEquals(result, expected);
	}

	test_appendPathSep_nonSepWithSep_onlyOneSep {
		var sep = thisProcess.platform.pathSeparator.asString;
		var result = ("abc") +/+ (sep ++ "def");
		var expected = "abc" ++ sep ++ "def";
		this.assertEquals(result, expected);
	}

	test_appendPathSep_sepWithSep_onlyOneSep {
		var sep = thisProcess.platform.pathSeparator.asString;
		var result = ("abc" ++ sep) +/+ (sep ++ "def");
		var expected = "abc" ++ sep ++ "def";
		this.assertEquals(result, expected);
	}

	// Windows should accept / as a path separator in these cases, and prefer using the LHS slash
	test_appendPathSep_slashWithSlash_onlyOneSep {
		var result = "abc/" +/+ "/def";
		var expected = "abc/def";
		this.assertEquals(result, expected);
	}

	test_appendPathSep_slashWithBackslash_onlyOneSep {
		var result = "abc/" +/+ "\\def";
		var expected = thisProcess.platform.isPathSeparator($\\).if { "abc/def" } { "abc/\\def" };
		this.assertEquals(result, expected);
	}

	test_appendPathSep_backslashWithBackslash {
		var result = "abc\\" +/+ "\\def";
		var expected = thisProcess.platform.isPathSeparator($\\).if { "abc\\def" } { "abc\\/\\def" };
		this.assertEquals(result, expected);
	}

	test_appendPathSep_stringWithPathName_convertsToPathName {
		var result = "abc" +/+ PathName("def");
		var expected = PathName("abc" +/+ "def");
		this.assertEquals(result.fullPath, expected.fullPath);
	}

	// should work with symbols too for backward compatibility
	test_appendPathSep_stringWithSymbol_producesString {
		var sep = thisProcess.platform.pathSeparator.asString;
		this.assertEquals("dir" +/+ 'file', "dir%file".format(sep));
	}

	// regression tests for #4252
	test_standardizePath_withTrailingSlash_shouldNotRemove {
		var result = "~/".standardizePath;
		var expected = "~".standardizePath ++ "/";
		this.assertEquals(result, expected);
	}

	test_standardizePath_withTwoTrailingSlashes_shouldNotRemove {
		var result = "~//".standardizePath;
		var expected = "~".standardizePath ++ "//";
		this.assertEquals(result, expected);
	}

	test_standardizePath_tilde_expandsToHome {
		var result = "~".standardizePath;
		var expected = Platform.userHomeDir;
		this.assertEquals(result, expected);
	}

	// ------- time-related operations -----------------------------------------------

	test_asSecs_stringDddHhMmSsSss_convertsToSeconds {
		var result = "001:01:01:01.001".asSecs;
		var expected = 90061.001;
		this.assertEquals(result, expected);
	}

	test_asSecs_stringSsSss_convertsToSeconds {
		var result = "01.001".asSecs;
		var expected = 1.001;
		this.assertEquals(result, expected);
	}

	test_asSecs_stringMmSs_convertsToSeconds {
		var result = "01:01".asSecs;
		var expected = 61.0;
		this.assertEquals(result, expected);
	}

	test_asSecs_stringSs_convertsToSeconds {
		var result = "01".asSecs;
		var expected = 1.0;
		this.assertEquals(result, expected);
	}

	test_findRegexp_nonEmptyResult {
		var result = "two words".findRegexp("[a-zA-Z]+");
		this.assertEquals(
			result,
			[[0, "two"], [4, "words"]],
			"`\"two words\".findRegexp(\"[a-zA-Z]+\")` should return a nested array of indices and matches"
		)
	}

	test_findRegexp_emptyResult {
		var result = "the quick brown fox".findRegexp("moo");
		this.assertEquals(result, Array.new, "Non-matching findRegexp should return empty array");
	}


	// ------- conversion to Integer -----------------------------------------------

	test_asInteger_base10_conversions {
		this.assertEquals("0".asInteger, 0);
		this.assertEquals("-1".asInteger, -1);
		this.assertEquals("847".asInteger, 847);
		this.assertEquals("-2147483648".asInteger, -2147483648);
		this.assertEquals("2147483647".asInteger, 2147483647);
		this.assertEquals(" \t \n  42  ".asInteger, 42);
		this.assertEquals("  -71".asInteger, -71);
	}

	test_asInteger_base16_conversions {
		this.assertEquals("0x4ad".asInteger, 0x4ad);
		this.assertEquals("-0x497".asInteger, -0x497);
		this.assertEquals("  -0x29A".asInteger, -0x29A);
		this.assertEquals("0xdeadBEEF  ".asInteger, 0xdeadBEEF);
	}

	test_asInteger_radix_conversions {
		this.assertEquals("2r10100110  ".asInteger, 2r10100110);
		this.assertEquals("\n  36rMaus".asInteger, 36rMaus);
		this.assertEquals(" \t -14r10A  ".asInteger, -14r10A);
		this.assertEquals("16rFeEd".asInteger, 16rFeEd);
		this.assertEquals("-39r25".asInteger, -39, "radixes greater than 36 not supported");
		this.assertEquals("1r0".asInteger, 1, "radixes less than 2 not supported");
	}

	test_asInteger_string_length_overflow {
		// Eventually numStr should become so long that parsing will fail, returning 0 instead of
		// the parsed number, but it should not crash or throw an exception.
		var blowUp = { |numStr, targetVal|
			var count = 0;
			if (targetVal == 0, {
				Error("blowUp argument of 0 will cause infinite loop").throw;
			});
			while ({ numStr.asInteger == targetVal }, {
				numStr = " " ++ numStr;
				count = count + 1;
			});
			this.assert(count > 0);
		};

		this.assertNoException({ blowUp.value("100", 100) });
		this.assertNoException({ blowUp.value("-5", -5) });
		this.assertNoException({ blowUp.value("0x416", 0x416) });
		this.assertNoException({ blowUp.value("-0xda0", -0xda0) });
		this.assertNoException({ blowUp.value("5r12340", 5r12340) });
		this.assertNoException({ blowUp.value("-9r334", -9r334) });
		this.assertNoException({ blowUp.value("34r10", 34r10) });
		this.assertNoException({ blowUp.value("-18re1a", -18re1a) });
	}

	// ------- conversion to Float -----------------------------------------------

	test_asFloat_standard_conversions {
		this.assertEquals("0.0005".asFloat, 0.0005);
		this.assertEquals("1.234".asFloat, 1.234);
		this.assertEquals("-0.001".asFloat, -0.001);
		this.assertEquals("  0.0  ".asFloat, 0.0);
		this.assertEquals(" \t2147483649.0001".asFloat, 2147483649.0001);
		this.assertEquals("\n   -2147483649.0001".asFloat, -2147483649.0001);
	}

	test_asFloat_keyword_conversions {
		this.assertEquals("  inf  ".asFloat, inf);
		this.assertEquals("-inf".asFloat, -inf);
		this.assertEquals("pi".asFloat, pi);
		this.assertEquals("-2pi".asFloat, -2pi);
		this.assertEquals("-2.5pi".asFloat, -2.5pi);
		this.assertEquals("0pi".asFloat, 0.0);
	}

	test_asFloat_exponential_notation {
		this.assertEquals("5.872e16".asFloat, 5.872e16);
		this.assertEquals("-1E100".asFloat, -1E100);
		this.assertEquals("4E-2".asFloat, 4E-2);
		this.assertEquals("-19e-74".asFloat, -19e-74);
		this.assertEquals("7.4e-2pi".asFloat, 7.4e-2pi);
		this.assertEquals("  -18E2pi".asFloat, -18E2pi);
	}

	test_asFloat_radix_notation {
		this.assertEquals("12r4A.ABA4".asFloat, 12r4A.ABA4);
		this.assertEquals("-16rfe.0010".asFloat, -16rfe.001);
		this.assertEquals("  2r101.00101".asFloat, 2r101.00101);
		this.assertEquals("-8r73.0742pi".asFloat, -8r73.0742pi);
		this.assertEquals("\t4r3pi".asFloat, 4r3pi);
		this.assertEquals("\n\n\n-36rONE.WAY".asFloat, -36rONE.WAY);
	}

	test_asFloat_scale_degrees {
		this.assertEquals("4b".asFloat, 4b);
		this.assertEquals("-16s".asFloat, -16s);
		this.assertEquals("  8ss".asFloat, 8ss);
		this.assertEquals("  -42bb".asFloat, -42bb);
		this.assertEquals("0bbb".asFloat, 0bbb);
		this.assertEquals("1sss".asFloat, 1sss);
		this.assertEquals("100ssss".asFloat, 100ssss);
		this.assertEquals("-3bbbb".asFloat, -3bbbb);
		this.assertEquals("-2b499".asFloat, -2b499);
		this.assertEquals("7s499".asFloat, 7s499);
		this.assertEquals("4s1".asFloat, 4s1);
		this.assertEquals("-3b002".asFloat, -2b002);
	}
}