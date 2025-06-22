
/*
TestPathName provides full coverage of all PathName.methods in 3.13,
to ensure full backwards compatibility.

// class methods:
c = PathName.class.methods.collect(_.name).postcs
c.size
m = PathName.methods.collect(_.name).postcs
m.size

// same in 3.13 and 3.14 dev 25_06_01:
// 6 class methods
['scroot', 'scroot_', 'tmp', 'tmp_', 'new', 'initClass']

// methods
['fullPath', 'colonIndices', 'fileName', 'fileNameWithoutExtension', 'fileNameWithoutDoubleExtension', 'extension', 'pathOnly', 'diskName', 'isRelativePath', 'isAbsolutePath', 'absolutePath', 'asRelativePath', 'asAbsolutePath', 'allFolders', 'folderName', 'lastColonIndex', 'nextName', 'noEndNumbers', 'endNumber', 'endNumberIndex', '+/+', 'entries', 'pathMatch', 'isFolder', 'isFile', 'files', 'folders', 'deepFiles', 'parentPath', 'filesDo', 'streamTree', 'dumpTree', 'printOn', 'dumpToDoc']


Tests for all PathName methods are organized by group:

// simple filename/path-related:
'fullPath', 'colonIndices', 'lastColonIndex',
'fileName', 'fileNameWithoutExtension', 'fileNameWithoutDoubleExtension', 'extension', 'pathOnly', 'diskName', 'allFolders', 'folderName',
'nextName', 'noEndNumbers', 'endNumber', 'endNumberIndex',
'parentPath',

// tests
'isRelativePath', 'isAbsolutePath', 'isFolder', 'isFile',

// conversions
'absolutePath', 'asRelativePath', 'asAbsolutePath', '+/+',

// file/folder access
'entries', 'pathMatch', 'files', 'folders', 'deepFiles', 'filesDo',

// info/posting -
'streamTree', 'dumpTree', 'printOn', 'dumpToDoc',
only streamTree needs test, the others are convenience.

bench { TestPathName.run };

*/
TestPathName : UnitTest {

	test_classMethods {
		this.assert(PathName.scroot == File.getcwd, "scroot");
		this.assert(PathName.tmp == Platform.defaultTempDir, "tmp");
	}

	// all string manipulation methods
	test_filePathMethods {

		var p = PathName("/Users/xyz/src/PathNameTest.abc.scd");

		this.assertEquals(p.fullPath, "/Users/xyz/src/PathNameTest.abc.scd");

		// keep List type for full compat
		this.assertEquals(p.colonIndices, List[0, 6, 10, 14], "colonIndices");

		this.assertEquals(p.fileName, "PathNameTest.abc.scd", "fileName");
		this.assertEquals(p.fileNameWithoutExtension, "PathNameTest.abc",
			"fileNameWithoutExtension");
		this.assertEquals(p.fileNameWithoutDoubleExtension, "PathNameTest",
			"fileNameWithoutDoubleExtension");
		this.assertEquals(p.extension, "scd", "extension");

		// pathOnly removes only filenames, ends with a training slash
		this.assertEquals(p.pathOnly, "/Users/xyz/src/", "pathOnly w file");
		// pathOnly keeps folders with a training slash as is
		this.assertEquals("/Users/xyz/src/", "/Users/xyz/src/", "pathOnly w folder");

		// parentPath removes filename at end
		this.assertEquals(p.parentPath, "/Users/xyz/src/", "parentPath w file");
		// parentPath also removes last folder with a training slash:
		this.assertEquals(PathName("/Users/xyz/src/").parentPath,
			"/Users/xyz/", "parentPath w folder");

		// diskName expects no separator at beginning ...
		this.assertEquals(PathName("ABC/def/ghj.scd").diskName, "ABC", "diskName");

		this.assertEquals(p.allFolders, List["Users", "xyz", "src"], "allFolders");

		this.assertEquals(p.folderName, "src", "folderName");

		this.assertEquals(p.lastColonIndex, 14, "lastColonIndex when it exists");
		this.assertEquals(PathName("123").lastColonIndex, -1,
			"lastColonIndex when not found");

		this.assertEquals(PathName("abc").nextName, "abc1",
			"nextName, no number, no extension");
		this.assertEquals(PathName("abc9").nextName, "abc10",
			"nextName with number, no extension");
		this.assertEquals(PathName("abc9").endNumber, 9, "endNumber");
		this.assertEquals(PathName("abc").endNumber, 0, "endNumber");
		this.assertEquals(PathName("abc9").endNumberIndex, 2, "endNumber");
		this.assertEquals(PathName("abc").endNumberIndex, 2, "endNumber");
		this.assertEquals(PathName("abc9").noEndNumbers, "abc", "endNumber");
		this.assertEquals(PathName("abc").noEndNumbers, "abc", "endNumber");

		// PathName has no == test, so we check the fullPaths
		this.assertEquals((PathName("abc/") +/+ PathName("def/")).fullPath,
			PathName("abc/def/").fullPath, "PathName +/+ PathName");
		this.assertEquals((PathName("abc/") +/+ "def/").fullPath,
			PathName("abc/def/").fullPath, "PathName +/+ String");
		this.assertEquals(("def/" +/+ PathName("abc/")).fullPath,
			PathName("def/abc/").fullPath, "String +/+ PathName");

	}

	test_pathTests {
		var p = PathName("/Users/xyz/src/PathNameTest.abc.scd");

		// true, because ~/ expanded correctly
		this.assert(p.isAbsolutePath, "isAbsolutePath");
		this.assert(PathName("relative.scd").isRelativePath, "isRelativePath");

		this.assert(PathName("~/").isFolder, "isFolder: home should be a folder");
		this.assert(PathName("~/").isFile.not, "isFile: home should not be a file");
	}

	test_conversions {
		var p = PathName("/Users/xyz/src/PathNameTest.abc.scd");

		this.assertEquals(p.asRelativePath("/Users/xyz/src"),
			"PathNameTest.abc.scd", "asRelativePath");
		this.assertEquals(p.asAbsolutePath, p.fullPath, "asAbsolutePath w absolute");

		this.assertEquals(PathName("relative.scd").asAbsolutePath,
			// duplicate String:absolutePath behavior
			File.getcwd ++ Platform.pathSeparator ++ "relative.scd",
			"asAbsolutePath w relative filename");
		this.assertEquals(PathName("relative.scd").absolutePath,
			File.getcwd ++ Platform.pathSeparator ++ "relative.scd",
			"absolutePath w relative filename");
	}

	test_fileAndFolderAccessMethods {
		// take known folders to test:
		var defaultLibFolder = Main.filenameSymbol.asString.dirname;
		var classLibFolder = Platform.classLibraryDir;
		var fileCount = 0, postStream;

		//  hierarchical access to files and folders
		// [ 'isFolder', 'isFile', 'entries', 'files', 'folders', 'deepFiles', 'parentPath',
		// 'filesDo', 'streamTree', 'dumpTree', 'printOn', 'dumpToDoc']

this.assert(PathName(defaultLibFolder).pathMatch.size > 0,
"pathMatch finds defaultLibrary");

		this.assert(PathName(defaultLibFolder).entries.size > 0, "DefaultLibrary has entries");
		this.assert(PathName(defaultLibFolder).entries[0].isKindOf(PathName),
			"DefaultLibrary entries are PathNames");

		this.assert(PathName(defaultLibFolder).files.size > 0, "DefaultLibrary has files");
		this.assert(PathName(classLibFolder).folders.size > 0, "SCClassLibrary has folders");
		this.assert(
			PathName(classLibFolder).deepFiles.size >
			PathName(classLibFolder).entries.size,
			"deepFiles: SCClassLibrary has more deepFiles than entries"
		);

		PathName(classLibFolder).filesDo {|path, i| fileCount = fileCount + 1 };
		this.assertEquals(fileCount, PathName(classLibFolder).deepFiles.size,
			"filesDo and deepFiles find the same number of files.");
	}

	test_InfoPosting {
		var c = CollStream.new;
		// PathName(Main.filenameSymbol.asString.dirname).streamTree(c);
		// this.assert(c.collection.contains("Main"), "streamTree");
	}
}
