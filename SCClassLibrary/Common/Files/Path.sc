/*
Path is an update/replacement for the PathName class, modeled on Python Path/pathlib.
It collects all path-related methods in one consistent interface.
For backwards compatibility, PathName is kept as a subclass with its
old interface; PathName and its suboptimal methods may be deprecated later.

Path may later also add path-related functionality from String and File.
All path-accessing primitives expect a String as input,
so methods calling them should call obj.asPathString(path);
this converts Path objects to Strings, and leaves Strings untouched.

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

	// access path parts - recommended new:
	name { ^str.basename }
	parent { ^Path(str.dirname) }
	// nested dirs as strings, drop initial empty string
	parts { ^str.split(Platform.pathSeparator).select(_.notEmpty) }

	// from PathName
	fileName { ^str.basename }
	fileNameWithoutExtension { ^str.basename.splitext[0] }
	fileNameWithoutDoubleExtension { ^str.basename.splitext[0].splitext[0] }
	extension { ^str.splitext.last ? "" }
	// from File
	type { File.type(str) }

	// needed for PathName isFile, isFolder distinction
	dirname { ^str.dirname 	+/+ Platform.pathSeparator }
	// also from PathName - need to keep this one here?
	folderName { ^str.dirname.basename }

	fullPath { ^str } // compat

	// conversions
	absolutePath { ^this.class.new(str.absolutePath) }
	relativeTo { |path2| ^Path(str.asRelativePath(path2)) }
	withName { |name| ^Path(str.dirname +/+ name) }
	// call this to convert to string before read or write primitive
	asPathString { ^str.standardizePath }
	// call for OSC communication
	asOSCArgArray { ^str.standardizePath }

	// match with String.asPath
	asPath { ^this }

	// tests
	isAbsolute {
		^Platform.case(\windows) {
			"Windows".postln;
			str.drop(1).beginsWith(":\\") or: { "\\\\".beginsWith("\\\\") }
		} {
			// unix
			str.at(0).isPathSeparator
		}
	}
	isRelative { ^this.isAbsolute.not }
	exists { ^File.exists(str) }
	== { |path2| ^str == path2.str }

	//// we need trailing slash for these
	isFolder { ^str.last == Platform.pathSeparator }
	isDir { ^str.last == Platform.pathSeparator }
	isFile { ^str.last != Platform.pathSeparator }

	// not sure these are needed?
	separatorIndices {
		var res = List[];
		str.do { |ch, i| if (ch.isPathSeparator) { res = res.add(i) } };
		^res
	}

	lastSeparatorIndex { ^this.separatorIndices.last ? -1 }

	// are these good enough to keep here?
	nextName {
		var name, ext;
		if (str.last.isDecDigit) {
			^this.noEndNumbers ++ (this.endNumber + 1)
		};
		#name, ext = str.splitext;
		if (ext.isNil) {
			^name ++ "1"
		};
		^Path(name).nextName ++ "." ++ ext
	}

	noEndNumbers {
		^str[..this.endNumberIndex]
	}

	endNumber {	// turn consecutive digits at the end of this into a number.
		^str[this.endNumberIndex + 1..].asInteger
	}

	endNumberIndex {
		var index = str.lastIndex;
		while({
			index > 0 and: { str.at(index).isDecDigit }
		}, {
			index = index - 1
		});
		^index
	}

	/* concatenation */
	+/+ { | path |
		var otherstr = path.respondsTo(\str).if({ path.str }, { path.asString });
		^this.class.new(str +/+ otherstr)
	}

	// search
	match { ^str.pathMatch }
	pathMatch { ^str.pathMatch } // compat
	entries { ^pathMatch(str +/+ "*").collect { |str| this.class.new(str) } }
	files { ^this.entries.select({ | item |item.isFile }) }
	folders { ^this.entries.select({ | item | item.isFolder }) }

	deepFiles { ^this.entries.collect({ | item |
			if(item.isFile, {
				item
			},{
			Path(item).deepFiles
			})
		}).flatIf { |item| item.isKindOf(String).not }
	}

	filesDo { | func |
		this.files.do(func);
		this.folders.do { | path |
			Path(path).filesDo(func)
		};
	}

	// text output
	streamTree { | str, tabs = 0 |
		str << this << Char.nl;
		this.files.do({ | item |
			tabs.do({ str << Char.tab });
			str << item.fileNameWithoutExtension  << Char.nl
		});
		this.folders.do({ | item |
			item.streamTree(str, tabs + 1);
		});
	}

	dumpTree {
		this.streamTree(Post)
	}

	storeArgs { ^[str] }
	printOn { | stream | this.storeOn(stream) }

	dumpToDoc { | title="Untitled" |
		var str, doc;
		doc = Document.new(title);
		str = CollStream.new;
		this.streamTree(str);
		doc.string = str.collection;
		^doc
	}

}