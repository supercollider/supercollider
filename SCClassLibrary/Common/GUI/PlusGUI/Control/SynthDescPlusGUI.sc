+SynthDesc {
	makeWindow{
		if (Platform.makeSynthDescWindowAction.notNil) {
			^Platform.makeSynthDescWindowAction.value(this)
		} {
			^this.makeGui;
		}
	}

	makeGui {
		var w, startButton, sliders;
		var synth, cmdPeriodFunc;
		var usefulControls, numControls;
		var getSliderValues, gui;

		gui = GUI.current;

		usefulControls = controls.select {|controlName, i|
			var ctlname;
			ctlname = controlName.name.asString;
			(ctlname != "?") && (msgFuncKeepGate or: { ctlname != "gate" })
		};

		numControls = usefulControls.collect(_.defaultValue).flatten.size;
		sliders = IdentityDictionary(numControls);

		// make the window
		w = gui.window.new("another control panel", Rect(20, 400, 440, numControls * 28 + 32));
		w.view.decorator = FlowLayout(w.view.bounds);

		w.view.background = Color.rand(0.5, 1.0);

		// add a button to start and stop the sound.
		startButton = gui.button.new(w, 75 @ 24);
		startButton.states = [
			["Start", Color.black, Color.green],
			["Stop", Color.white, Color.red]
		];

		getSliderValues = {
			var envir;

			envir = ();
			usefulControls.do {|controlName, i|
				var ctlname = controlName.name.asString;
				var sliderEntry = sliders[controlName.name];
				if(ctlname[1] == $_ and: { "ti".includes(ctlname[0]) }) {
					ctlname = ctlname[2..];
				};

				if (sliderEntry.isArray) {
					envir.put(controlName.name, sliderEntry.collect(_.value));
				} {
					envir.put(controlName.name, sliderEntry.value);
				}
			};
			envir.use {
				msgFunc.valueEnvir
			};
		};


		startButton.action = { |view|
			var synthEndWatcher, localSynth;
			if (view.value == 1) {
				synth = Synth(name, getSliderValues.value).register;
				localSynth = synth;
				synthEndWatcher = SimpleController(synth).put(\n_end, {
					synthEndWatcher.remove;
					// 'synth' may have changed in the interim --
					// clear 'synth' only if n_end is for the current synth
					if(localSynth === synth) {
						synth = nil;
						defer { view.value = 0 };
					};
				});
			} {
				if(synth.isPlaying) {
					if (this.hasGate) {
						synth.release;
					} {
						synth.free
					};
				};
			};
		};

		// create controls for all parameters
		usefulControls.do {|controlName|
			var ctlname, ctlname2, capname, spec, controlIndex, slider;
			ctlname = controlName.name;
			capname = ctlname.asString;
			capname[0] = capname[0].toUpper;
			w.view.decorator.nextLine;
			ctlname = ctlname.asSymbol;
			if((spec = metadata.tryPerform(\at, \specs).tryPerform(\at, ctlname)).notNil) {
				spec = spec.asSpec
			} {
				spec = ctlname.asSpec;
			};

			if (spec.isKindOf(ControlSpec)) {
				slider = EZSlider(w, 400 @ 24, capname, spec,
					{ |ez|
						if(synth.isPlaying) { synth.set(ctlname, ez.value) }
					}, controlName.defaultValue);
			} {
				spec = ControlSpec(-1e10, 1e10);
				if (controlName.defaultValue.isNumber) {
					slider = EZNumber(w, 400 @ 24, capname, spec,
						{ |ez|
							if(synth.isPlaying) { synth.set(ctlname, ez.value) }
						}, controlName.defaultValue)
				} {
					slider = Array(controlName.defaultValue.size);
					controlName.defaultValue.do {|value, i|
						slider.add(EZNumber(w, 96 @ 24, "%[%]".format(capname, i), spec,
							{ |ez|
								if(synth.isPlaying) { synth.set(controlName.index + i, ez.value) }
							}, value))
					}
				}
			};
			sliders.put(ctlname, slider)
		};

		// set start button to zero upon a cmd-period
		cmdPeriodFunc = { startButton.value = 0; };
		CmdPeriod.add(cmdPeriodFunc);

		// stop the sound when window closes and remove cmdPeriodFunc.
		w.onClose = {
			synth.release;
			CmdPeriod.remove(cmdPeriodFunc);
		};

		^w.front; // make window visible and front window.
	}
}
