
SampleGui : ObjectGui {
	
	var <sigG,lastSoundFilePath,maxs,mins,resolution = 128;
	var nameG,tempoG,beatsG;
	
	guiBody { arg layout;
		var xfade,r,size;

		layout = this.guify(layout,Rect(0,0,700,500),model.asString);
		nameG = ActionButton(layout,"pathname",{
			this.loadDialog
		},maxx:140).background_(Color.white);
		
		ActionButton(layout,"query",{
			if(model.buffer.notNil,{
				model.buffer.query;
			},{
				"Sample buffer not loaded".inform;
			})
		});
		
		CXLabel(layout,"bpm:");
		tempoG=NumberEditor(1,[0,1000])
					.action_({ arg th; model.tempo_(th.value / 60) });
		tempoG.smallGui(layout);
					
		CXLabel(layout,"beats:");
		beatsG=NumberEditor(1,[0.000001,32])
					.action_({arg th; model.beats_(th.value)});
		beatsG.smallGui(layout);

		ActionButton(layout,"reload buffer",{
			model.reloadBuffer;
		});

		//ActionButton(layout,"play",{model.play},maxx:70);
		//ActionButton(layout, "save...",{model.save},maxx:70).background_(Color.white);
		
			
		//ActionButton(layout,"selAll",{sigG.setSelection(0,model.size)},maxx:70);

		//sigG=SignalView(layout.view,r,model.signal.at(0));
		//SliderView(layout.view,layout.layRight(10,75,2),"",1,0,3.4,0).action_({arg th; sigG.zoom=(model.size/sigG.view.bounds.width) * th.value});
		//SliderView(layout.view,layout.layRight(10,75,2),"",1,0.0,1.0,0).action_({arg th; sigG.scroll=(model.size) * th.value});

		size = layout.asView.bounds.width - 50;
		sigG = SCMultiSliderView(layout, Rect(0, 0, size, 50));
		sigG.readOnly_(true);
		layout.startRow;

		SCSlider( layout, Rect(0, 0, size, 12))
			.action_({arg ex; sigG.setProperty(\xOffset, (ex.value * 4) + 1 )});
		sigG.thumbSize_(1); 
		sigG.gap_(0);
		sigG.colors_(Color.black, Color.grey(0.1,0.8));
		sigG.showIndex_(true);
		sigG.action = { arg xb;
			var start;
			model.startFrame = start = (xb.index * resolution);
			model.endFrame = (xb.selectionSize * resolution - start);
		};
		sigG.drawLines_(true);
		sigG.drawRects_(false);
		sigG.isFilled_(true);
		sigG.background_(Color.new255(248, 248, 255,170));

		
		this.drawWaveform;
		
		SCSlider( layout, Rect(0, 0, size, 12))
			.action_({arg ex; sigG.setProperty(\startIndex, ex.value * maxs.size )});
		

		//ActionButton(layout,"!normalize",{model.normalize});
		//ActionButton(layout,"< -fades- >",{model.inouts});
		
		//ActionButton(layout,"crop",{model.crop(sigG.selectionStart,sigG.selectionEnd)});

		
		this.update;		
	}
	update {
		nameG.label_(model.name).refresh;
		tempoG.value_(model.tempo*60).changed;
		beatsG.value_(model.beats).changed;

		if(model.soundFilePath !== lastSoundFilePath,{
			this.drawWaveform;
		});
	}
	drawWaveform {
		var file,c,minval=0,maxval=0;
		maxs = Array.new;
		mins = Array.new;
		
		lastSoundFilePath = model.soundFilePath;
		file = model.soundFile;
		file.openRead(lastSoundFilePath);
		file.numFrames;
		// what if its stereo ???
		c = FloatArray.newClear(file.numFrames);
		file.readData(c);

		file.close;
		
		c.do({arg fi, i;
			if(fi < minval, {minval = fi});
			if(fi > maxval, {maxval = fi});
		
			if(i % resolution == 0,{
				maxs = maxs.add((1 + maxval ) * 0.5 );
				mins = mins.add((1 + minval ) * 0.5 );
			
				minval = 0;
				maxval = 0;
			});
		});
		
		sigG.reference_(maxs); //this is used to draw the upper part of the table
		sigG.value_(mins);
		
		sigG.index_(model.startFrame / resolution);
		sigG.selectionSize_(model.size / resolution);

		//	sigG.zoom_(ceil(model.size  /  sigG.view.bounds.width ) * 3);
	}
/*
	selectionStart { ^sigG.selectionStart }
	selectionEnd {^sigG.selectionEnd }
	selectAll { 	sigG.setSelection(0,model.size) }
	setSelection {arg s,e; sigG.setSelection(s,e) }
	selectionSize { ^(sigG.selectionEnd - sigG.selectionStart) }
	selectedData { ^model.signal.collect({arg s; s.copyRange(sigG.selectionStart,sigG.selectionEnd) }) }
*/
	loadDialog {
		File.openDialog(nil,{ arg path;
			model.load(path).changed;
		});
	}
}

		
