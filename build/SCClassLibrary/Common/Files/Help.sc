Help {

	*all {
		//		^this.new("Help/").dumpToDoc("all-helpfiles");
		var doc;
		var str = CollStream.new;
		doc = Document.new("all-helpfiles");
		[ 
			Platform.classLibraryDir,
			Platform.systemAppSupportDir,
			Platform.userAppSupportDir,
			Platform.systemExtensionDir,
			Platform.userExtensionDir
		].do{ |it| PathName.new( it ).foldersWithoutSVN.select{ |it| it.fullPath.contains("Help") }.do{ |help| help.streamTree(str) } };
		doc.string = str.collection;
	}	
}

