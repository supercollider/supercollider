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

// Document: themes are cocoa-specific
+ Document {
	*theme_ { |...args|
		this.deprecated(thisMethod);
		this.implementationClass.tryPerform(\theme_, args)
	}

	*theme {
		this.deprecated(thisMethod);
		^this.implementationClass.tryPerform(\theme)
	}

	*postColor {
		this.deprecated(thisMethod);
		^this.implementationClass.tryPerform(\postColor)
	}

	*postColor_ {|...args|
		this.deprecated(thisMethod);
		^this.implementationClass.tryPerform(\postColor_, args)
	}

	*background {
		this.deprecated(thisMethod);
		^this.implementationClass.tryPerform(\background)
	}

	*background_ {|...args|
		this.deprecated(thisMethod);
		^this.implementationClass.tryPerform(\background_, args)
	}

	*selectedBackground {
		this.deprecated(thisMethod);
		^this.implementationClass.tryPerform(\selectedBackground)
	}

	*selectedBackground_ {|...args|
		this.deprecated(thisMethod);
		^this.implementationClass.tryPerform(\selectedBackground_, args)
	}

	*stringColor_ {|...args|
		this.deprecated(thisMethod);
		^this.implementationClass.tryPerform(\stringColor_, args)
	}

	setFont { | ...args |
		this.deprecated(thisMethod);
		^this.subclassResponsibility(thisMethod)
	}

	setTextColor { | ...args |
		this.deprecated(thisMethod);
		^this.subclassResponsibility(thisMethod)
	}

	syntaxColorize {
		this.deprecated(thisMethod);
		^this.subclassResponsibility(thisMethod)
	}
}
