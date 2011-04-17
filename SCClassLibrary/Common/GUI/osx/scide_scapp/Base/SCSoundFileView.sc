SCSoundFileView : SCScope{
	var < soundfile;
	var <>metaAction, <elasticMode, <drawsWaveForm=true, <readProgress;
	var <startFrame, <numFrames;
	var zoomOne, <>dataFrames, <viewFrames, <scrollPos, <>block=64;

	init{ arg argParent, argBounds;
		super.init(argParent, argBounds);
		soundfile = SoundFile.new;
	}

	soundfile_{|snd|
		soundfile = snd;
		if(soundfile.isOpen){^this};
		if(soundfile.openRead.not){^this};
		soundfile.close;
	}

	read { |startframe=0, frames, block=64, closeFile=true|
		this.block = block;
		if(soundfile.isOpen.not){
			if(soundfile.openRead.not){
				^this
			};
		};
		startFrame = startframe;
		frames = frames ? soundfile.numFrames;

		if (frames == -1) { frames = soundfile.numFrames };
		numFrames = frames.clip(0, soundfile.numFrames);

		this.readFile(soundfile, startframe, frames, block);
		if(closeFile){soundfile.close};
	}

	readWithTask{|startframe=0, frames, block=64, doneAction, showProgress=true|
		this.readFileWithTask(soundfile, startframe, frames, block, doneAction, showProgress);
	}

	makeProgressWindow{
		this.addDependant(SoundFileViewProgressWindow(soundfile.path.split.last));
	}

	// needs an open soundfile;
	readFileWithTask{|soundfile, startframe=0, frames, block=64,
						doneAction, showProgress=true|
		var zoomx, o, readtime, data, resampleFrames;
		this.block = block;
		if(showProgress){
			this.makeProgressWindow;
		};
		frames = frames ? soundfile.numFrames;
		resampleFrames = frames/block;
		this.drawsWaveForm_(false);
		o = 2**19;
		zoomx = (resampleFrames/(this.bounds.width-2));
		Task({
			var times, frame, data, spec;
			data = Array(resampleFrames*soundfile.numChannels);
			frame = startframe;
			times = (frames/o).ceil;
			spec = [0, times].asSpec;
			times.do{|i|
				readtime = {
					this.read(frame, o, block, false);
					frame = frame + o;
					readProgress = spec.unmap(i);
				}.bench(false);
				(readtime*0.001).wait;
					data.addAll(this.data);
					this.changed(\progress);
				(readtime*0.05).wait;
			};
			this.setData(data, block, startframe, soundfile.numChannels, soundfile.sampleRate);
			this.xZoom_(zoomx).drawsWaveForm_(true).refresh;

			this.updateData;

			doneAction.value(this);
			soundfile.close;
			this.changed(\progressFinished);
		}).play(AppClock);
	}

	readFile{|asoundfile, startframe=0, frames=0, block=0, closefile=true|
		this.setProperty(\readSndFile,
			[asoundfile, startframe, frames, block,
			soundfile.sampleRate,
			soundfile.numFrames,
			soundfile.numChannels]);
		this.updateData;
	}

	mouseEndTrack{|x,y|
//		mouseUpAction.value(this, x,y)
	}

	doMetaAction{
		metaAction.value(this)
	}

	currentSelection_{|index|
		this.setProperty(\currentSelection, index);
	}

	currentSelection{
		^this.getProperty(\currentSelection);
	}

	setSelectionStart{|index,frame|
		this.setProperty(\selectionStart, [index,frame]);
	}

	setSelectionSize{|index,frame|
		this.setProperty(\selectionSize, [index,frame]);
	}

	setEditableSelectionStart{|index, bool|
		this.setProperty(\selectionStartIsEditable, [index, bool]);
	}

	setEditableSelectionSize{|index, bool|
		this.setProperty(\selectionSizeIsEditable, [index, bool]);
	}

	setSelectionColor{|index,color|
		this.setProperty(\selectionColor, [index, color]);
	}

	selections{
		var array;
		array = Array.fill(64, {[0,0]});
		^this.getProperty(\selections, array);
	}

	selectionStart{|index|
		^this.getProperty(\selectionStart, index);
	}

	selectionSize{|index|
		^this.getProperty(\selectionSize, index);
	}

	selection { |index|
		^[this.selectionStart(index), this.selectionSize(index)]
	}
	setSelection { |index, selection|
		this.setSelectionStart(index, selection[0])
			.setSelectionSize(index, selection[1]);
	}

	selectionStartTime{|index|
		^this.getProperty(\selectionStartTime, index);
	}

	selectionDuration{|index|
		^this.getProperty(\selectionDuration, index);
	}

	readSelection{ |block=64, closeFile=true|
		this.read(
			this.selectionStart(this.currentSelection),
			this.selectionSize(this.currentSelection),
			block,
			closeFile
		);
	}

	readSelectionWithTask{
		this.readWithTask(
			this.selectionStart(this.currentSelection),
			this.selectionSize(this.currentSelection)
		);
	}

	gridOn_{|boolean|
		this.setProperty(\gridOn, boolean);
	}

	gridResolution_{|resolution|
		this.setProperty(\gridResolution, resolution);
	}

	gridOn { ^this.getProperty(\gridOn) }
	gridResolution { ^this.getProperty(\gridResolution) }
	gridColor { ^this.getProperty(\gridColor, Color.new) }
	timeCursorOn { ^this.getProperty(\timeCursorOn) }
	timeCursorPosition { ^this.getProperty(\timeCursorPosition) }
	timeCursorColor { ^this.getProperty(\timeCursorColor, Color.new) }

	dataNumSamples{
		^this.getProperty(\getViewNumSamples);
	}

	data{
		var arr;
		arr = Array.newClear(this.dataNumSamples);
		^this.getProperty(\getViewData, arr);
	}

	data_{|arr|
		this.setData(arr, 64, 0, soundfile.numChannels, soundfile.sampleRate);
	}

	setData{|arr, block=64, startframe = 0, channels=1, samplerate=44100|
		this.block = block;
		this.setProperty(\setViewData, [arr, block, startframe, channels, samplerate]);
	}

	elasticMode_{arg mode;
		elasticMode =mode;
		this.setProperty(\elasticResizeMode, mode);
	}

	drawsWaveForm_{|bool|
		drawsWaveForm = bool;
		this.setProperty(\drawsWaveForm, bool);
	}

	timeCursorPosition_{|frame|
		this.setProperty(\timeCursorPosition, frame);
	}

	timeCursorOn_{|bool|
		this.setProperty(\showTimeCursor, bool)
	}

	timeCursorColor_{|color|
		this.setProperty(\timeCursorColor, color)
	}

	zoom {| factor |	// zoom factor n or 1/n.
		this.xZoom = zoomOne.min(this.xZoom * factor);
		viewFrames = dataFrames * block * (this.xZoom / zoomOne);
		if ( this.x > (soundfile.numFrames - viewFrames), {
			this.x_(soundfile.numFrames - viewFrames)
		});
		this.refresh;
		this.updateScroll
	}
	zoomToFrac { arg frac; // 0..1
		this.xZoom = (frac * zoomOne);
		viewFrames = dataFrames * block * (this.xZoom / zoomOne);
		if ( this.x > (soundfile.numFrames - viewFrames), {
			this.x_(soundfile.numFrames - viewFrames)
		});
		this.refresh;
		this.updateScroll
	}
	zoomAllOut {
		this.x_(0); this.xZoom = zoomOne; this.refresh;
		viewFrames = dataFrames * block * (this.xZoom / zoomOne);
		this.updateScroll;
	}

	zoomSelection {| index |	// selection index
		if ( this.selectionSize(index) > 0, {
			this.x_(this.selectionStart(index));
			this.xZoom = this.selectionSize(index) / (this.bounds.width-2) / block;
			viewFrames = dataFrames * block * (this.xZoom / zoomOne);
			this.refresh;
			this.updateScroll
		})
	}

	scrollTo { | position |		// absolute. from 0 to 1
		this.x_((soundfile.numFrames - viewFrames) * position).refresh;
		this.updateScroll;
	}

	scroll {| amount |	// +/- range in viewFrames
		this.x_((this.x + (amount*viewFrames))
			.clip(0,soundfile.numFrames - viewFrames)).refresh;
		this.updateScroll
	}

	scrollToStart {
		this.x_(0).refresh;
		scrollPos = 0;
	}

	scrollToEnd {
		this.x_(soundfile.numFrames  - viewFrames ).refresh;
		scrollPos = 1;
	}

	selectAll {| index |	// selection index
		this.setSelectionStart(index, 0); this.setSelectionSize(index, this.soundfile.numFrames)
	}

	selectNone {| index |	 // selection index
		this.setSelectionSize(index, 0); // sends x to 0 when selection is not full width visible? ( thus update scrollPos )
		this.updateScroll
	}

	gridOffset_{|offset|
		this.setProperty(\gridOffset, offset);
	}

/* private methods*/

	updateScroll {
		scrollPos = this.x / (soundfile.numFrames - viewFrames)
	}

	updateData {
		scrollPos = 0;
		dataFrames = this.dataNumSamples/this.soundfile.numChannels;
		zoomOne = dataFrames / (this.bounds.width-2);
		viewFrames = dataFrames * block * (this.xZoom / zoomOne);
	}

	*paletteExample { arg parent, bounds;
		var example, file;
		example = this.new(parent, bounds);
		file = SoundFile.new;
		file.openRead("sounds/a11wlk01.wav");
		example.soundfile = file;
		example.read(0, file.numFrames);
		^example
	}

}

SoundFileViewProgressWindow{
	var win, slider;

	*new{|name|
		^super.new.makeWindow(name)
	}

	makeWindow{|name|
		win = SCWindow.new("reading: " ++ name,
				 Rect(100,100,300, 40), false).front;
		win.view.decorator = FlowLayout(win.view.bounds);
		slider = SCRangeSlider(win, Rect(4,4,290,10));
	}

	update{|changed, changer|

		if(changer === \progress){
			{slider.lo_(0).hi_(changed.readProgress)}.defer
			^this
		};
		if(changer === \progressFinished){
			{win.close;
			changed.removeDependant(this)}.defer;
			^this
		};

	}
}

/* jan truetzschler 2005 */
