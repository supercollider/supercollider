+SynthDesc {

	makeWindow{
		^this.makeGui;
	}

	makeGui {
		var w, s, startButton, sliders;
		var id, cmdPeriodFunc;
		var usefulControls, numControls;
		var getSliderValues, gui;

		s = Server.default;

		gui = GUI.current;

		usefulControls = controls.select {|controlName, i|
			var ctlname;
			ctlname = controlName.name.asString;
			(ctlname != "?") && (msgFuncKeepGate or: { ctlname != "gate" })
		};

		numControls = usefulControls.size;
		sliders = Array.newClear(numControls);

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
				var ctlname;
				ctlname = controlName.name;
				if(ctlname[1] == $_ and: { "ti".includes(ctlname[0]) }) {
					ctlname = ctlname[2..];
				};
				envir.put(ctlname.asSymbol, sliders[i].value);
			};
			envir.use {
				msgFunc.valueEnvir
			};
		};


		startButton.action = {|view|
			if (view.value == 1) {
				// start sound
				if(id.isNil) { id = s.nextNodeID };
				s.sendBundle(s.latency, ["/s_new", name, id, 0, 1] ++ getSliderValues.value);
				OSCpathResponder(s.addr, ['/n_end', id], { |time, resp, msg|
						// if the synth has a long release, a new node might be playing
					if(id.isNil or: { id == msg[1] }) {
						defer { startButton.value = 0 };
					};
					resp.remove;
					id = nil;
				}).add;
			};
			if (view.value == 0) {
				if (this.hasGate) {
					// set gate to zero to cause envelope to release
					s.sendMsg("/n_set", id, "gate", 0);
				}{
					s.sendMsg("/n_free", id);
				};
				id = nil;
			};
		};

		// create controls for all parameters
		usefulControls.do {|controlName, i|
			var ctlname, ctlname2, capname, spec;
			ctlname = controlName.name;
			capname = ctlname.copy;
			capname[0] = capname[0].toUpper;
			w.view.decorator.nextLine;
			ctlname = ctlname.asSymbol;
			if((spec = metadata.tryPerform(\at, \specs).tryPerform(\at, ctlname)).notNil) {
				spec = spec.asSpec
			} {
				spec = ctlname.asSpec;
			};
			if (spec.notNil) {
				sliders[i] = gui.ezSlider.new(w, 400 @ 24, capname, spec,
					{ |ez|
						if(id.notNil) { s.sendMsg("/n_set", id, ctlname, ez.value) }
					}, controlName.defaultValue);
			}{
				spec = ControlSpec(-1e10,1e10);
				sliders[i] = gui.ezNumber.new(w, 400 @ 24, capname, spec,
					{ |ez|
						if(id.notNil) { s.sendMsg("/n_set", id, ctlname, ez.value) }
					}, controlName.defaultValue);
			};
		};


		// set start button to zero upon a cmd-period
		cmdPeriodFunc = { startButton.value = 0; };
		CmdPeriod.add(cmdPeriodFunc);

		// stop the sound when window closes and remove cmdPeriodFunc.
		w.onClose = {
			if(id.notNil) {
				s.sendMsg("/n_free", id);
			};
			CmdPeriod.remove(cmdPeriodFunc);
		};

		^w.front; // make window visible and front window.
	}
}

