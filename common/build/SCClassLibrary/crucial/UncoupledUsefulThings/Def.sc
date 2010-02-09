

Def {

	*new { arg name,function;
		Library.put(Def,name,function)
	}
}

Do {
	*new { arg name ... args;
		^Library.at(Def,name).valueArray(args)
	}
}