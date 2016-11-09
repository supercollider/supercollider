

+ Object {
	openHelpFile {
		this.deprecated(thisMethod);
		this.help
	}
}

+ String {
	openHelpFile {
		this.deprecated(thisMethod);
		this.help
	}
}

+ Method {
	openHelpFile {
		this.deprecated(thisMethod);
		this.help
	}
}

+ Quark {
	openHelpFile {
		this.deprecated(thisMethod);
		this.help
	}
}


// openTextFile is actually the same as openDocument
+ String {
	openTextFile { arg selectionStart=0, selectionLength=0;
		this.deprecated(thisMethod);
		this.openDocument(selectionStart, selectionLength)
	}
}

+ Symbol {
	openTextFile { arg selectionStart=0, selectionLength=0;
		this.deprecated(thisMethod);
		^this.openDocument(selectionStart, selectionLength)
	}
}
