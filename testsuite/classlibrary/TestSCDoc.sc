//# VERSION 3.16
TestSCDoc : UnitTest {

	test_helpSourceDirs_excludedExtensions {
		var extensionPaths = [thisProcess.platform.systemExtensionDir, thisProcess.platform.userExtensionDir];
		var prevSetting = LanguageConfig.excludeDefaultPaths;
		var tmpHelpSourcePath = Platform.userExtensionDir +/+ "HelpSource";
		var folderWasCreated = false;
		LanguageConfig.excludeDefaultPaths = true;
		if(File.exists(tmpHelpSourcePath).not) {
			// SCDoc.prRescanHelpSourceDirs will only report entries
			// in userExtensionsDir, if it contains a HelpSource subfolder
			// we create one just for this test
			File.mkdir(tmpHelpSourcePath);
			folderWasCreated = true;
		};
		var result = SCDoc.prRescanHelpSourceDirs.any { |p| extensionPaths.any { |ep| ("^"++ep.escapeChar($\\)).matchRegexp(p) } };
		LanguageConfig.excludeDefaultPaths = prevSetting;
		if(folderWasCreated) { File.delete(tmpHelpSourcePath) };
		this.assert(result.not, "should not search for extensions' HelpSource when LanguageConfig.excludeDefaultPaths = true");
	}

	test_helpSourceDirs_includedExtensions {
		var extensionPaths = [thisProcess.platform.systemExtensionDir, thisProcess.platform.userExtensionDir];
		var prevSetting = LanguageConfig.excludeDefaultPaths;
		var tmpHelpSourcePath = Platform.userExtensionDir +/+ "HelpSource";
		var folderWasCreated = false;
		LanguageConfig.excludeDefaultPaths = false;
		if(File.exists(tmpHelpSourcePath).not) {
			// SCDoc.prRescanHelpSourceDirs will only report entries
			// in userExtensionsDir, if it contains a HelpSource subfolder
			// we create one just for this test
			File.mkdir(tmpHelpSourcePath);
			folderWasCreated = true;
		};
		var result = SCDoc.prRescanHelpSourceDirs.any { |p| extensionPaths.any { |ep| ("^"++ep.escapeChar($\\)).matchRegexp(p) } };
		LanguageConfig.excludeDefaultPaths = prevSetting;
		if(folderWasCreated) { File.delete(tmpHelpSourcePath) };
		this.assert(result, "should search for extensions' HelpSource when LanguageConfig.excludeDefaultPaths = false");
	}


	// Versions in code tags: code#<release>::

	// Parses text as a help file in the given mode (0 full, 2 metadata).
	prParse { |text, mode = 0|
		var path = PathName.tmp +/+ "TestSCDoc_%.schelp".format(this.hash);
		var file = File(path, "w");
		file.write(text);
		file.close;
		var result = SCDoc.prParseFile(path, mode);
		File.delete(path);
		^result
	}

	prDocument { |body|
		^"title:: Test\nsummary:: Test\ndescription::\n" ++ body
	}

	// The first node with the id, depth first.
	prFind { |node, id|
		if(node.id == id) { ^node };
		node.children.do { |child|
			var found = this.prFind(child, id);
			if(found.notNil) { ^found };
		};
		^nil
	}

	test_codeVersion_block {
		var node = this.prFind(this.prParse(this.prDocument("code#3.16::\n1 + 2\n::\n")), \CODEBLOCK);
		this.assertEquals(node.text, "1 + 2", "a versioned code block keeps its text");
		this.assertEquals(node.version, "3.16", "a code block's release is its version");
	}

	test_codeVersion_blockLegacy {
		var node = this.prFind(this.prParse(this.prDocument("code#legacy::\n1 + 2\n::\n")), \CODEBLOCK);
		this.assertEquals(node.version, "legacy", "legacy is a release");
	}

	test_codeVersion_blockWithoutVersion {
		var node = this.prFind(this.prParse(this.prDocument("code::\n1 + 2\n::\n")), \CODEBLOCK);
		this.assertEquals(node.text, "1 + 2", "a code block without a version keeps its text");
		this.assertEquals(node.version, nil, "a code block without a version has none");
		this.assertEquals(node.children, nil, "a code block without a version has no children");
		this.assertEquals(node.isClassCode, false, "a code block without #class is code to evaluate");
	}

	test_codeVersion_classBlock {
		var node = this.prFind(this.prParse(this.prDocument("code#3.16#class::\nFoo { }\n::\n")), \CODEBLOCK);
		this.assertEquals(node.text, "Foo { }", "a class code block keeps its text");
		this.assertEquals(node.version, "3.16", "a class code block's release is its version");
		this.assertEquals(node.isClassCode, true, "#class marks class library code");
	}

	test_codeVersion_classBlockWithoutVersion {
		var node = this.prFind(this.prParse(this.prDocument("code#class::\nFoo { }\n::\n")), \CODEBLOCK);
		this.assertEquals(node.version, nil, "a class code block without a release has no version");
		this.assertEquals(node.isClassCode, true, "code#class:: marks class library code");
	}

	test_codeVersion_versionIsNotClassCode {
		var node = this.prFind(this.prParse(this.prDocument("code#3.16::\n1 + 2\n::\n")), \CODEBLOCK);
		this.assertEquals(node.isClassCode, false, "a version alone is code to evaluate");
	}

	test_codeVersion_inlineClass {
		var node = this.prFind(this.prParse(this.prDocument("Try code#legacy#class:: Foo { } :: now.\n")), \CODE);
		this.assertEquals(node.version, "legacy", "inline class code keeps its release");
		this.assertEquals(node.isClassCode, true, "inline code can be class code");
	}

	test_codeVersion_fragmentBlock {
		var node = this.prFind(this.prParse(this.prDocument("code#fragment::\nchain = 1\n::\n")), \CODEBLOCK);
		this.assertEquals(node.text, "chain = 1", "a fragment keeps its text");
		this.assertEquals(node.version, nil, "a fragment without a release has no version");
		this.assertEquals(node.isFragment, true, "#fragment marks a fragment");
		this.assertEquals(node.isErrorCode, false, "a fragment is not error code");
		this.assertEquals(node.isClassCode, false, "a fragment is not class code");
	}

	test_codeVersion_errorBlock {
		var node = this.prFind(this.prParse(this.prDocument("code#3.16#class#error::\nFoo { } Foo { }\n::\n")), \CODEBLOCK);
		this.assertEquals(node.version, "3.16", "error code keeps its release");
		this.assertEquals(node.isClassCode, true, "error code can be class code");
		this.assertEquals(node.isErrorCode, true, "#error marks code that must fail to compile");
		this.assertEquals(node.isFragment, false, "error code is not a fragment");
	}

	test_codeVersion_inlineFragment {
		var node = this.prFind(this.prParse(this.prDocument("Try code#legacy#fragment:: a.b :: now.\n")), \CODE);
		this.assertEquals(node.version, "legacy", "an inline fragment keeps its release");
		this.assertEquals(node.isFragment, true, "inline code can be a fragment");
	}

	test_codeVersion_blockIndentedAndUpperCase {
		var node = this.prFind(this.prParse(this.prDocument("  CODE#3.16::\n1 + 2\n::\n")), \CODEBLOCK);
		this.assertEquals(node.version, "3.16", "the tag is case insensitive and may be indented");
	}

	test_codeVersion_inline {
		var node = this.prFind(this.prParse(this.prDocument("Try code#3.16:: 1 + 2 :: now.\n")), \CODE);
		this.assertEquals(node.text, "1 + 2", "versioned inline code keeps its text");
		this.assertEquals(node.version, "3.16", "inline code's release is its version");
	}

	test_codeVersion_inlineWithoutVersion {
		var node = this.prFind(this.prParse(this.prDocument("Try code:: 1 + 2 :: now.\n")), \CODE);
		this.assertEquals(node.version, nil, "inline code without a version has none");
	}

	test_codeVersion_badRelease {
		[
			"code#3::\n1\n::\n", "code#latest::\n1\n::\n", "code#3.16 ::\n1\n::\n", "Try code#3.x:: 1 ::\n",
			"code#class#3.16::\n1\n::\n", "code#3.16#Class::\n1\n::\n", "code#3.16#klass::\n1\n::\n", "code##class::\n1\n::\n",
			"code#error#class::\n1\n::\n", "code#fragment#error::\n1\n::\n", "code#error#3.16::\n1\n::\n", "code#3.16#fragment#class::\n1\n::\n"
		].do { |body|
			this.assertEquals(this.prParse(this.prDocument(body)), nil, "a bad release is a parse error: %".format(body.cs));
		};
	}

	test_codeVersion_metadata {
		var text = this.prDocument("code#3.16::\n1 + 2\n::\ncode#class::\nFoo { }\n::\ncode#fragment::\na\n::\nTry code#legacy#class#error:: 1 :: now.\n");
		this.assert(this.prParse(text, 2).notNil, "metadata parsing skips versioned code");
	}

	test_codeVersion_metadataBadRelease {
		var text = this.prDocument("code#latest::\n1 + 2\n::\n");
		this.assertEquals(this.prParse(text, 2), nil, "metadata parsing rejects a bad release");
	}

}
