
PathName {

	var <fullPath, colonIndices;
	
	classvar <>scroot;
	classvar <>secondVolume; //needed only for OS9 path conversions
	classvar <>tmp;
	
	*new { arg path = ""; 
		^super.new.init(path.standardizePath); 
	}
	*fromOS9 { arg path="";
		if(secondVolume.notNil and: 
			{ path.copyRange(0,secondVolume.size - 1) == secondVolume },{
				path = "/Volumes/" ++ path;
		});
		^super.new.init(
			String.streamContents({ arg s;
				path.do({ arg char,i;
					if(char == $:,{
						if(i != 0,{ // leading : is not wanted in unix
							s << $/
						})
					},{
						s <<  char
					});
				})
			})
		)
	}
	*initClass {	
		scroot = File.getcwd;
		tmp = ["/tmp/", scroot ++ "\\sounds\\", "sounds/"].detect({ |path|
			File.exists(path);
		});
		tmp.isNil.if(
			{"No valid temp directory found. Please set this manually using PathName.tmp_".warn});
	}
	init { arg inPath;			
		fullPath = inPath;	
	}
	colonIndices {
		^colonIndices ?? {
			colonIndices = List.new;	
		 	fullPath.do({ arg eachChar, i; 
				if (eachChar.isPathSeparator, { colonIndices.add(i) }); 
			});
			colonIndices
		}
	}
	fileName { 
		^fullPath.copyRange((this.lastColonIndex) + 1, (fullPath.size -1).max(0));
	} 
	
	fileNameWithoutExtension {
		var fileName;
		fileName = this.fileName;
		fileName.reverseDo({ arg char,i;
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
		^pathName.fileNameWithoutExtension;
	}

	extension {
		var fileName;
		fileName = this.fileName;
		fileName.reverseDo({ arg char,i;
			if(char == $.,{
				^fileName.copyRange(fileName.size - i,fileName.size - 1)
			})
		});
		^""	
	}
	pathOnly { 		
		^fullPath.copyRange(0, this.lastColonIndex);
	}
	
	diskName {  ^fullPath.copyRange(0, this.colonIndices.first - 1) }
	
	isRelativePath { ^this.isAbsolutePath.not }
	
	isAbsolutePath { 
		^fullPath.at(0).isPathSeparator
	}
	
	asRelativePath { |relativeTo|
		var r, a, b, i, mePath;
		mePath = this.fullPath.absolutePath;
		relativeTo = (relativeTo ? scroot ).absolutePath;
		r = thisProcess.platform.pathSeparator;
		
		a = mePath.split(r);
		b = relativeTo.split(r);
		
		i=0;
		while{a[i]==b[i] and:{i<a.size}}{
		        i = i + 1;
		};
		^(".."++r).dup(b.size-i).join ++ a[i..].join(r)
	}
	asAbsolutePath {
		if(this.isAbsolutePath,{
			^fullPath
		},{
			// this assumes relative to the sc app
			// deprecated b/c String:absolutePath uses File.getcwd, more robust
//			^scroot ++ "/" ++ fullPath;
			^fullPath.absolutePath
		})
	}
	allFolders { 	
		var folderNames, pathCopy; 
		folderNames = List.new; 
		pathCopy = fullPath;

		this.colonIndices.doAdjacentPairs({ arg startColon, endColon; 
			folderNames.add( fullPath.copyRange(startColon + 1, endColon - 1) );
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

		^fullPath.copyRange(indexBeforeFolder, this.lastColonIndex - 1);
	}	

	lastColonIndex {
		var ci;
		ci = this.colonIndices;
		^if (ci.isEmpty, { -1 }, { ci.last }) ;
	} 
	
	nextName { 
		var nextName;
		nextName = if (fullPath.last.isDecDigit, 
			{ this.noEndNumbers ++ (this.endNumber + 1).asString;
			},
			{fullPath ++ "1"; }
		);
		^nextName; 
	}
	
	noEndNumbers { 
		var result, count = 0, char; 
		
		result = fullPath.copy;
		while(
			{ 	count = count + 1;
				char = fullPath.at(fullPath.size - count); 
				char.notNil and: { char.isDecDigit};
			}, 
			{ result = result.copyRange(0,  result.size - 2) }
			);
		^result;
	}

	endNumber {	// turn consecutive digits at the end of fullPath into a number.
	
		var reverseNumString = "";
		var count = 0, char, number;

		while(
			{ 	count = count + 1;
				char = fullPath.at(fullPath.size - count);
				char.notNil and: { char.isDecDigit};
			}, 
			{ reverseNumString = reverseNumString ++ char }
			);

			// convert reverseNumString back to number (digits times powers of 10)
		number = reverseNumString.inject(0, 
			{ arg sum, eachChar, i; sum = sum + (eachChar.digit * (10 ** i))
			}
		);
		^number
	}
	
	/* concatenation */
	+/+ { | path |
		var otherFullPath = path.respondsTo(\fullPath).if({ path.fullPath }, { path.asString });
		^this.class.new(fullPath +/+ otherFullPath)
	}

	/* additional methods jrh */
	
	entries {
		var path;
		path = fullPath;
		if(path.last.isPathSeparator.not, { path = path ++ thisProcess.platform.pathSeparator });
		^pathMatch(path ++ "*").collect({ arg item; PathName(item) });
	}
	
	pathMatch {
		^pathMatch(fullPath)
	}
	
	isFolder {
		var path;
		path = this.pathMatch;
		^if(path.notEmpty, {
			path.at(0).last.isPathSeparator
		}, { false });
	}
	isFile {
		var path;
		path = this.pathMatch;
		^if(path.notEmpty, {
			path.at(0).last.isPathSeparator.not
		}, { false });
	}
	
	files {
		^this.entries.select({ arg item; item.isFile })
	}
	folders {
		^this.entries.select({ arg item; item.isFolder })
	}
	deepFiles {
		^this.entries.collect({ arg item;
			if(item.isFile,{
				item
			},{
				item.deepFiles
			})
		}).flat
	}
	
	parentPath {
		var ci = this.colonIndices;
	
		^if((fullPath.last.isPathSeparator) && (ci.size > 1), {
			fullPath.copyRange(0, ci[ci.size - 2]);
		}, {
			fullPath.copyRange(0, this.lastColonIndex)
		});
	}

	foldersWithoutCVS { arg path;
		^this.folders(path).reject({ arg item; item.isCVS })
	}
	isCVS {
		^this.fileName == "CVS";
	}
	foldersWithoutSVN { arg path;
		^this.folders(path).reject({ arg item; item.isSVN })
	}
	isSVN {
		^this.fileName == ".svn";
	}
	filesDo { arg func;
		this.files.do(func);
		this.foldersWithoutSVN.do { arg pathname;
			pathname.filesDo(func)
		}
	}
	filesDoNoCVS { arg func;
		this.files.do(func);
		this.foldersWithoutCVS.do { arg pathname;
			pathname.filesDo(func)
		}
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
	streamTree { arg str, tabs=0;
		str << this.fullPath << Char.nl;
		this.files.do({ arg item; 
			tabs.do({ str << Char.tab });
			str << item.fileNameWithoutExtension  << Char.nl
		});
		this.foldersWithoutSVN.do({ arg item; 
			item.streamTree(str, tabs + 1);
		});
	}
	streamTreeNoCVS { arg str, tabs=0;
		str << this.fullPath << Char.nl;
		this.files.do({ arg item; 
			tabs.do({ str << Char.tab });
			str << item.fileNameWithoutExtension  << Char.nl
		});
		this.foldersWithoutCVS.do({ arg item; 
			item.streamTree(str, tabs + 1);
		});
	}
	
	dumpTree {
		this.streamTree(Post)
	}
		
	printOn { arg stream;
		stream << "PathName(" << fullPath <<")"
	}
	
	dumpToDoc { arg title="Untitled";
		var str, doc;
		doc = Document.new(title);
		str = CollStream.new;
		this.streamTree(str);
		doc.string = str.collection;
		^doc
	}

}
