
Document {

	classvar <>dir="";
	
	// var path;
	
	*standardizePath { arg p;
		var pathName;
		pathName = PathName.fromOS9(p);
		^if(pathName.isRelativePath,{
			dir ++ pathName.fullPath
		},{
			pathName.fullPath
		})
	}
	// ~/Documents
	// /Volumes
	// Patches
	*abrevPath { arg path;
		if(path.size < dir.size,{ ^path });
		if(path.copyRange(0,dir.size - 1) == dir,{
			^path.copyRange(dir.size, path.size - 1)
		});
		^path
	}
	
	//*currentDocument
	//*openDocuments
	//*closeAllDocuments
	//*reviewUnsavedDocumentsWithAlertTitle
	//*saveAllDocuments
	//*recentDocumentPaths
	//*hasEditedDocuments
	//front
	//save
	//saveAs
	//open
	//print
	//close
	//isEdited
	
	//*keyDown
	//selectedText
	//insertText
	//text
	//hasPath  was loaded
	
}

