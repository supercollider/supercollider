/*
// Path, the offical new interface class for paths:
TestPath.run;
*/

TestPath : UnitTest {

	test_classMethods {
		this.assertEquals(Path.scroot, File.getcwd, "scroot");
		this.assertEquals(Path.tmp, Platform.defaultTempDir, "tmp");
		this.assertEquals(Path.home, Platform.userHomeDir, "home");
		this.assertEquals(Path.cwd, File.getcwd, "cwd");
	}

	// all string manipulation methods
	test_accessPathParts {

		var p = Path("/Users/xyz/src/pathTest.abc.scd");

		// path parts:
		this.assertEquals(p.separatorIndices, List[0, 6, 10, 14], "separatorIndices");
		this.assertEquals(p.name, "pathTest.abc.scd", "name");
		this.assertEquals(p.parent, Path("/Users/xyz/src"), "parent");
		this.assertEquals(p.parts, ["Users", "xyz", "src", "pathTest.abc.scd"], "parts");
		this.assertEquals(p.fileName, "pathTest.abc.scd", "fileName");

		this.assertEquals(p.fileName, "pathTest.abc.scd", "fileName");
		this.assertEquals(p.fileNameWithoutExtension, "pathTest.abc", "withoutExtension");
		this.assertEquals(p.fileNameWithoutDoubleExtension, "pathTest", "withoutDoubleExtension");
		this.assertEquals(p.extension, "scd", "extension");

		this.assertEquals(p.dirname, "/Users/xyz/src/", "folderName");
		this.assertEquals(p.folderName, "src", "folderName");

		this.assertEquals(p.fullPath, "/Users/xyz/src/pathTest.abc.scd", "folderName");
	}

	// tests
	test_tests {
		var p = Path("/Users/xyz/src/pathTest.abc.scd");

		this.assert(p.isAbsolute, "isAbsolute");
		this.assert(Path("relative.scd").isRelative, "isRelative");
		this.assert(Path(TestPath.filenameSymbol.asString).exists, "file exists");

		/////  how to test these? isFolder checked for trialing slash. cant tell without it.
		this.assert(Path(p.dirname).isFolder, "isFolder");
		// this.assert(p.dirname.isDir, "isDir");
		this.assert(p.isFile, "isFile");
	}

	test_conversions {
		var p = Path("/Users/xyz/src/pathTest.abc.scd");
		// conversions
		this.assertEquals(p.relativeTo("/Users/xyz/"), Path("src/pathTest.abc.scd"), "relativeTo");
		this.assertEquals(p.withName("newName.txt"), Path("/Users/xyz/src/newName.txt"), "withName");

		this.assertEquals(Path("abc").nextName, "abc1", "nextName, no number, no extension");
		this.assertEquals(Path("abc9").nextName, "abc10", "nextName with number, no extension");
		this.assertEquals(
			Path("/where/ever/abc99.txt").nextName,
			"/where/ever/abc100.txt",
			"nextName with extension, long path and high number"
		);

		this.assertEquals(Path("abc/") +/+ "def", Path("abc/def"), "concat with String");
		this.assertEquals(Path("abc/") +/+ Path("/def"), Path("abc/def"),
			"concat with 2 Paths");
	}

	test_fileAndFolderAccessMethods {
		var defaultLibFolder = Main.filenameSymbol.asString.dirname;
		var classLibFolder = Platform.classLibraryDir;
		var fileCount = 0, postStream;
		//  hierarchical access to files and folders
		// [ 'isFolder', 'isFile', 'entries', 'files', 'folders', 'deepFiles',
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

		Path(classLibFolder).filesDo {|path, i| fileCount = fileCount + 1 };
		this.assertEquals(fileCount, Path(classLibFolder).deepFiles.size,
			"filesDo and deepFiles find the same number of files.");
	}
}
