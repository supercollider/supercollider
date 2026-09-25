TestExampleFiles : UnitTest {
	test_filesExist {
		// we use the methods of the meta class to access the static methods
		Meta_ExampleFiles.methods.do({|staticMethod|
			var filePath = ExampleFiles.performArgs(staticMethod.name);
			this.assert(
				boolean: PathName(filePath).isFile,
				message: "% is an example file and should exist".format(filePath),
			);
		});
	}
}
