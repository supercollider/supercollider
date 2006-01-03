
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
		tmp = ["/tmp/", "/WINDOWS/TEMP/", "sounds/"].detect({ |path|
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
				if (eachChar == $/, { colonIndices.add(i) }); 
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
		^fullPath.at(0) == $/
	}
	
	asRelativePath {
		if(fullPath.size > scroot.size and: {fullPath.copyRange(0,scroot.size - 1) == scroot },{
			^fullPath.copyRange(scroot.size,fullPath.size - 1)
		},{
			^fullPath
		})
	}
	asAbsolutePath {
		if(this.isAbsolutePath,{
			^fullPath
		},{
			// this assumes relative to the sc app
			^scroot ++ "/" ++ fullPath;
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
		var result, count = 0; 
		
		result = fullPath.copy;
		while(
			{ 	count = count + 1;
				fullPath.at(fullPath.size - count).isDecDigit;
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
				char.isDecDigit;
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
	
	/* additional methods jrh */
	
	entries {
		var path;
		path = fullPath;
		if(path.last != $/, { path = path ++ "/" });
		^pathMatch(path ++ "*").collect({ arg item; PathName(item) });
	}
	
	pathMatch {
		^pathMatch(fullPath)
	}
	
	isFolder {
		var path;
		path = this.pathMatch;
		^if(path.notEmpty, {
			path.at(0).last == $/
		}, { false });
	}
	isFile {
		var path;
		path = this.pathMatch;
		^if(path.notEmpty, {
			path.at(0).last != $/
		}, { false });
	}
	
	files {
		^this.entries.select({ arg item; item.isFile })
	}
	folders {
		^this.entries.select({ arg item; item.isFolder })
	}
	foldersWithoutCVS { arg path;
		^this.folders(path).reject({ arg item; item.isCVS })
	}
	isCVS {
		^this.fileName == "CVS";
	}
	filesDo { arg func;
		this.files.do(func);
		this.foldersWithoutCVS.do { arg pathname;
			pathname.filesDo(func)
		}
	}
	streamTree { arg str, tabs=0;
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

Help : PathName {

	*all {
		^this.new("Help/").dumpToDoc("all-helpfiles");
	}

}

