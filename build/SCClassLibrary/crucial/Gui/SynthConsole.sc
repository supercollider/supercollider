
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


