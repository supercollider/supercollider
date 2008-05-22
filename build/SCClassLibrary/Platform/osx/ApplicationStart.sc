
ApplicationStart : StartUp {
	classvar <>functions, <done=false;
	
	*run {
		done = true;
		functions.do{|func|
			func.try{|error|
				"ApplicationStart: an error has occurred.".postln;
				error.reportError;
				"Thrown during function:".postln;
				func.postcs;
			}
		};
	}
}
