
Done : UGen {	
	*kr { arg src;
		^this.multiNew('control', src)
	}
}

FreeSelf : UGen {	
	*kr { arg in;
		this.multiNew('control', in);
		^in
	}
}

PauseSelf : UGen {	
	*kr { arg in;
		this.multiNew('control', in);
		^in
	}
}

FreeSelfWhenDone : UGen {	
	*kr { arg src;
		^this.multiNew('control', src)
	}
}

PauseSelfWhenDone : UGen {	
	*kr { arg src;
		^this.multiNew('control', src)
	}
}

Pause : UGen {	
	*kr { arg gate, id;
		^this.multiNew('control', gate, id)
	}
}

Free : UGen {	
	*kr { arg trig, id;
		^this.multiNew('control', trig, id)
	}
}

// the doneAction arg lets you cause the EnvGen to stop or end the 
// synth without having to use a PauseSelfWhenDone or FreeSelfWhenDone ugen. 
// It is more efficient to use a doneAction.
// doneAction = 0   do nothing when the envelope has ended.
// doneAction = 1   pause the synth running, it is still resident.
// doneAction = 2   remove the synth and deallocate it.
// doneAction = 3   remove and deallocate both this synth and the preceeding node.
// doneAction = 4   remove and deallocate both this synth and the following node.
// doneAction = 5   remove and deallocate this synth and if the preceeding node is a group then do g_freeAll on it, else n_free it.
// doneAction = 6   remove and deallocate this synth and if the following node is a group then do g_freeAll on it, else n_free it.
// doneAction = 7   remove and deallocate this synth and all preceeding nodes in this group
// doneAction = 8   remove and deallocate this synth and all following nodes in this group

// doneAction = 9   remove and deallocate this synth and pause the preceeding node
// doneAction = 10   remove and deallocate this synth and pause the following node
// doneAction = 11   remove and deallocate this synth and if the preceeding node is a group then do g_deepFree on it, else n_free it.
// doneAction = 12   remove and deallocate this synth and if the following node is a group then do g_deepFree on it, else n_free it.

EnvGen : UGen { // envelope generator	
	*ar { arg envelope, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;
		^this.multiNewList(['audio', gate, levelScale, levelBias, timeScale, doneAction, `envelope])
	}
	*kr { arg envelope, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;
		^this.multiNewList(['control', gate, levelScale, levelBias, timeScale, doneAction, `envelope])
	}
	*new1 { arg rate, gate, levelScale, levelBias, timeScale, doneAction, envelope;

		^super.new.rate_(rate).addToSynth.init([gate, levelScale, levelBias, timeScale, doneAction] 
			++ envelope.dereference.asArray); 
	}
 	init { arg theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
	argNamesInputsOffset { ^2 }
}

Linen : UGen {
	*kr { arg gate = 1.0, attackTime = 0.01, susLevel = 1.0, releaseTime = 1.0, doneAction = 0;
		^this.multiNew('control', gate, attackTime, susLevel, releaseTime, doneAction)
	}
}

