
/*
	sc3d5 os 9 graphics code.
	not currently functioning.
	but very cool.

ImageSynth {	var <synth, <pic, finalizer;		*new { arg pic, synth;		^super.newCopyArgs(synth, pic);	}	start {		_ImageSynth_Start		^this.primitiveFailed	}	next { // generate one frame		_ImageSynth_Next		^this.primitiveFailed	}	end {		finalizer.finalize;	}}*/
