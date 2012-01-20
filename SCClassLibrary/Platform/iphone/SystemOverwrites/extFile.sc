//primitive fails with boost::filesystem::status: Operation not permitted: "/tmp/"
//using the old sclang implementation for file exists until fixed.

+ File {
	*exists { arg pathName;
		var file;
		file = File(pathName,"r");
		if (file.isOpen, { file.close; ^true });
		^false
	}
}
