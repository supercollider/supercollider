
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

	var <>format, <>duration;
	var <>ugenFunc,<>onRecordOrWrite;
	 var pauseControl;

	 var tempoG;

	*new { arg ugenFunc,layout;
		^super.new.ugenFunc_(ugenFunc).layout_(layout.asPageLayout).format_(SoundFileFormats.new)
	}

	play {
		ActionButton(layout,">",{this.doPlay }).background_(Color.green);
	}
	prepare {
		ActionButton(layout,"pre",{this.doPrepare}).background_(Color.green(1.0,0.2))
	}
	scope {arg duration=0.5;
		//ActionButton(layout,"scope",{this.doScope(duration)})
		//	.background_(Color.green);
	}
	fftScope {
		//ActionButton(layout,"fftScope",{this.doFFTScope})
		//	.background_(Color.green);
	}
	record { arg defpath;
		/*if(defpath.notNil,{ defaultPath = defpath });
		ActionButton(layout,"|*|",{
			this.getPathThen(\doRecord);
		}).background_(Color.red);
		*/
	}
	pauseableRecord { arg defpath;
		//if(defpath.notNil,{ defaultPath = defpath });
		// should be a multi state button
		//ActionButton(layout,"|,|",{
		//	this.getPathThen(\doPauseableRecord);
		//}).background_(Color.red);
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
	}
	free {
		ActionButton(layout,"free",{
			ugenFunc.free;
		});
	}
	formats {
		format.gui(layout);
	}

	tempo {
		Tempo.default.gui(layout);
		//TempoGui.setTempoKeys;
	}

	// pr
	doPlay {
		this.ugenFunc.play;
	}
	doPrepare {
		this.ugenFunc.prepareForPlay
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
		ugenFunc.stop;
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

}




SaveConsole : AbstractConsole {

	var <>object;
	var <path; // defaultPath may be a function
	var <>onSaveAs; // arg path

	*new { arg object, path,layout;
		^super.new.object_(object).path_(path)
			.layout_(layout ?? {MultiPageLayout.new.front})
	}

	print {
		ActionButton(layout,"post",{ object.asCompileString.postln });
	}
	printPath {
		ActionButton(layout,"post path",{ path.value.asCompileString.postln })
	}
	save { arg title="save",minWidth=100;
		ActionButton(layout,title,{
			if(path.value.isNil,{
		 		this.getPathThen(\doSaveAs);
		 	},{
		 		this.doSave
		 	})
	 	},minWidth).background_(
	 		if(path.value.isNil,{ // virgin
	 			Color.new255(202,255,161)
	 		},{
	 			Color.new255(255,242,89)
	 		})
	 	);
	}
	saveAs { arg onSaveF,default;
		defaultPath = default ? defaultPath;
		onSaveAs = onSaveF ? onSaveAs;
		ActionButton(layout,"save as",{
			this.getPathThen(\doSaveAs,default);
	 	});
	}

	getPathThen {  arg then ... args;
		//var defPath;
		//defPath=(defaultPath.value ? object).asString;
		GUI.dialog.savePanel({arg argpath;
			this.path = argpath;
			this.performList(then,[path] ++ args);
		});
	}

	doSave {
		var clobber,vpath,evpath;
		vpath = path.value;
		if(File.exists(vpath),{
			evpath = vpath.escapeChar($ );
			("cp " ++ evpath + evpath ++ ".bak").unixCmd;
		});
		object.asCompileString.write(vpath);
	}
	doSaveAs {
		var clobber,vpath,evpath;
		vpath = path.value;
		if(File.exists(vpath),{
			evpath = vpath.escapeChar($ );
			("cp " ++ evpath + evpath ++ ".bak").unixCmd;
		});
		object.asCompileString.write(vpath);
		onSaveAs.value(vpath);
	}

	path_ { arg p;
		if(p.notNil,{
			path = PathName(p).asRelativePath
		})
	}
}

SoundFileFormats { // an interface

	var format='float32';

	gui { arg layout;
		var items;
		items = #['float32', 'aiff16', 'aiff24','wav16'];
		GUI.popUpMenu.new(layout,Rect(0,0,80,16))
			.items_(items)
			.action_({ arg pop;
				format = items.at(pop.value);
			})
			.background_(Color.red(0.4,0.3));
	}

	headerFormat {
		if(format=='float32',{
			^'Sun'
		});
		if(format=='aiff16',{
			^'AIFF'
		});
		if(format=='aiff24',{
			^'AIFF'
		});
		if(format=='wav16',{
			^'WAV'
		});
		^nil
	}

	sampleFormat {
		if(format=='float32',{
			^'float32'
		});
		if(format=='aiff16',{
			^'int16'
		});
		if(format=='aiff24',{
			^'int24'
		});
		if(format=='wav16',{
			^'int16'
		});
		^nil
	}
}

