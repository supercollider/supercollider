SCSoundFileView : SCScope{
	var < soundfile;
	var <>metaAction, <>mouseUpAction, <elasticMode, <drawsWaveForm=true, <readProgress;
	var <startFrame, <numFrames;
	var zoomOne, <>dataFrames, viewFrames, scrollRatio, <scrollPos, <>block=64;
	
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

	read{|startframe=0, frames=0, block=64, closeFile=true|
		this.block = block;
		if(soundfile.isOpen.not){
			if(soundfile.openRead.not){
				^this
			};
		};
		startFrame = startframe;
		numFrames = frames;
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
		o = 2**15;
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
		mouseUpAction.value(this, x,y)
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

	selectionStartTime{|index|
		^this.getProperty(\selectionStartTime, index);
	}

	selectionDuration{|index|
		^this.getProperty(\selectionDuration, index);
	}	
	
	readSelection{
		this.read(
			this.selectionStart(this.currentSelection),
			this.selectionSize(this.currentSelection)
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

	zoom {| factor |	// zoom factor n or 1/n. view width units.
		this.xZoom = zoomOne.min(this.xZoom * factor);
		viewFrames = dataFrames * (this.xZoom / zoomOne);
		if ( (this.x/block) >= (dataFrames - viewFrames), {
			this.x_(0.max((dataFrames - viewFrames)*block));
		});
		this.xZoom = this.xZoom;
		this.refresh;
		this.updateScroll
	}
	
	zoomAllOut {
		viewFrames = dataFrames;
		this.x_(0); this.xZoom = zoomOne; this.refresh;
		this.updateScroll;
	}
	
	zoomSelection {| index |	// selection index
		if ( this.selectionSize(index) > 0, {
			this.x_(this.selectionStart(index));
			this.xZoom = this.selectionSize(index) / block / (this.bounds.width-2);
			this.refresh;
			this.updateScroll
		})
	}
	
	scrollTo { | position |		// absolute. from 0 to 1
		this.x_(soundfile.numFrames * position).refresh;
		this.updateScroll;
	}
	
	scroll {| amount |	// +/- range. view width units
		this.x_(this.x + (amount*soundfile.numFrames));
		this.refresh;
		this.updateScroll
	}
	
	scrollToStart {
		this.x_(0).refresh;
		this.updateScroll
	}
	
	scrollToEnd {
		this.x_(soundfile.numFrames).refresh;
		this.updateScroll
	}
	
	selectAll {| index |	// selection index
		this.setSelectionStart(index, 0); this.setSelectionSize(index, this.soundfile.numFrames)
	}
	
	selectNone {| index |	 // selection index
		this.setSelectionSize(index, 0)
	}
	
	gridOffset_{|offset|
		this.setProperty(\gridOffset, offset);		
	}

/* private methods*/

	updateScroll {
		scrollRatio = (1 - (this.xZoom / zoomOne)) * dataFrames;
		scrollPos = if( scrollRatio > 0, { this.x / scrollRatio }, { 0 })
	}
	
	updateData {
		scrollRatio = 0;
		scrollPos = 0;
		dataFrames = this.dataNumSamples/this.soundfile.numChannels;
		zoomOne = dataFrames / (this.bounds.width-2);
		viewFrames = dataFrames * (this.xZoom / zoomOne);
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