ApplicationStart : StartUp {
	classvar <>objects;

	*run {
		this.objects.do({ arg item; item.doOnApplicationStart;  });
	}
}

+ Function {
	doOnApplicationStart {
		this.try{|error|
				"ApplicationStart: an error has occurred.".postln;
				error.reportError;
				"Thrown during function:".postln;
				this.postcs;
		}
	}
}
