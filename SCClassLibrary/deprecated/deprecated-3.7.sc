// in 3.6, both "help" and "openHelpFile" will work fine. In 3.7, "openHelpFile" will be deprecated. In 3.8 it will be gone.

+ Object {
	openHelpFile {
		this.help
	}
}

+ String {
	openHelpFile {
		this.help
	}
}

+ Method {
	openHelpFile {
		this.help
	}
}

+ Quark {
	openHelpFile {
		this.help
	}
}


// openTextFile is actually the same as openDocument
+ String {
	openTextFile { arg selectionStart=0, selectionLength=0;
		this.openDocument(selectionStart, selectionLength)
	}
}

+ Symbol {
	openTextFile { arg selectionStart=0, selectionLength=0;
		^this.openDocument(selectionStart, selectionLength)
	}
}
