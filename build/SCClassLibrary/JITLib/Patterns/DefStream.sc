DefStream : Pstr {
	
	var <>parent, <>defaultValue;
	
	*new { arg parent;
		^this.basicNew(parent.pattern).parent_(parent).resetPat;
	}
	
	resetPat {
			//pattern.post; "  resetPat.".postln;
			"_".post;
			pattern = parent.pattern;
			stream = Routine({ arg inval;
				var str, outval;
				str = pattern.asStream;
				this.makeDependant(parent);
				loop ({
						outval = str.value(inval);
						if(inval.isNil || outval.isNil, { 
							//\free.debug; 
							this.releaseDependant;
							this.resetPat;
							outval = defaultValue; //don't yield nil
						});
						inval = outval.yield(inval);
						
				});
		});
	}
	
	freeChain {
		stream.next(nil)
	}
	
	update {
		this.freeChain; 
		this.resetPat; 
	}
	
	makeDependant { arg argParent;
		parent = argParent;
		parent.children.add(this);
		//\madeDependant.debug;
	}
	
	releaseDependant {
		parent.children.remove(this); //then I should be dragged away by gc
	}
	

}