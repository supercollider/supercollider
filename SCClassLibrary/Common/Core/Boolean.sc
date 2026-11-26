Boolean {
	*new { ^this.shouldNotImplement(thisMethod) }
	*newCopyArgs { ^this.shouldNotImplement(thisMethod) }
	xor { |bool| ^(this === bool).not }
	if { ^this.subclassResponsibility(thisMethod) }
	not { ^this.subclassResponsibility(thisMethod) }
	&& { ^this.subclassResponsibility(thisMethod) }
	|| { ^this.subclassResponsibility(thisMethod) }
	and { ^this.subclassResponsibility(thisMethod) }
	or { ^this.subclassResponsibility(thisMethod) }
	nand { ^this.subclassResponsibility(thisMethod) }
	asInteger { ^this.subclassResponsibility(thisMethod) }
	binaryValue { ^this.subclassResponsibility(thisMethod) } // TODO: deprecate for asInteger

	asBoolean    { ^this }
	booleanValue { ^this } // TODO in the long-run, deprecate for asBoolean

	keywordWarnings {
		// turn on/off warnings if a keyword argument is not found
		_KeywordError
		^this.primitiveFailed
	}
	trace {
		// this is only available in a special debugging version of the app
		_Trace
		^this.primitiveFailed
	}
	printOn { |stream|
		stream.putAll(this.asString)
	}
	storeOn { |stream|
		stream.putAll(this.asCompileString)
	}
	archiveAsCompileString { ^true }

	while {
		^"While was called with a fixed (unchanging) Boolean as the condition. Please supply a Function instead.".error
	}
}

True : Boolean {
	if { |trueFunc, falseFunc| ^trueFunc.value }
	not { ^false }
	&& { |that| ^that.value }
	|| { |that| ^this }
	and { |that| ^that.value }
	or { |that| ^this }
	nand { |that| ^that.value.not }
	asInteger   { ^1 }
	binaryValue { ^1 } // TODO in the long-run, deprecate for asInteger
}

False : Boolean  {
	if { |trueFunc, falseFunc| ^falseFunc.value }
	not { ^true }
	&& { |that| ^this }
	|| { |that| ^that.value }
	and { |that| ^this }
	or { |that| ^that.value }
	nand { |that| ^true }
	asInteger   { ^0 }
	binaryValue { ^0 } // TODO in the long-run, deprecate for asInteger
}
