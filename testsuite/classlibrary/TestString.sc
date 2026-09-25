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

	test_splitext {
		var result = "/foo/bar/baz.xyz".splitext;
		var expected = ["/foo/bar/baz", "xyz"];
		this.assertEquals(result, expected);
	}

	test_splitext_platform_path {
		var p = Platform.pathSeparator;
		var result = (p ++ "foo" ++ p ++ "bar" ++ p ++ "baz.xyz").splitext;
		var expected = [p ++ "foo" ++ p ++ "bar" ++ p ++ "baz", "xyz"];
		this.assertEquals(result, expected);
	}

	test_splitext_no_extension {
		var result = "/foo/bar/baz".splitext;
		var expected = ["/foo/bar/baz", nil];
		this.assertEquals(result, expected);
	}

	test_splitext_no_extension_platform_path {
		var p = Platform.pathSeparator;
		var result = (p ++ "foo" ++ p ++ "bar" ++ p ++ "baz").splitext;
		var expected = [p ++ "foo" ++ p ++ "bar" ++ p ++ "baz", nil];
		this.assertEquals(result, expected);
	}

	test_splitext_early_return {
		var result = "/foo.bar/baz.xyz".splitext;
		var expected = ["/foo.bar/baz", "xyz"];
		this.assertEquals(result, expected);
	}

	test_splitext_early_return_platform_path {
		var p = Platform.pathSeparator;
		var result = (p ++ "foo.bar" ++ p ++ "baz.xyz").splitext;
		var expected = [p ++ "foo.bar" ++ p ++ "baz", "xyz"];
		this.assertEquals(result, expected);
	}

	test_splitext_no_extension_early_return {
		var result = "/foo.bar/baz".splitext;
		var expected = ["/foo.bar/baz", nil];
		this.assertEquals(result, expected);
	}

	test_splitext_no_extension_early_return_platform_path {
		var p = Platform.pathSeparator;
		var result = (p ++ "foo.bar" ++ p ++ "baz").splitext;
		var expected = [p ++ "foo.bar" ++ p ++ "baz", nil];
		this.assertEquals(result, expected);
	}

	test_absolutePath_Windows {
		var drive;
		if (thisProcess.platform.name == \windows) {
			drive = Platform.resourceDir[..1];
			this.assertEquals("C:/test".absolutePath, "C:/test", "absolute path <drive>:/");
			this.assertEquals("C:\\test".absolutePath, "C:\\test", "absolute path <drive>:\\");
			this.assertEquals("\\\\system07\\test".absolutePath, "\\\\system07\\test", "absolute path UNC");
			this.assertEquals("/test".absolutePath, drive ++ "/test", "path starting with /");
			this.assertEquals("\\test".absolutePath, drive ++ "\\test", "path starting with \\");
			this.assertEquals("~/test".absolutePath, "~/test".standardizePath, "path starting with ~");
			this.assertEquals("test".absolutePath, File.getcwd +/+ "test", "relative path");
		}
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

	test_largeStringCompileString {
		var large = String.fill(81920, { |i| "0123456789".wrapAt(i) });
		var reconstructed = large.asCompileString.interpret;
		this.assert(large == reconstructed, "A large string's compileString should interpret back to itself");
	}

	// ------- YAML/JSON -----------------------------------------------

	test_parseYaml_arrays {
		var yaml = "---\n- first\n- second\n- third";
		var result = ["first", "second", "third"];
		var expected = yaml.parseYAML;
		this.assertEquals(result, expected);
	}

	test_parseYaml_dictionaries {
		var yaml = "---\nkeyA: valueA\nkeyB: valueB";
		var result = Dictionary.newFrom([
			"keyA", "valueA",
			"keyB", "valueB"
		]);
		var expected = yaml.parseYAML;
		this.assertEquals(result, expected);
	}

	test_parseYaml_scalars {
		var yaml = "--- single_scalar_string";
		var result = "single_scalar_string";
		var expected = yaml.parseYAML;
		this.assertEquals(result, expected);
	}

	test_parseYaml_nested_structures {
		var yaml = "---\nparent:\n  - child1\n  - child2\nconfig:\n  status: active";
		var result = Dictionary.newFrom([
			"parent", ["child1", "child2"],
			"config", Dictionary.newFrom(["status", "active"])
		]);
		var expected = yaml.parseYAML;
		this.assertEquals(result, expected);
	}

	test_parseJson_arrays {
		var json = "[\"one\", \"two\", \"three\"]";
		var result = ["one", "two", "three"];
		var expected = json.parseJSON;
		this.assertEquals(result, expected);
	}

	test_parseJson_dictionaries {
		var json = "{\"node_id\": \"1001\", \"type\": \"sine\"}";
		var result = Dictionary.newFrom([
			"node_id", "1001",
			"type", "sine"
		]);
		var expected = json.parseJSON;
		this.assertEquals(result, expected);
	}

	test_parseJson_nested {
		var json = "{\"synth\": {\"freq\": \"440\", \"amp\": \"0.5\"}, \"fx\": [\"reverb\", \"delay\"]}";
		var result = Dictionary.newFrom([
			"synth", Dictionary.newFrom(["freq", "440", "amp", "0.5"]),
			"fx", ["reverb", "delay"]
		]);
		var expected = json.parseJSON;
		this.assertEquals(result, expected);
	}

	test_asStringLimit {
		this.assert((0..1024).asString(32).size < (0..1024).asString, "asString should pass on the limit argument and make the string shorter.");
	}
}
