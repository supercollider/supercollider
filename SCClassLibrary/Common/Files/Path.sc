/*
Path.class.methods.printAll;
Path.methods.printAll.size;
 "";
*/

Path {
	var <str;

	// should these be settable?
	classvar <>scroot;
	classvar <>tmp;

	*new { | str = "" |
		^super.newCopyArgs(str)
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
	parent { ^this.class.new(str.dirname) }
	// nested dirs as strings, drop initial empty string
	parts { ^str.split(Platform.pathSeparator).select(_.notEmpty) }

	// from PathName
	fileName { ^str.basename }
	fileNameWithoutExtension { ^str.basename.splitext[0] }
	fileNameWithoutDoubleExtension { ^str.basename.splitext[0].splitext[0] }
	extension { ^str.splitext.last ? "" }

	// needed for PathName isFile, isFolder distinction
	dirname { ^str.dirname 	+/+ Platform.pathSeparator }
	// also from PathName - need to keep this one here?
	folderName { ^str.dirname.basename }

	fullPath { ^str } // compat

	// from File
	type { File.type(str) }

	// conversions
	absolutePath { ^this.class.new(str.absolutePath) }
	relativeTo { |path2| ^str.asRelativePath(path2) }
	withName { |name| ^this.class.new(str.dirname +/+ name) }

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

	// replace PathName:colonIndices
	separatorIndices {
		var res = List[];
		str.do { |ch, i| if (ch.isPathSeparator) { res = res.add(i) } };
		^res
	}

	// replace PathName:lastColonIndex
	lastSeparatorIndex { ^this.separatorIndices.last ? -1 }

	nextName { |indexOfNumber, numExtToRemove=2|
		var parent = str.dirname, basefile = str.basename;
		var filename, extensions, nameParts, digitIndices;
		var partIndex, endNumber = 0, nextFileName, nextFullName;
		filename = basefile;
		numExtToRemove.do { filename = filename.splitext[0] };
		extensions = basefile.drop(filename.size);

		nameParts = filename.separate { |a, b| a.isDecDigit != b.isDecDigit };
		digitIndices = nameParts.selectIndices { |part| part[0].isDecDigit };

		if (digitIndices.notEmpty) {
			indexOfNumber = (indexOfNumber ? digitIndices.lastIndex).clip(0, digitIndices.lastIndex);
			partIndex = digitIndices.at(indexOfNumber);
			endNumber = nameParts.at(partIndex).asInteger;
			nextFileName = nameParts.put(partIndex, (endNumber + 1).asString).join("");
		} {
			// no endNumber:
			nextFileName = filename ++ 1;
		};
		nextFullName = parent +/+ nextFileName ++ extensions;
		if (nextFullName.beginsWith("./")) { nextFullName = nextFullName.drop(2) };

		^nextFullName
	}

	/* concatenation */
	+/+ { | path |
		var otherstr = path.respondsTo(\str).if({ path.str }, { path.asString });
		^this.class.new(str +/+ otherstr)
	}

	// search
	match { ^str.pathMatch }
	pathMatch { ^str.pathMatch } // compatibility

	entries { ^pathMatch(str +/+ "*").collect { |str| this.class.new(str) } }
	files { ^this.entries.select({ | item |item.isFile }) }
	folders { ^this.entries.select({ | item | item.isFolder }) }

	deepFiles {
		^this.entries.collect({ | item |
			if(item.isFile, {
				item
			},{
				item.deepFiles
			})
		}).flatIf { |item| item.isKindOf(String).not }
	}

	filesDo { | func |
		this.files.do(func);
		this.folders.do { | path |
			path.filesDo(func)
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
