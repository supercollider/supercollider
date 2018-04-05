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

	// Windows should accept / as a path separator in this case
	test_appendPathSep_slashWithSlash_onlyOneSep {
		var sep = thisProcess.platform.pathSeparator.asString;
		var result = "abc/" +/+ "/def";
		var expected = "abc" ++ sep ++ "def";
		this.assertEquals(result, expected);
	}

}

