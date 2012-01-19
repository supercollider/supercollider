+ PatternProxy {
		// like asCompileString, but with full source etc.
	asCode {
		var mykey = this.key.asCompileString;
		var str;  str = "" ++ this.class.name ++ "(";
		
		if(source.isNil or: { source == this.class.default }) {
			str = str ++ mykey;
		} {
			str = str ++ this.repositoryArgs.asCompileString.drop(1).drop(-1);
		};
		str = str ++ ")";
		
		if(this.envir.notNil and: { this.envir.notEmpty }) {
			str = str ++ ".set(" ++
			this.envir.asKeyValuePairs.asCompileString.drop(1).drop(-1) ++ ")"
		};
		str = str ++ ";"
		^str
	}	
}

+ Tdef {
	printOn { |stream|
		^this.storeOn(stream);
	}
}
+ Pdef {
	printOn { |stream|
		^this.storeOn(stream);
	}
}
+ Pdefn {
	printOn { |stream|
		^this.storeOn(stream);
	}
}