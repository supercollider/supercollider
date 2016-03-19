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
		tmp.isNil.if(
			{"No valid temp directory found. Please set this manually using PathName.tmp_".warn});
	}

	colonIndices {
		^colonIndices ?? {
			colonIndices = List.new;
		 	fullPath.do({ | eachChar, i |
				if (eachChar.isPathSeparator, { colonIndices.add(i) });
			});
			colonIndices
		}
	}

	fileName {
		^fullPath.copyRange((this.lastColonIndex) + 1, (fullPath.size -1).max(0))
	}

	fileNameWithoutExtension {
		var fileName;
		fileName = this.fileName;
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

		i=0;
		while{ a[i] == b[i] and: { i < a.size } } {
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
		var indexBeforeFolder,ci;
		ci = this.colonIndices;
		if (ci.isEmpty, { ^"" });

		indexBeforeFolder =
		if (ci.size == 1, 0,
			{ ci.at(ci.size - 2) + 1 });

		^fullPath.copyRange(indexBeforeFolder, this.lastColonIndex - 1)
	}

	lastColonIndex {
		var ci = this.colonIndices;
		^if(ci.isEmpty, { -1 }, { ci.last }) ;
	}

	nextName {
		^if(fullPath.last.isDecDigit, {
			this.noEndNumbers ++ (this.endNumber + 1).asString
		}, {
			fullPath ++ "1"
		})
	}

	noEndNumbers {
		var count = 0, char;
		var result = fullPath.copy;
		while({
			count = count + 1;
			char = fullPath.at(fullPath.size - count);
			char.notNil and: { char.isDecDigit};
		}, {
			result = result.copyRange(0,  result.size - 2)
		});
		^result
	}

	endNumber {	// turn consecutive digits at the end of fullPath into a number.
		var reverseNumString = "";
		var count = 0, char;

		while({
			count = count + 1;
			char = fullPath.at(fullPath.size - count);
			char.notNil and: { char.isDecDigit};
		}, {
			reverseNumString = reverseNumString ++ char
		});

		// convert reverseNumString back to number (digits times powers of 10)
		^reverseNumString.inject(0,
			{ arg sum, eachChar, i; sum = sum + (eachChar.digit * (10 ** i)) }
		)
	}

	/* concatenation */
	+/+ { | path |
		var otherFullPath = path.respondsTo(\fullPath).if({ path.fullPath }, { path.asString });
		^this.class.new(fullPath +/+ otherFullPath)
	}

	entries {
		var path = fullPath;
		if(path.isEmpty) { ^[] };
		if(path.last.isPathSeparator.not, { path = path ++ thisProcess.platform.pathSeparator });
		^pathMatch(path ++ "*").collect({ | item | PathName(item) })
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

	// Iterates over all files within this path which match criteria for being help files.
	// Doesn't iterate over the help files listed in the Help tree - see Help:do for that.
	helpFilesDo { arg func;
		var extensions    = #['html', 'htm', 'scd', 'rtf', 'rtfd']; // included
		var ignoreFolders = #['ignore', '.svn', '_darcs', 'CVS', '.git']; // excluded
		this.files.select{|afile| extensions.includes(afile.extension.asSymbol) }
			.do(func);
		this.folders.reject{|afolder| ignoreFolders.includes(afolder.folderName.asSymbol) }
			.do{ arg pathname;
				pathname.helpFilesDo(func)
			}
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

}
