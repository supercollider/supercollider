ApplicationStart : StartUp {
	classvar <>objects;

	*run {
		this.objects.do { |item| item.doOnApplicationStart }
	}
}

+ Function {
	doOnApplicationStart {
		try(this) { |error|
			"ApplicationStart: an error has occurred.".postln;
			error.reportError;
			"Thrown during function:".postln;
			this.postcs
		}
	}
}
