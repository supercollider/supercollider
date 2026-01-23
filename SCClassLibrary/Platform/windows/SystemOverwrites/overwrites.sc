+ String {
	// See https://learn.microsoft.com/en-us/dotnet/standard/io/file-path-formats for reference.
	absolutePath {
		// First check if the path is already absolute.
		// There are 3 styles of absolute paths:
		// 1. C:\..., 2. C:/...  and 3. \\<machine>\... (= UNC path)
		if((this[0].isAlpha and: { this[1] == $: and: { this[2] == $\\ or: { this[2] == $/ } } })
				or: { this[0] == $\\ and: { this[1] == $\\ } }) {
			^this
		} {
			// \... and /... start from the current drive.
			// Let's use the drive where SC is installed.
			if(this[0] == $\\ or: { this[0] == $/ }) {
				^File.getcwd[..1] ++ this
			};
			// Resolve ~ to the user's home directory.
			if(this[0] == $~){^this.standardizePath};
			^File.getcwd +/+ this;
		}
	}
}

+ PathName {
	isAbsolutePath {
		// See String -absolutePath
		^(fullPath[0].isAlpha and: { fullPath[1] == $: and: { fullPath[2] == $\\ or: { fullPath[2] == $/ } } })
				or: { fullPath[0] == $\\ and: { fullPath[1] == $\\ } }
	}
}
