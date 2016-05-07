// thor magnusson (2007)

// Example showing how SCScope can be used in a UI

GUI.cocoa;	// use Mac OS X native GUI
GUI.swing;	// use Java GUI

(
s.waitForBoot({
	var sc;
	b = Buffer.alloc(s,2048,2);
	a = SynthDef(\scopeExample, { arg freq=400, rate=2, amp=0.4, pan=0;
		var signal, scope;
		signal = [LFSaw.ar(freq, 0, amp/2)*SinOsc.ar(rate),
				Pulse.ar( freq, 0.51, amp)*SinOsc.ar(rate)];
		signal = Balance2.ar(signal[0], signal[1], pan);
		scope  = if( GUI.id === \cocoa, \ScopeOut, \JScopeOut ).asClass;
		scope.ar( signal, b );
		Out.ar(0, signal);
	}).play(s);

	w = Window("scope in a gui", Rect(100, 400, 400, 300))
		.onClose_({a.free;}) // free synth on closing window
		.front;

	MultiSliderView(w, Rect(10, 10, 90, 120))
		.value_([0.4, 0.5, 0.6, 0.5])
		.indexIsHorizontal_(false)
		.isFilled_(true)
		.strokeColor_(Color.new255(10, 55, 10))
		.fillColor_(Color.new255(110, 155, 110).alpha_(0.6))
		.indexThumbSize_(26)
		.gap_(4)
		.valueThumbSize_(1)
		.action_({|sl|
			sl.index.switch
				{0} { a.set(\freq, 400+(sl.value[sl.index]*400)) }
				{1} { a.set(\rate, (sl.value[sl.index]*10)) }
				{2} { a.set(\amp, sl.value[sl.index]) }
				{3} { a.set(\pan, (sl.value[sl.index]*2)-1) };
		});

	StaticText(w, Rect(14, 4, 90, 30))
		.string_("Freq");
	StaticText(w, Rect(14, 34, 90, 30))
		.string_("Rate");
	StaticText(w, Rect(14, 64, 90, 30))
		.string_("Amp");
	StaticText(w, Rect(14, 94, 90, 30))
		.string_("Pan");

	StaticText(w, Rect(10, 140, 90, 16))
		.string_("xZoom:");

	Slider(w, Rect(10, 160, 90, 24))
		.action_({|sl| sc.xZoom_(sl.value*4)});

	sc = ScopeView(w, Rect(120,10,260,260))
		.bufnum_(b.bufnum)
		.background_(Color.white)
		.resize_(5)
		.waveColors_([Color.black, Color.black]);
});
)
