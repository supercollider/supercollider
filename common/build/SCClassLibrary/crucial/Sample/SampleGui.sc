
// new SampleGui by blackrain@spymac.com
// thanks!

SampleGui : ObjectGui {

	var lastSoundFilePath;
	var nameG,tempoG,beatsG;

	var v, d, divs, block = 64;
	var zin, zout, zyin, zyout, lScroll, rScroll, selectionStartG, selectionSizeG;

	writeName { arg layout;
		this.prWriteName(layout,model.class.asString);
	}
	guiBody { arg layout;
		var xfade, r, size,absBounds;

		divs = 1;
		zout = 1.1; zin = zout.reciprocal;
		zyout = 1.1; zyin = zyout.reciprocal;
		rScroll = 0.01; lScroll = rScroll * -1;

		nameG = ActionButton(layout,"load file...",{
			this.loadDialog
		},minWidth:140).background_(Color.white);

		ActionButton(layout,"query buffer",{
			if(model.buffer.notNil,{
				model.buffer.query;
			},{
				"Sample buffer not loaded".inform;
			})
		});

		CXLabel(layout,"bpm:");
		tempoG=NumberEditor(model.tempo * 60.0,[0,1000])
					.action_({ arg th; model.tempo_(th.value / 60).changed(this) });
		tempoG.smallGui(layout);

		CXLabel(layout,"beats:");
		beatsG=NumberEditor(model.beats,[0.000001,32])
					.action_({arg th; model.beats_(th.value).changed(this) });
		beatsG.smallGui(layout);

		ActionButton(layout,">",{model.play},minWidth:70).background_(Color.green(alpha:0.5));

		size = min(800,layout.asView.bounds.width - 220);
		layout.startRow;

		// sound file view is still broken in relative views
		//absBounds = Rect(0, 0, size, 80);
		//absBounds = absBounds.moveToPoint(layout.absoluteBounds.origin);
		//absBounds = absBounds.moveBy(0,layout.decorator.top);
		v = GUI.soundFileView.new(layout, Rect(0,0,4,4)) //Rect(0, 0, size, 80)
			.canFocus_(true).gridOn_(true).timeCursorOn_(false);
		d = GUI.slider2D.new(layout, size@30)
			.action_({|sl|
				var y;
				v.zoomToFrac(y = sl.y * 0.9 + 0.1);
				v.scrollTo(sl.x);
				//d.thumbSize = size * y;
			});

		layout.startRow;
		CXLabel(layout,"Resolution:");
		block = max(1,(model.numFrames / 44100).asInteger);
		GUI.numberBox.new(layout, Rect(0, 0, 30, 18)).value_(block)
			.action_({|numbox|
				numbox.value = max(numbox.value,1);
				if (numbox.value != block, {
					block = numbox.value;
					if(model.soundFilePath.notNil, {
						v.soundfile = SoundFile.new(model.soundFilePath);
						v.readWithTask( block: block);
					})
				})

			});
		CXLabel(layout,"divs/beat:");
		GUI.numberBox.new(layout, Rect(0, 0, 30, 18)).value_(divs)
			.action_({|numbox|
				numbox.value = max(numbox.value,1);
				divs = numbox.value;
				v.gridResolution_(60/tempoG.value/divs);
			});
		GUI.staticText.new(layout, Rect(0, 0, 50, 18)).string_("Start:").align_(\right);
		selectionStartG = GUI.staticText.new(layout, Rect(0, 0, 100, 18));
		GUI.staticText.new(layout, Rect(10, 0, 30, 18)).string_("Size:").align_(\right);
		selectionSizeG = GUI.staticText.new(layout, Rect(0, 0, 100, 18));

		v.mouseUpAction = {
			selectionStartG.string = v.selectionStart(0);
			selectionSizeG.string = v.selectionSize(0)
		};
		//v.keyDownAction = this.keyDownResponder;
		v.keyDownAction = { arg ascii, char;
			case
				{char === $+} { v.zoom(zin) }		// zoom in
				{char === $-} { v.zoom(zout) }		// zoom out
				{char === $'} { v.yZoom = v.yZoom * zyin } // zoom in on y
				{char === $;} { v.yZoom = v.yZoom * zyout } // zoom out on y
				{char === $/} { v.yZoom = 1 }		// zoom 1 on y
				{char === $=} { v.zoomAllOut }		// view all - zoom all out
				{char === $f} { v.zoomSelection(0) }	// fit selection to view
				{char === $.} { v.scroll(rScroll) }	// scroll right ->
				{char === $,} { v.scroll(lScroll) }	// scroll left <-
				{char === $s} { v.scrollToStart }	// scroll to buffer start
				{char === $e} { v.scrollToEnd }		// scroll to buffer end
				{char === $a} { v.selectAll(0) }		// select all
				{char === $n} { v.selectNone(0) };	// select none

			d.value = v.scrollPos;	// update scrollbar position
			selectionStartG.string = v.selectionStart(0);
			selectionSizeG.string = v.selectionSize(0)
		};

		this.update;
	}
	update { arg changed,changer;
		tempoG.value_(model.tempo*60).changed;
		beatsG.value_(model.beats).changed;
		if(model.soundFilePath !== lastSoundFilePath,{
			nameG.label_(model.name).refresh;

			if(model.soundFilePath.isNil, { ^this });
			lastSoundFilePath = model.soundFilePath;

			v.soundfile = SoundFile.new(lastSoundFilePath);
			v.readWithTask( block: block, doneAction: { v.gridResolution_(60/tempoG.value/divs) } );
		});
	}
	updateSelection {
		//d.x = v.scrollPos;	// update scrollbar position
		//d.y = v.xZoom;
		selectionStartG.string = v.selectionStart(0);
		selectionSizeG.string = v.selectionSize(0)
	}
	/*keyDownResponder {
		// grrrr. shift.
		var kdr;
		kdr = UnicodeResponder.new;
		kdr.normal(
			$+ -> { v.zoom(zin); this.updateScroll; },		// zoom in
			$- -> { v.zoom(zout); this.updateScroll; },		// zoom out
			$' -> { v.yZoom = v.yZoom * zyin; this.updateScroll; }, // zoom in on y
			$; -> { v.yZoom = v.yZoom * zyout;  this.updateScroll; }, // zoom out on y
			$/ -> { v.yZoom = 1;  this.updateScroll; },		// zoom 1 on y
			$= -> { v.zoomAllOut;  this.updateScroll; },		// view all - zoom all out
			$f -> { v.zoomSelection(0);  this.updateScroll; },	// fit selection to view
			$. -> { v.scroll(rScroll);  this.updateScroll; },	// scroll right ->
			$, -> { v.scroll(lScroll);  this.updateScroll; },	// scroll left <-
			$s -> { v.scrollToStart;  this.updateScroll; },	// scroll to buffer start
			$e -> { v.scrollToEnd;  this.updateScroll; },		// scroll to buffer end
			$a -> { v.selectAll(0);  this.updateScroll; },		// select all
			$n -> { v.selectNone(0);  this.updateScroll; });	// select none
		^kdr
	}*/

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
			model.load(path);
			model.changed;
		});
	}
}

/*

WavetableSampleGui : SampleGui {

	guiBody { arg layout;
		//layout.within(100,75,{arg layout;
			//nameG = CXLabel(layout,model.name);
			/ *ActionButton(layout,">",{
				if(Synth.isPlaying,{ Synth.stop },{
					{
						Osc.ar(model.wavetable,440,mul: 0.1)
					}.scope
				})
			});* /
			ActionButton(layout,"saveAs",{this.saveAs}).background_(Color.white);
			ActionButton(layout,"save",{this.save}).background_(Color.white);
		//});

		//sigG=WavetableView(layout.window,layout.layRight(250,75),model.wavetable);

		//ActionButton(layout,"editAsSample",{model.editAsSample});
//		layout.removeOnClose(Updater(this,{
//			nameG.label_("S:" ++ name);
//			sigG.wavetable_(signal);
//			sigG.update;
//		}));
	}
	save {
		if(model.wavetablePath.isNil,{ ^this.saveAs });
		model.write;
	}
	saveAs {
		GetStringDialog("wavetable name...",model.name,{ arg ok,name;
			var newpath;
			if(ok,{
				newpath = Sample.standardizePath(model.class.dir ++ name);
				model.write(newpath);
				nameG.label = name;
			});
		});
//		PutFileDialog("save wavetable...",model.name,{ arg ok,path;
//			if(ok,{
//				model.write(path);
//			})
//		})
	}

}

*/

