
SampleGui : ObjectGui {
	
//	var <sigG;
	var nameG,tempoG,beatsG;
	
	guiBody { arg layout;
		var xfade,r;

		layout = this.guify(layout,"sample",700,500);
		//layout.startRow;
		nameG = ActionButton(layout,"pathname",{
			this.loadDialog
		},maxx:140).backColor_(Color.white);
		
		//ActionButton(layout,"play",{model.play},maxx:70);
		//ActionButton(layout, "save...",{model.save},maxx:70).backColor_(Color.white);
		
			
		//ActionButton(layout,"selAll",{sigG.setSelection(0,model.size)},maxx:70);
		//sigG=SignalView(layout.view,r,model.signal.at(0));
		//SliderView(layout.view,layout.layRight(10,75,2),"",1,0,3.4,0).action_({arg th; sigG.zoom=(model.size/sigG.view.bounds.width) * th.value});
		//SliderView(layout.view,layout.layRight(10,75,2),"",1,0.0,1.0,0).action_({arg th; sigG.scroll=(model.size) * th.value});

		//ActionButton(layout,"!normalize",{model.normalize});
		//ActionButton(layout,"< -fades- >",{model.inouts});
		
		//ActionButton(layout,"crop",{model.crop(sigG.selectionStart,sigG.selectionEnd)});
		CXLabel(layout,"bpm:");
		tempoG=NumberEditor(1,[0,1000])
					.action_({arg th; model.tempo_(th.value / 60)});
		tempoG.gui(layout);
					
		CXLabel(layout,"beats:");
		beatsG=NumberEditor(1,[0.000001,32])
					.action_({arg th; model.beats_(th.value)});
		beatsG.gui(layout);
		
		this.update;		
	}
	update {
		nameG.label_(model.name);
		tempoG.value_(model.tempo*60);
		beatsG.value_(model.beats);
		
	//	sigG.signal_(model.signal.at(0));
	//	sigG.zoom_(ceil(model.size  /  sigG.view.bounds.width ) * 3);  
	//	sigG.update;
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

		
