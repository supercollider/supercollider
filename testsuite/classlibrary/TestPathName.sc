/*
// Path, the offical new interface class for paths:
TestPath.run;
// PathName old interface for backwards compatibility:
TestPathName.run
*/

TestPath : UnitTest {

	// all string manipulation methods
	test_filePathMethods {

		var p = Path("/Users/adc/src/Sounds/FunkyChicken.abc.scd");

		// path parts:
		this.assertEquals(p.separatorIndices, List[0, 6, 10, 14, 21], "separatorIndices");
		this.assertEquals(p.fileName, "FunkyChicken.abc.scd", "fileName");
		this.assertEquals(p.fileNameWithoutExtension, "FunkyChicken.abc", "withoutExtension");
		this.assertEquals(p.fileNameWithoutDoubleExtension, "FunkyChicken", "withoutDoubleExtension");
		this.assertEquals(p.extension, "scd", "extension");
		this.assertEquals(p.folderName, "Sounds", "folderName");
		this.assertEquals(p.parts, ["Users", "adc", "src", "Sounds", "FunkyChicken.abc.scd"], "parts");

		// tests
		this.assert(p.isAbsolute, "isAbsolute");
		this.assert(Path("relative.scd").isRelative, "isRelative");
		this.assert(Path(TestPath.filenameSymbol.asString).exists, "file exists");
		/////  how to test these? isFolder checked for trialing slash. cant tell without it.
		this.assert(p.dirname.isFolder, "isFolder");
		// this.assert(p.dirname.isDir, "isDir");
		this.assert(p.isFile, "isFile");

		// conversions
		this.assertEquals(p.relativeTo("/Users/adc/src"), "Sounds/FunkyChicken.abc.scd", "relativeTo");
		this.assertEquals(p.withName("newName.txt"), Path("/Users/adc/src/Sounds/newName.txt"), "withName");

		this.assertEquals(PathName("abc").nextName, "abc1", "nextName, no number, no extension");
		this.assertEquals(PathName("abc9").nextName, "abc10", "nextName with number, no extension");
		this.assertEquals(
			PathName("/where/ever/abc99.txt").nextName,
			"/where/ever/abc100.txt",
			"nextName with extension, long path and high number"
		);

		this.assertEquals(PathName("abc/") +/+ "def", PathName("abc/def"), "concat with String");
		this.assertEquals(PathName("abc/") +/+ PathName("/def"), PathName("abc/def"),
		"concat with 2 PathNames");
	}

	test_fileAndFolderAccessMethods {
		var defaultLibFolder = Main.filenameSymbol.asString.dirname;
		var classLibFolder = Platform.classLibraryDir;
		var fileCount = 0, postStream;
		//  hierarchical access to files and folders
		// [ 'isFolder', 'isFile', 'entries', 'files', 'folders', 'deepFiles', 'parentPath',
		// 'filesDo', 'streamTree', 'dumpTree', 'printOn', 'dumpToDoc']

		// this.assert(Path("~/").isFolder, "isFolder: home should be a folder");
		// this.assert(Path("~/").isFile.not, "isFile: home should not be a file");
		this.assert(Path(defaultLibFolder).entries.size > 0, "DefaultLibrary has entries");
		this.assert(Path(defaultLibFolder).files.size > 0, "DefaultLibrary has files");
		this.assert(Path(classLibFolder).folders.size > 0, "SCClassLibrary has folders");
		this.assert(
			Path(classLibFolder).deepFiles.size > Path(classLibFolder).folders.size,
			"deepFiles: SCClassLibrary has more deepFiles than folders"
		);
		this.assertEquals(
			Path(defaultLibFolder).parentPath, (classLibFolder +/+ "/"),
			"parentPath is correct"
		);

		Path(classLibFolder).filesDo {|path, i| fileCount = fileCount + 1 };
		this.assertEquals(fileCount, Path(classLibFolder).deepFiles.size,
			"filesDo and deepFiles find the same number of files.");
	}


}

/*
// full coverage of former PathName.methods:
PathName.methods.collect(_.name).postcs.removeAll(String.methods.collect(_.name));

Tests for all PathName methods by group:

// simple filename/path-related
'colonIndices', 'separatorIndices', // synonom for back/comp
'fileName', 'fileNameWithoutExtension', 'fileNameWithoutDoubleExtension', 'extension',
'dirname', 'pathOnly', // synonym

'diskName', // useful at all? seems dated

'isRelativePath', 'isAbsolutePath', 'absolutePath',
'asRelativePath', 'asAbsolutePath', 'allFolders', 'folderName', 'lastColonIndex',

// should work as in old style without extensions, and new with extensions!
'nextName', 'noEndNumbers', 'endNumber', 'endNumberIndex',

// hierarchical folder access
'entries', 'pathMatch', 'isFolder', 'isFile', 'files', 'folders', 'deepFiles', 'parentPath', 'filesDo', 'streamTree', 'dumpTree', 'printOn', 'dumpToDoc'

bench { TestPathName.run };

*/
TestPathName : UnitTest {

	// all string manipulation methods
	test_filePathMethods {

		var p = PathName("/Users/adc/src/Sounds/FunkyChicken.abc.scd");

		// keep List type for full compat
		this.assertEquals(p.colonIndices, List[0, 6, 10, 14, 21], "colonIndices");
		// new, better name
		this.assertEquals(p.separatorIndices, List[0, 6, 10, 14, 21], "separatorIndices");

		this.assertEquals(p.fileName, "FunkyChicken.abc.scd", "fileName");
		this.assertEquals(p.fileNameWithoutExtension, "FunkyChicken.abc", "fileNameWithoutExtension");
		this.assertEquals(p.fileNameWithoutDoubleExtension, "FunkyChicken", "fileNameWithoutDoubleExtension");
		this.assertEquals(p.extension, "scd", "extension");

		// pathOnly ends with a training slash
		this.assertEquals(p.pathOnly, "/Users/adc/src/Sounds/", "pathOnly");

		// diskName expects no separator at beginning ...
		this.assertEquals(PathName("ABC/def/ghj.scd").diskName, "ABC", "diskName");

		this.assert(p.isAbsolutePath, "isAbsolutePath"); // true, because ~/ expanded correctly
		this.assert(PathName("relative.scd").isRelativePath, "isRelativePath");
		this.assertEquals(p.asRelativePath("/Users/adc/src"), "Sounds/FunkyChicken.abc.scd", "asRelativePath");

		this.assertEquals(p.allFolders, List["Users", "adc", "src", "Sounds"], "allFolders");

		this.assertEquals(p.folderName, "Sounds", "folderName");

		this.assertEquals(p.lastColonIndex, 21, "lastColonIndex when it exists");
		this.assertEquals(PathName("123").lastColonIndex, -1, "lastColonIndex when not found");


		this.assertEquals(PathName("abc").nextName, "abc1", "nextName, no number, no extension");
		this.assertEquals(PathName("abc9").nextName, "abc10", "nextName with number, no extension");
		this.assertEquals(
			PathName("/where/ever/abc99.txt").nextName,
			"/where/ever/abc100.txt",
			"nextName with extension, long path and high number"
		);

		this.assertEquals(PathName("abc/") +/+ "def", PathName("abc/def"), "concat with String");
		this.assertEquals(PathName("abc/") +/+ PathName("/def"), PathName("abc/def"),
			"concat with 2 PathNames");

	}

	test_fileAndFolderAccessMethods {
		var defaultLibFolder = Main.filenameSymbol.asString.dirname;
		var classLibFolder = Platform.classLibraryDir;
		var fileCount = 0, postStream;
		//  hierarchical access to files and folders
		// [ 'isFolder', 'isFile', 'entries', 'files', 'folders', 'deepFiles', 'parentPath',
		// 'filesDo', 'streamTree', 'dumpTree', 'printOn', 'dumpToDoc']

		this.assert(PathName("~/").isFolder, "isFolder: home should be a folder");
		this.assert(PathName("~/").isFile.not, "isFile: home should not be a file");
		this.assert(PathName(defaultLibFolder).entries.size > 0, "DefaultLibrary has entries");
		this.assert(PathName(defaultLibFolder).files.size > 0, "DefaultLibrary has files");
		this.assert(PathName(classLibFolder).folders.size > 0, "SCClassLibrary has folders");
		this.assert(
			PathName(classLibFolder).deepFiles.size > PathName(classLibFolder).folders.size,
			"deepFiles: SCClassLibrary has more deepFiles than folders"
		);
		this.assertEquals(
			PathName(defaultLibFolder).parentPath, (classLibFolder +/+ "/"),
			"parentPath is correct"
		);

		PathName(classLibFolder).filesDo {|path, i| fileCount = fileCount + 1 };
		this.assertEquals(fileCount, PathName(classLibFolder).deepFiles.size,
		"filesDo and deepFiles find the same number of files.");
	}
}
