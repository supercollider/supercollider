
AbstractConsole {

	var <>layout,<>defaultPath;
	
	getPathThen {  arg then ... args;
		/*
		PutFileDialog.new("Filename..." ++ then.asString, defaultPath, { arg ok, path;
			if (ok, {
				defaultPath = path;
				this.performList(then,[path] ++ args)
			})
		})
		*/
	}
	addToLayout { arg moreFunc;
		moreFunc.value(layout)
	}
}

SynthConsole : AbstractConsole  {

	var format =  '32float', <>duration;
	var <>ugenFunc,<>onRecordOrWrite;
	 var pauseControl;
	 
	 var tempoG;
	 
	*new { arg ugenFunc,layout;
		^super.new.ugenFunc_(ugenFunc).layout_(layout.asPageLayout)
	}
	
	play {	// any time you hit play, you register the space bar as the stop/start play key
			// if several consoles are open, the most recently played one has the keydown registration
		ActionButton(layout,">",{this.registerPlayKey.doPlay }).backColor_(Color.green);
	}
	registerPlayKey { arg keycode=49; // spacebar
		/*	KeyCodeResponder.resetKeycode(keycode).registerKeycode(0,keycode,{  
			if(Synth.isPlaying,{
				this.doStop
			},{
				this.doPlay    
			})
		});//  
		*/
	}	
	scope {arg duration=0.5;
		//ActionButton(layout,"scope",{this.doScope(duration)})
		//	.backColor_(Color.green);
	}
	fftScope {
		//ActionButton(layout,"fftScope",{this.doFFTScope})
		//	.backColor_(Color.green);
	}
	record { arg defpath;
		/*if(defpath.notNil,{ defaultPath = defpath });
		ActionButton(layout,"|*|",{
			this.getPathThen(\doRecord);
		}).backColor_(Color.red);
		*/
	}
	pauseableRecord { arg defpath;
		//if(defpath.notNil,{ defaultPath = defpath });
		// should be a multi state button
		//ActionButton(layout,"|,|",{
		//	this.getPathThen(\doPauseableRecord);
		//}).backColor_(Color.red);
		//pauseControl = CheckBoxView(layout.win,layout.layRight(29,13),",");
	}
	write {arg dur,defpath;
//		if(defpath.notNil,{ defaultPath = defpath });
//		ActionButton(layout, "{}",{ this.getPathThen(\doWrite,dur.value ? duration ?? { 120 }) } ); // do a dialog
	}
	
	stop { arg stopFunc;
		ActionButton(layout,"[_]",{ 
			this.doStop(stopFunc)
		});
		//KeyCodeResponder.registerKeycode(16,49,{ this.doStop      });//  
	}

	formats {
		//SelectButtonSet(layout,['32float','16aiff','24AIFF'],{arg i,th; format = th.selectedLabel })
	}

	tempo {
		Tempo.gui(layout);
//		TempoGui.setTempoKeys;
	}

	// pr
	doPlay { 
		this.ugenFunc.play;
	}
	
	doScope { arg duration=0.5;
//		Synth.scope({ arg synth;
//			Tempo.setTempo;
//			this.ugenFunc.value(synth)
//		},duration)
	}
	doFFTScope {
//		Synth.fftScope({ arg synth;
//			Tempo.setTempo;
//			this.ugenFunc.value(synth)
//		})
	}
	
	doStop { arg stopFunc;
		stopFunc.value;
		ugenFunc.tryPerform(\stop) ?? {RootNode.new.freeAll};
		NotificationCenter.notify(this,\didStop);
	}
		
	doRecord {	arg path;
//		var hformat,sformat;
//		# hformat, sformat = this.getFormats;
//	
//		Synth.record({arg synth; 
//			Tempo.setTempo;
//			this.ugenFunc.value(synth) 
//		},duration,path,hformat,sformat);
//		onRecordOrWrite.value(path);
//		NotificationCenter.notify(this,\didRecordOrWrite);
	}
	
	doWrite { arg path, argduration;
//		var hformat,sformat;
//		# hformat, sformat = this.getFormats;
//	
//		Synth.write({arg synth; 
//			Tempo.setTempo;
//			this.ugenFunc.value(synth) 
//		} ,argduration ? duration,path,hformat,sformat);	
//		onRecordOrWrite.value(path);
//		NotificationCenter.notify(this,\didRecordOrWrite);
	}
	doPauseableRecord { arg path;
//
//		var file, newsynth;
//		var hformat,sformat;
//		# hformat, sformat = this.getFormats;
//
//		newsynth = Synth.new({ arg synth;
//			var ugenGraph;
//
//			Tempo.setTempo;
//
//			ugenGraph = this.ugenFunc.value(synth).asArray;
//			
//			file = SoundFile.new;
//			file.headerFormat = hformat;
//			file.sampleFormat = sformat;
//			file.numChannels = ugenGraph.size;
//			if (file.writeHeader(path), {
//				file.prepareRecord;
//				Pause.ar({
//					DiskOut.ar(file, 32768, ugenGraph)
//				},pauseControl.kr);
//				ugenGraph
//			},{
//				file = nil;
//				nil
//			});
//		});
//		
//		if (newsynth.notNil, { 
//			NotificationCenter.registerOneShot(this,\didStop,this,{ 
//				//Synth.stop appears to return asynch
//				if(Synth.isPlaying.not,{
//					file.endRecord
//				});
//			});
//			newsynth.play;
//			file.endRecord;
//		});
	}
	
	getFormats {
		
		if(format=='16aiff',{
			^[ 'AIFF' ,'16 big endian signed']
		});
		
		if(format=='32float',{
			^[ 'Sun',  '32 big endian float' ]
		});
		
		if(format=='24AIFF',{
			^[ 'AIFF',  '24 big endian signed' ]
		});
		
		^nil 
	}

}




SaveConsole : AbstractConsole {

	var <>object;
	var <path,<>defaultPath; // defaultPath may be a function
	var <>onSaveAs; // arg path
	
	*new { arg object, path,layout;
		^super.new.object_(object).path_(path)
			.layout_(layout ?? {MultiPageLayout.new.front})
	}
	
	print {
		ActionButton(layout,"#",{ object.asCompileString.postln }); 
	}
	printPath {
		ActionButton(layout,"#path",{ path.value.asCompileString.postln })
	}
	//dirtyAwareSave
	save { arg title="save",maxx=100;
	 	/*
		ActionButton(layout,title,{
			if(path.value.isNil,{
		 		this.getPathThen(\doSaveAs);
		 	},{	
		 		this.doSave 
		 	})
	 	},maxx).backColor_(
	 		if(path.value.isNil,{ // virgin
	 			Color(202,255,161) 
	 		},{
	 			Color(255,242,89)
	 		})
	 	);
	 	*/
	}
	saveAs { arg onSaveF,default;
		/*
		defaultPath = default ? defaultPath;
		onSaveAs = onSaveF ? onSaveAs;
		ActionButton(layout,"save as",{
			this.getPathThen(\doSaveAs,default);
	 	});
	 	*/
	}
	load {
		//ActionButton(layout,"load",{ this.doLoad });
	}	
			
	getPathThen {  arg then ... args;
		/*
		var defPath;
		defPath=(defaultPath.value ? object).asString;
		defPath = defPath.copyRange(0,30); // mac beeps if path is too long
		File.saveDialog("Filename..." ++ then.asString, defPath, { arg  argpath;
			this.path = argpath;
			this.performList(then,[path] ++ args)
		});
		*/
	}
	
	doSave {
		var clobber,vpath;
		vpath = path.value;
		clobber=File.new(vpath,"r");
		if(clobber.length.notNil,{
			clobber.contents.write("~"++vpath);
		});
		clobber.close;	
		object.asCompileString.write(vpath);
	}
	doSaveAs {
		var clobber,vpath;
		vpath = path.value;
		clobber=File.new(vpath,"r");
		if(clobber.length.notNil,{
			clobber.contents.write("~"++vpath);
		});
		clobber.close;	
		object.asCompileString.write(vpath);
		onSaveAs.value(vpath);
	}

	doLoad { arg onLoad; // optional func
		/*
		File.openDialog("Load...",{ arg apath;
			this.path = apath;
			object=thisProcess.interpreter.executeFile(apath);
			onLoad.value(object,this)
		})
		*/
	}
	path_ { arg p; 
		if(p.notNil,{
			path = PathName(p).asRelativePath 
		})
	}
	
}



SoundFileFormats { // an interface

	var format='32float';

	gui { arg layout;
		SelectButtonSet(layout,20,16,['32float','16aiff','16SD2','24SD2'],{arg i,th; format = th.selectedLabel },
			Color(245, 245, 245),Color(255, 99, 71))
	}

	headerFormat {
	
		if(format=='16aiff',{
			^'AIFF'
		});
		
		if(format=='16SD2',{
			^'SD2'
		});
		
		if(format=='32float',{
			^'Sun'
		});
		
		if(format=='24SD2',{
			^'SD2'
		});
		
		^nil 
		
	}
	sampleFormat {
	
		if(format=='16aiff',{
			^'16 big endian signed'
		});
		
		if(format=='16SD2',{
			^'16 big endian signed'
		});
		
		if(format=='32float',{
			^'32 big endian float'
		});
		
		if(format=='24SD2',{
			^'24 big endian signed'
		});
		
		^nil 
		
	}
}

