
Document {

	classvar <>dir="";
	
	*standardizePath { arg p;
		var pathName;
		pathName = PathName.fromOS9(p);
		^if(pathName.isRelativePath,{
			dir ++ pathName.fullPath
		},{
			pathName.fullPath
		})
	}
	
}

