/*
Path is an update to the PathName class, modeled on Python Path/pathlib.
It collects all path-related methods in one consistent interface.
For backwards compatibility, PathName is kept as a subclass with its
old interface; PathName and its suboptimal methods will be deprecated.

Path will also use some path-related functionality of String and File.
Because all path-accessing primitives expect a String as input,
calls to them will convert Path (and PathName) objects to Strings
with .asPath.
*/

Path {
	var <str;

	// should these be settable?
	classvar <>scroot;
	classvar <>tmp;

	*new { | path = "" |
		^super.newCopyArgs(path)
	}

	*home { ^Platform.userHomeDir }
	*cwd { ^File.getcwd }

	*initClass {
		scroot = File.getcwd;
		tmp = Platform.defaultTempDir;
		if(tmp.isNil, {
			"Path: No valid temp directory found. Please set it manually using Path.tmp_".warn
		})
	}

	// call to convert to string
	asPath { ^str.standardizePath }

	fileName { ^str.fileName }
	fileNameWithoutExtension { ^str.fileNameWithoutExtension }
	fileNameWithoutDoubleExtension { ^str.fileNameWithoutDoubleExtension }
	extension { ^str.extension }
	// needed for isFile, isFolder distinction
	dirname { ^str.dirname 	+/+ Platform.pathSeparator }
	parentPath { ^this.dirname }
	// need to keep this one?
	folderName { ^str.dirname.basename }

	// nested dirs,not starting slash
	parts { ^str.split(Platform.pathSeparator).select(_.notEmpty) }

	// conversions
	absolutePath { ^str.absolutePath }
	relativeTo { |path2| ^str.asRelativePath(path2) }
	withName { |name| ^this.class.new(str.dirname +/+ name) }

	// tests
	isAbsolute { ^str.isAbsolutePath }
	isRelative { ^str.isAbsolutePath.not }
	exists { ^File.exists(str) }
	== { |path2| ^str == path2.str }

	//// we need trailing slash for that
	isFolder { ^str.last == Platform.pathSeparator }
	isDir { ^str.last == Platform.pathSeparator }
	isFile { ^str.last != Platform.pathSeparator }

	// not sure these are needed?
	separatorIndices { ^str.separatorIndices }
	lastSeparatorIndex { ^str.lastSeparatorIndex }

	//
	nextName { ^str.nextName }
	noEndNumbers { ^str.noEndNumbers }
	endNumber { ^str.endNumber }
	endNumberIndex { ^str.endNumberIndex }

	/* concatenation */
	+/+ { | path |
		var otherstr = path.respondsTo(\str).if({ path.str }, { path.asString });
		^this.class.new(str +/+ otherstr)
	}
	// search
	match { ^str.pathMatch }
	entries { ^str.entries }
	files { ^str.files }
	folders { ^str.folders }
	deepFiles { ^str.deepFiles }

	filesDo { |func| ^str.filesDo(func) }

	// text output
	streamTree { | str, tabs = 0 | str.streamTree(str, tabs) }
	dumpTree { str.dumpTree }
	printOn { | stream | this.storeOn(stream) }
	storeArgs { ^[str] }
	dumpToDoc { | title="Untitled" | str.dumpToDoc(title) }

}

PathName : Path {
	// these methods are only for backwards compatibility,
	// to be deprecated at some point.
	// old instvar name
	*new { | path = "" |
		^super.newCopyArgs(path.standardizePath)
	}

	fullPath { ^str }

	// old methods, colon was Mac OS9 path separator...
	colonIndices { ^str.separatorIndices }
	lastColonIndex { ^str.lastSeparatorIndex }

	isFolder { ^str.last.isPathSeparator }

	isFile {
		var path = str.pathMatch;
		^if(path.notEmpty, {
			path.at(0).last.isPathSeparator.not
		}, { false })
	}

	// PathName:pathOnly ended with separator
	pathOnly {
		^str.dirname +/+ Platform.pathSeparator
	}
	// not sure this is useful in any way, but backwards compat.
	diskName {
		^str.copyRange(0, this.separatorIndices.first - 1)
	}

	allFolders { ^List.newFrom(this.parts.drop(-1)) }

	folderName { ^str.folderName }

	== { |other| { ^str.absolutePath }
		^other.respondsTo(\str) and: { other.str == this.str }
	}

	isRelativePath { ^this.isRelative }
	isAbsolutePath { ^this.isAbsolute }
	asRelativePath { |path2| ^str.asRelativePath(path2) }

	parentPath { ^str.parentPath }

}
