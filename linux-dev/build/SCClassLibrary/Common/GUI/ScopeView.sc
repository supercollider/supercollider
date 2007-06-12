/*
	These are from sc3d5 for os9.
	they do not work currently in sc3 (os x)

SignalArt {	var <>pen, <>bounds, <>bank, <>index, <>channel = 0;	var <>minval = -1.0, <>maxval = 1.0, <>scroll = 0, <>zoom = 1;		*new { arg pen, bounds, bank, index, minval = -1.0, maxval = 1.0, scroll = 0, zoom = 1, channel = 0;		^super.newCopyArgs(pen.asPen, bounds, bank, index, channel, minval, maxval, scroll, zoom)	}	draw {		pen.use({			this.prDraw;		});	}	fitZoom {		var size;		size = bank.numFramesInBuf(index);		zoom = ceil( size / bounds.width);	}}SignalView : SignalArt {	prDraw {		_SignalView_Draw		^this.primitiveFailed	}}ScopeView : SignalArt {	prDraw {		_ScopeView_Draw		^this.primitiveFailed	}	fitZoom {		var size;		size = bank.numFramesInBuf(index) >> 1;		zoom = ceil( size / bounds.width);	}}XYScopeView : SignalArt {	prDraw {		_XYScopeView_Draw		^this.primitiveFailed	}}
*/
