
Done : UGen {	
	*kr { arg src;
		^this.multiNew('control', src)
	}
}

FreeSelf : UGen {	
	*kr { arg src;
		^this.multiNew('control', src)
	}
}

PauseSelf : UGen {	
	*kr { arg src;
		^this.multiNew('control', src)
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
// doneAction = 5   remove and deallocate this synth and free all children in the preceeding group (if it is a group).
// doneAction = 6   remove and deallocate this synth and free all children in the following group (if it is a group).

EnvGen : UGen { // envelope generator	
	*ar { arg envelope, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;
		^this.multiNewList(['audio', gate, levelScale, levelBias, timeScale, doneAction] 
			++ envelope.asArray)
	}
	*kr { arg envelope, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;
		^this.multiNewList(['control', gate, levelScale, levelBias, timeScale, doneAction] 
			++ envelope.asArray)
	}
}

//BufEnvGen : UGen { // envelope generator	
//	*ar { arg bufnum, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;
//		^this.multiNew('audio', bufnum, gate, levelScale, levelBias, timeScale, doneAction)
//	}
//	*kr { arg bufnum, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;
//		^this.multiNew('control', bufnum, gate, levelScale, levelBias, timeScale, doneAction)
//	}
//}

Linen : UGen {
	*kr { arg gate = 1.0, attackTime = 0.01, susLevel = 1.0, releaseTime = 1.0, doneAction = 0;
		^this.multiNew('control', gate, attackTime, susLevel, releaseTime, doneAction)
	}
}

