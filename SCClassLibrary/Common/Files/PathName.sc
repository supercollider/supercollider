PathName {

	var <fullPath, colonIndices;

	classvar <>scroot;
	classvar <>tmp;

	*new { | path = "" |
		^super.newCopyArgs(path.standardizePath)
	}

	*initClass {
		scroot = File.getcwd;
		tmp = Platform.defaultTempDir;
		if(tmp.isNil, {
			"No valid temp directory found. Please set it manually using PathName.tmp_".warn
		})
	}

	colonIndices {
		^colonIndices ?? {
			colonIndices = List.new;
			fullPath.do({ | eachChar, i |
				if(eachChar.isPathSeparator, { colonIndices.add(i) })
			});
			colonIndices
		}
	}

	fileName {
		^fullPath.copyRange((this.lastColonIndex) + 1, (fullPath.size -1).max(0))
	}

	fileNameWithoutExtension {
		var fileName = this.fileName;
		fileName.reverseDo({ | char, i |
			if(char == $.,{
				^fileName.copyRange(0,fileName.size - (i + 2))
			})
		});
		^fileName
	}

	fileNameWithoutDoubleExtension {
		var fileName, pathName;
		fileName = this.fileNameWithoutExtension;
		pathName = PathName( fileName );
		^pathName.fileNameWithoutExtension
	}

	extension {
		var fileName;
		fileName = this.fileName;
		fileName.reverseDo({ | char, i |
			if(char == $., {
				^fileName.copyRange(fileName.size - i,fileName.size - 1)
			})
		});
		^""
	}

	pathOnly {
		^fullPath.copyRange(0, this.lastColonIndex)
	}

	diskName {
		^fullPath.copyRange(0, this.colonIndices.first - 1)
	}

	isRelativePath { ^this.isAbsolutePath.not }

	isAbsolutePath {
		^fullPath.at(0).isPathSeparator
	}

	absolutePath{
		^fullPath.absolutePath
	}

	asRelativePath { |relativeTo|
		var r, a, b, i, mePath;
		mePath = this.fullPath.absolutePath;
		relativeTo = (relativeTo ? scroot ).absolutePath;
		r = thisProcess.platform.pathSeparator;

		a = mePath.split(r);
		b = relativeTo.split(r);

		i = 0;
		while { a[i] == b[i] and: { i < a.size } } {
			i = i + 1;
		};
		^(".." ++ r).dup(b.size - i).join ++ a[i..].join(r)
	}

	asAbsolutePath {
		if(this.isAbsolutePath,{
			^fullPath
		},{
			// this assumes relative to the sc app
			^fullPath.absolutePath
		})
	}

	allFolders {
		var folderNames, pathCopy;
		folderNames = List.new;
		pathCopy = fullPath;

		this.colonIndices.doAdjacentPairs({ | startColon, endColon |
			folderNames.add( fullPath.copyRange(startColon + 1, endColon - 1) )
		});

		^folderNames
	}

	folderName {
		var indexBeforeFolder;
		var ci = this.colonIndices;
		if (ci.isEmpty, { ^"" });

		indexBeforeFolder =
		if (ci.size == 1, 0, { ci.at(ci.size - 2) + 1 });

		^fullPath.copyRange(indexBeforeFolder, this.lastColonIndex - 1)
	}

	lastColonIndex {
		var ci = this.colonIndices;
		^if(ci.isEmpty, { -1 }, { ci.last }) ;
	}

	nextName {
		^if(fullPath.last.isDecDigit, {
			this.noEndNumbers ++ (this.endNumber + 1)
		}, {
			fullPath ++ "1"
		})
	}

	noEndNumbers {
		^fullPath[..this.endNumberIndex]
	}

	endNumber {	// turn consecutive digits at the end of fullPath into a number.
		^fullPath[this.endNumberIndex + 1..].asInteger
	}

	endNumberIndex {
		var index = fullPath.lastIndex;
		while({
			index > 0 and: { fullPath.at(index).isDecDigit }
		}, {
			index = index - 1
		});
		^index
	}

	/* concatenation */
	+/+ { | path |
		var otherFullPath = path.respondsTo(\fullPath).if({ path.fullPath }, { path.asString });
		^this.class.new(fullPath +/+ otherFullPath)
	}

	entries {
		var path = fullPath;
		if(path.isEmpty) { ^[] };
		^pathMatch(path +/+ "*").collect({ | item | PathName(item) })
	}

	pathMatch {
		^pathMatch(fullPath)
	}

	isFolder {
		var path = this.pathMatch;
		^if(path.notEmpty, {
			path.at(0).last.isPathSeparator
		}, { false })
	}

	isFile {
		var path = this.pathMatch;
		^if(path.notEmpty, {
			path.at(0).last.isPathSeparator.not
		}, { false })
	}

	files {
		^this.entries.select({ | item | item.isFile })
	}

	folders {
		^this.entries.select({ | item | item.isFolder })
	}

	deepFiles {
		^this.entries.collect({ | item |
			if(item.isFile, {
				item
			},{
				item.deepFiles
			})
		}).flat
	}

	parentPath {
		var ci = this.colonIndices;

		^if((fullPath.last.isPathSeparator) and: { ci.size > 1 }, {
			fullPath.copyRange(0, ci[ci.size - 2])
		}, {
			fullPath.copyRange(0, this.lastColonIndex)
		})
	}

	filesDo { | func |
		this.files.do(func);
		this.folders.do { | pathname |
			pathname.filesDo(func)
		};
	}

	streamTree { | str, tabs = 0 |
		str << this.fullPath << Char.nl;
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

	printOn { | stream |
		stream << "PathName(" << fullPath << ")"
	}

	dumpToDoc { | title="Untitled" |
		var str, doc;
		doc = Document.new(title);
		str = CollStream.new;
		this.streamTree(str);
		doc.string = str.collection;
		^doc
	}

	// deprecated messages

	*fromOS9 { | path | ^this.deprecated(thisMethod) }
	foldersWithoutCVS { | path | ^this.deprecated(thisMethod) }
	isCVS { ^this.deprecated(thisMethod) }
	foldersWithoutSVN { | path |^this.deprecated(thisMethod) }
	isSVN { ^this.deprecated(thisMethod) }
	filesDoNoCVS { | func | ^this.deprecated(thisMethod) }
	filesDoNoSVN { | func | ^this.deprecated(thisMethod) }
	streamTreeNoCVS { | str, tabs=0 | ^this.deprecated(thisMethod) }
	helpFilesDo { |func| ^this.deprecated(thisMethod) }
}
