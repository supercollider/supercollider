
PathName { 	// this class by originally by AdC 

	var <fullPath, <colonIndices;
	
	classvar <>scroot;
	
	*new { arg path = ""; 
		^super.new.init(path.standardizePath); 
	}
	*fromOS9 { arg path="";
		var treated;
		treated = Array.new(path.size);
		path = path.do({ arg char,i ;
					if(char == $:,{
						if(i != 0,{ // leading : is not wanted in unix
							treated.add($/);
						})
					},{
						treated.add(char)
					});
				});
		^super.new.init(treated.as(String))
	}
	*initClass {
		// uh ... no
		//scroot = 	unixCmd("pwd") ++ "/";
	}
	init { arg inPath;			// always calculate indices for all the colons,
							// since they are always needed.
		fullPath = inPath;	
		colonIndices = List.new;	
	 	fullPath.do({ arg eachChar, i; 
			if (eachChar == $/, { colonIndices.add(i) }); 
		});
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

	pathOnly { 		
		^fullPath.copyRange(0, this.lastColonIndex);
	}
	
	diskName {  ^fullPath.copyRange(0, colonIndices.first - 1) }
	
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
			// this makes a big assumption
			^scroot ++ fullPath;
		})
	}
	allFolders { 	
		var folderNames, pathCopy; 
		folderNames = List.new; 
		pathCopy = fullPath;

		colonIndices.doAdjacentPairs({ arg startColon, endColon; 
			folderNames.add( fullPath.copyRange(startColon + 1, endColon - 1) );
		});

		^folderNames
	}
	
	folderName {
		var indexBeforeFolder; 
		if (colonIndices.isEmpty, { ^"" }); 
		
		indexBeforeFolder = 
		if (colonIndices.size == 1, 0, 
			{ colonIndices.at(colonIndices.size - 2) + 1 });

		^fullPath.copyRange(indexBeforeFolder, this.lastColonIndex - 1);
	}	

	lastColonIndex { 				
		^if (colonIndices.isEmpty, { -1 }, { colonIndices.last }) ;
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
}
