
PathName : Path {
	// these methods are only for backwards compatibility,
	// to be deprecated at some point.

	*new { | path = "" |
		^super.newCopyArgs(path.standardizePath)
	}
	// old instvar name
	fullPath { ^str }

	// colon was Mac OS9 path separator...
	colonIndices { ^this.separatorIndices }
	lastColonIndex { ^this.lastSeparatorIndex }

	isFolder { ^str.last.isPathSeparator }

	// PathName:pathOnly ends with separator
	pathOnly {
		^str.dirname +/+ Platform.pathSeparator
	}
	// not sure this is useful, but backwards compat
	diskName {
		^str.copyRange(0, this.separatorIndices.first - 1)
	}

	allFolders { ^List.newFrom(this.parts.drop(-1)) }

	== { |other| { ^str.absolutePath }
		^other.respondsTo(\str) and: { other.str == this.str }
	}

	isRelativePath { ^this.isRelative }
	isAbsolutePath { ^this.isAbsolute }
	asRelativePath { |path2| ^str.asRelativePath(path2) }
	// expects a string, not a PathName
	asAbsolutePath { ^str.absolutePath }
	absolutePath { ^str.absolutePath }

	// alias for PathName:dirname
	parentPath { ^this.dirname }

}
