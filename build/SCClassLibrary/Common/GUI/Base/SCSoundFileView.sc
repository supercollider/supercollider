SCSoundFileView : SCScope{
	var < soundfile;
	var <>metaAction, <>mouseUpAction, <elasticMode, <drawsWaveForm=true, <readProgress;
	var <startFrame, <numFrames;
	
	init{ arg argParent, argBounds;
		super.init(argParent, argBounds);
		soundfile = SoundFile.new;
	}
	
	soundfile_{|snd|
		soundfile = snd;
		if(soundfile.isOpen){^this};
		soundfile.openRead(snd.path);
		if(soundfile.openRead(snd.path).not){^this};
		soundfile.close;
	}

	read{|startframe=0, frames=0, block=64|
		if(soundfile.isOpen.not){
			if(soundfile.openRead.not){
				^this
			}
		};
		startFrame = startframe;
		numFrames = frames;
		this.readFile(soundfile, startframe, frames, block)
	}

	readWithTask{|startframe=0, frames, block=64, doneAction, showProgress=true|
		this.readFileWithTask(soundfile, startframe, frames, block, doneAction, showProgress);
	}
	
	makeProgressWindow{
		this.addDependant(SoundFileViewProgressWindow(soundfile.path.split.last));
	}
	
	// needs an opended soundfile;
	readFileWithTask{|soundfile, startframe=0, frames, block=64, 
						doneAction, showProgress=true|
		var zoomx, o, readtime, data, resampleFrames;
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
					this.read(frame, o, block);
					frame = frame + o;		
					readProgress = spec.unmap(i);
				}.bench(false);
				(readtime*0.001).wait;
					data.addAll(this.data);
					this.changed(\progress);
				(readtime*0.05).wait;
			};
			this.setData(data, block, startframe);
			this.xZoom_(zoomx).drawsWaveForm_(true).refresh;
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
		this.setData(arr);		
	}
	
	setData{|arr, block=64, startframe = 0|
		this.setProperty(\setViewData, Array.with(arr, block, startframe));
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
	
	
	
}

SoundFileViewProgressWindow{
	var win, slider;
	
	*new{|name|
		^super.new.makeWindow(name)
	}
	
	makeWindow{|name|
		win = SCWindow.new("reading: " ++ name,
				 Rect(100,100,300, 40)).front;
		win.view.decorator = FlowLayout(win.view.bounds);
		slider = SCSlider(win, Rect(4,4,290,10));
			//.background_(Color.black).knobColor_(Color.white);	
	}
	
	update{|changed, changer|
		
		if(changer === \progress){
			{slider.value_(changed.readProgress)}.defer
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