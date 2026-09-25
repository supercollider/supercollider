TestPathName : UnitTest {
	test_isAbsolutePath_Windows {
		if (thisProcess.platform.name == \windows) {
			// absolute paths
			this.assert(PathName("C:/test").isAbsolutePath, "absolute path <drive>:/");
			this.assert(PathName("C:\\test").isAbsolutePath, "absolute path <drive>:\\");
			this.assert(PathName("\\\\system07\\test").isAbsolutePath, "absolute path UNC");
			// only because PathName calls standardizePath
			this.assert(PathName("~/test").isAbsolutePath, "path starting with ~ is treated as absolute");

			// relative paths
			this.assert(PathName("test").isAbsolutePath.not, "relative path");
			this.assert(PathName("./test").isAbsolutePath.not, "relative path with leading ./");
			this.assert(PathName(".\\test").isAbsolutePath.not, "relative path with leading .\\");
			this.assert(PathName("/test").isAbsolutePath.not, "path starting with / is relative (to the current drive)");
			this.assert(PathName("\\test").isAbsolutePath.not, "path starting with \\ is relative (to the current drive)");
		}
	}
}
