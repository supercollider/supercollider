+ Document {
	// Document class for Emacs interface.
	// Delegates to EmacsDocument.

	*initClass { }
	*open { | path, selectionStart=0, selectionLength=0 |
		^EmacsDocument.prBasicNew.initFromPath(path, selectionStart, selectionLength)
	}	
	*new { | title="Untitled", string="", makeListener=false |
		^EmacsDocument.prBasicNew.initByString(title, string, makeListener);		
	}
	
	*listener { ^thelistener }

	// PRIVATE
	*prBasicNew { ^super.new }
	*numberOfOpen { ^allDocuments.size }	
	*newFromIndex { ^this.shouldNotImplement }
	*prGetLast { ^allDocuments.last }	
	*prGetIndexOfListener { ^this.shouldNotImplement }
}

// EOF