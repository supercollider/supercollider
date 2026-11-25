SoundFileView : View {

	var <>soundfile;
	var <metaAction;
	var <>elasticMode; // NOTE: no-op, only for compatibility
	var curDoneAction;

	*qtClass { ^'QcWaveform' }

	load { |filename, startFrame, frames, block, doneAction|
		if(filename.isString && filename != "") {
			if(curDoneAction.notNil)
				{ this.disconnectFunction('loadingDone()', curDoneAction) };
			if(doneAction.notNil) {
				this.connectFunction('loadingDone()', doneAction)
			};

			curDoneAction = doneAction;

			if(startFrame.notNil && frames.notNil) {
				this.invokeMethod(\load, [filename, startFrame.asInteger, frames.asInteger])
			} {
				this.invokeMethod(\load, filename)
			}
		}
	}

	alloc { |frames, channels = 1, samplerate = 44100|
		this.invokeMethod(\allocate, [frames.asInteger, channels.asInteger, samplerate.asInteger])
	}

	data_ { |data|
		this.setData(data)
	}

	setData { |data, block, startFrame = 0, channels = 1, samplerate = 44100|
		if(data.isKindOf(DoubleArray).not and: { data.isKindOf(FloatArray).not })
			{ data = data.as(DoubleArray) };
		this.invokeMethod(\load, [data, startFrame, channels, samplerate.asInteger])
	}

	set { |offset = 0, data|
		if(data.isKindOf(DoubleArray).not and: { data.isKindOf(FloatArray).not })
			{ data = data.as(DoubleArray) };
		this.invokeMethod(\write, [data, offset.asInteger])
	}

	readFile { |aSoundFile, startFrame, frames, block, closeFile, doneAction|
		this.load(aSoundFile.path, startFrame, frames, block, doneAction)
	}

	read { |startFrame, frames, block, closeFile, doneAction|
		if(soundfile.notNil) {
			this.readFile(soundfile, startFrame, frames, block, nil, doneAction)
		}
	}

	readFileWithTask { |soundFile, startFrame, frames, block, doneAction, showProgress|
		this.readFile(soundFile, startFrame, frames, block, nil, doneAction)
	}

	readWithTask { |startFrame, frames, block, doneAction, showProgress|
		this.read(startFrame, frames, block, nil, doneAction)
	}

	drawsWaveForm { ^this.getProperty(\drawsWaveform) }
	drawsWaveForm_ { |boolean| this.setProperty(\drawsWaveform, boolean) }

	drawsRMS { ^this.getProperty(\drawsRMS) }
	drawsRMS_ { |boolean| this.setProperty(\drawsRMS, boolean) }

	drawsCenterLine { ^this.getProperty(\drawsCenterLine) }
	drawsCenterLine_ { |boolean| this.setProperty(\drawsCenterLine, boolean) }

	drawsBoundingLines { ^this.getProperty(\drawsBoundingLines) }
	drawsBoundingLines_ { |boolean| this.setProperty(\drawsBoundingLines, boolean) }

	waveColors { ^this.getProperty(\waveColors) }
	waveColors_ { |colors| this.setProperty(\waveColors, colors) }

	//// Info

	startFrame { ^this.getProperty(\startFrame) }

	numFrames { ^this.getProperty(\frames) }

	scrollPos { ^this.getProperty(\scrollPos) } // a fraction of the full scrolling range

	viewFrames { ^this.getProperty(\viewFrames) }

	readProgress { ^this.getProperty(\readProgress) }

	//// Navigation

	zoom { |factor| this.invokeMethod(\zoomBy, factor.asFloat) }

	zoomToFrac { |fraction| this.invokeMethod(\zoomTo, fraction.asFloat) }

	zoomAllOut { this.invokeMethod(\zoomAllOut) }

	zoomSelection { |selection|
		if(selection.isNil) { selection = this.currentSelection };
		this.invokeMethod(\zoomSelection, selection)
	}

	scrollTo { |fraction| // a fraction of the full scrolling range
		this.setProperty(\scrollPos, fraction)
	}

	scroll { |fraction| // a fraction of the visible range
		var frames = this.viewFrames * fraction + this.getProperty(\viewStartFrame);
		this.setProperty(\viewStartFrame, frames)
	}

	scrollToStart { this.invokeMethod(\scrollToStart) }

	scrollToEnd { this.invokeMethod(\scrollToEnd) }

	xZoom { ^this.getProperty(\xZoom) }

	xZoom_ { |seconds| this.setProperty(\xZoom, seconds) }

	yZoom { ^this.getProperty(\yZoom) }

	yZoom_ { |factor| this.setProperty(\yZoom, factor.asFloat) }

	yOffset { ^this.getProperty(\yOffset) }

	yOffset_ { |offset| this.setProperty(\yOffset, offset.asFloat) }

	spacing { ^this.getProperty(\spacing) }

	spacing_ { |factor| this.setProperty(\spacing, factor.asFloat) }

	//// Selections

	selections { ^this.getProperty(\selections) }

	currentSelection { ^this.getProperty(\currentSelection) }

	currentSelection_ { |index| this.setProperty(\currentSelection, index) }

	selection { |index| ^this.invokeMethod(\selection, index, true) }

	setSelection { |index, selection|
		this.invokeMethod(\setSelection, [index, selection])
	}

	selectionStart { |index|
		var sel = this.selection(index);
		^sel.at(0)
	}

	setSelectionStart { |index, frame|
		var sel = this.selection(index);
		sel.put(0, frame);
		this.setSelection(index, sel)
	}

	selectionSize { |index|
		var sel = this.selection(index);
		^sel.at(1)
	}

	setSelectionSize { |index, frames|
		var sel = this.selection(index);
		sel.put(1, frames);
		this.setSelection(index, sel)
	}

	selectAll { |index| this.setSelection(index, [0, this.numFrames]) }

	selectNone { |index| this.setSelection(index, [0, 0]) }


	setEditableSelectionStart { |index, editable| ^this.nonimpl("setEditableSelectionStart") }

	setEditableSelectionSize { |index, editable| ^this.nonimpl("setEditableSelectionSize") }

	setSelectionColor { |index, color| this.invokeMethod(\setSelectionColor, [index, color]) }


	selectionStartTime { |index| ^this.nonimpl("selectionStartTime") }

	selectionDuration { |index| ^this.nonimpl("selectionDuration") }


	readSelection { |block, closeFile| ^this.nonimpl("readSelection") }

	readSelectionWithTask { ^this.nonimpl("readSelectionWithTask") }

	// cursor

	timeCursorOn { ^this.getProperty(\cursorVisible) }
	timeCursorOn_ { |flag| this.setProperty(\cursorVisible, flag) }

	timeCursorEditable { ^this.getProperty(\cursorEditable) }
	timeCursorEditable_ { |flag| this.setProperty(\cursorEditable, flag) }

	timeCursorPosition { ^this.getProperty(\cursorPosition) }
	timeCursorPosition_ { |frame| this.setProperty(\cursorPosition, frame) }

	// grid

	gridOn { ^this.getProperty(\gridVisible) }
	gridOn_ { |flag| this.setProperty(\gridVisible, flag) }

	gridResolution { ^this.getProperty(\gridResolution) }
	gridResolution_ { |seconds| this.setProperty(\gridResolution, seconds) }

	gridOffset { ^this.getProperty(\gridOffset) }
	gridOffset_ { |seconds| this.setProperty(\gridOffset, seconds) }

	// colors

	peakColor { ^this.getProperty(\peakColor) }
	peakColor_ { |color| this.setProperty(\peakColor, color) }

	rmsColor { ^this.getProperty(\rmsColor) }
	rmsColor_ { |color| this.setProperty(\rmsColor, color) }

	timeCursorColor { ^this.getProperty(\cursorColor) }
	timeCursorColor_ { |color| this.setProperty(\cursorColor, color) }

	gridColor { ^this.getProperty(\gridColor) }
	gridColor_ { |color| this.setProperty(\gridColor, color) }

	// actions

	metaAction_ { |action|
		this.manageFunctionConnection(metaAction, action, 'metaAction()');
		metaAction = action
	}
}
