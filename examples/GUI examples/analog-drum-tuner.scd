// Ringz tuner - h. james harkins, http://www.dewdrop-world.net
// Makes analog-style drum sounds with a bank of tunable Ringz filters
// code is released under the LGPL, http://creativecommons.org/licenses/LGPL/2.1/

// optional
// Server.default = s = Server.internal;

// double-click inside the parenthesis, then hit enter to evaluate

(
var	percBus, percGroup, ringzBus, ringzGroup, trigbus, trigsynth, src, toStereo,
levelBus, levelEditor, toStereoOut,
nodes, ffreqs, rqs, decays, amps,
log001, sr, dt,
freqspec, rqspec, ampspec,
noiseString = "PinkNoise.ar", envString = "Env.perc(0.01, 0.2)",

post, buildSynthDef, currentDef, g, stop,

window, leftview, rightview, ffreqview, rqview, ampview, rateEdit, newButton,
funcedit, envedit, helpview;

s.waitForBoot({
	{
		// preparation
		percBus = Bus.audio(s, 1);
		ringzBus = Bus.audio(s, 1);
		percGroup = Group.new;
		ringzGroup = Group.after(percGroup);
		levelBus = Bus.control(s, 1).set(0.75);
		toStereoOut = SynthDef('1to2', { |bus, level|
			Out.ar(0, (In.ar(bus, 1) * level) ! 2)
		}).play(ringzGroup, [bus: ringzBus, level: levelBus.asMap], addAction: \addAfter);

		SynthDef(\ringz, { |ffreq = 20, decay = 0.01, in, out, amp = 0|
			Out.ar(out, Ringz.ar(In.ar(in, 1), ffreq, decay, amp))
		}).send(s);

		nodes = Array.new(10);
		ffreqs = [];
		rqs = [];
		amps = [];
		decays = dt.(ffreqs, rqs);
		log001 = log(0.001);	// constant, will be used repeatedly

		// func to calc decay time from freq and rq
		sr = s.sampleRate;
		dt = { |ffreq, rq|
			(log001 / log(1 - (pi/sr * ffreq * rq))) / sr;
		};
		freqspec = \freq.asSpec;
		rqspec = [1, 0.001, \exp].asSpec;
		ampspec = \amp.asSpec;

		// gui building
		window = PageLayout("Ringz tuner", Rect(150, 5, 800, 570));
		leftview = FlowView(window, 470@490);
		rightview = FlowView(window, 280@490);

		// left-hand side: filters
		GUI.staticText.new(leftview, 150@20).string_("frequencies");
		GUI.staticText.new(leftview, 150@20).string_("resonances");
		GUI.staticText.new(leftview, 150@20).string_("amplitudes");
		ffreqview = GUI.multiSliderView.new(leftview, 150@200)
		.value_(freqspec.unmap(ffreqs))
		.thumbSize_(14)
		.action_({ |v|
			ffreqs = freqspec.map(v.value);
			decays = dt.(ffreqs, rqs);
			nodes.do({ |n, i|
				n.set(\ffreq, ffreqs[i], \decay, decays[i])
			})
		});
		rqview = GUI.multiSliderView.new(leftview, 150@200)
		.value_(rqspec.unmap(rqs))
		.thumbSize_(14)
		.action_({ |v|
			rqs = rqspec.map(v.value);
			decays = dt.(ffreqs, rqs);
			nodes.do({ |n, i|
				n.set(\decay, decays[i]);
			})
		});
		ampview = GUI.multiSliderView.new(leftview, 150@200)
		.value_(ampspec.unmap(amps))
		.thumbSize_(14)
		.action_({ |v|
			amps = ampspec.map(v.value);
			nodes.do({ |n, i| n.set(\amp, amps[i]) })
		});

		leftview.startRow;

		rateEdit = EZSlider(leftview, 220@20, "trigrate", #[0.5, 10], { |view| trigsynth.set(\trigrate, view.value) }, 2);
		levelEditor = EZSlider(leftview, 220@20, "volume", \amp, { |view| levelBus.set(view.value) }, 0.75);

		// ActionButtons -- add filter, post parameters, stop, save, load
		post = {
			(nodes.size > 0).if({
				postf("\nKlank array:\n`[ %,\n   %,\n   % ]\n\n",
					ffreqs, //[..nodes.size-1],
					amps, // \amp.asSpec.map(ampview.value[..nodes.size-1]),
					decays); // [..nodes.size-1];
				[ffreqs, decays, amps]
				.flop/*[..nodes.size-1]*/.do({ |item|
					postf("[\\ffreq, %, \\decay, %, \\amp, %]\n", *item);
				});
			});
		};

		leftview.startRow;
		newButton = Button(leftview, 100@20)
		.states_([["new resonz"]])
		.action_({
			if(nodes.size < 10) {
				ffreqs = ffreqs.add(20);
				rqs = rqs.add(1);
				decays = dt.(ffreqs, rqs);
				amps = amps.add(0);
				ffreqview.value = freqspec.unmap(ffreqs);
				rqview.value = rqspec.unmap(rqs);
				ampview.value = ampspec.unmap(amps);
				//		nodes.add(ringzMixer.play(\ringz, [\in, percMixer.inbus.index]));
				nodes.add(Synth(\ringz, [\in, percBus, \out, ringzBus], ringzGroup,
					addAction: \addToTail));
			};
		});

		Button(leftview, 140@20)
		.states_([["post parameters"]])
		.action_(post);

		stop = {
			post.value;
			toStereoOut.free;
			src.free;
			nodes.do(_.free);
			trigbus.free;
			percBus.free; percGroup.free;
			ringzBus.free; ringzGroup.free;
			levelBus.free;
			//	percMixer.free; ringzMixer.free;
			if(window.isClosed.not) { window.onClose_(nil).close };
		};

		Button(leftview, 80@20)
		.states_([["stop"]])
		.action_(stop);

		leftview.startRow;

		Button(leftview, 80@20)
		.states_([["save"]])
		.action_({
			GUI.dialog.savePanel({ |path|
				[ffreqview.value, rqview.value, ampview.value,
					noiseString, envString, nodes.size]
				.writeTextArchive(path)
			});
		});

		Button(leftview, 80@20)
		.states_([["load"]])
		.action_({
			var	values;
			File.openDialog("", { |path|
				values = Object.readTextArchive(path[0]);
				(values.size != 6).if({
					Error("File does not contain a ringz spec.").throw;
					}, {
						noiseString = values[3];
						funcedit.setString(noiseString, 0, funcedit.string.size);
						envString = values[4];
						envedit.setString(envString, 0, envedit.string.size);
						buildSynthDef.value;
						ffreqview.value = values[0];
						rqview.value = values[1];
						ampview.value = values[2];
						nodes.do(_.free);
						nodes = { |i|
							Synth(\ringz, [\in, percBus, \out, ringzBus], ringzGroup,
								addAction: \addToTail);
							//				ringzMixer.play(\ringz, [\in, percMixer.inbus.index]);
						} ! values[5];	// make the nodes
						{	ffreqview.doAction;
							rqview.doAction;
							ampview.doAction;
						}.defer(0.2);
				});
			});
		});


		// dynamic synthdef construction based on user input
		GUI.staticText.new(rightview, 100@20).string_("Noise function:");
		Button(rightview, 80@20)
		.states_([["evaluate"]])
		.action_({ |view|
			noiseString = funcedit.string;
			buildSynthDef.value;
		});
		rightview.startRow;
		funcedit = GUI.textView.new(rightview, 275@200)
		.string_(noiseString);

		rightview.startRow;
		GUI.staticText.new(rightview, 100@20).string_("Envelope:");
		Button(rightview, 80@20)
		.states_([["evaluate"]])
		.action_({ |view|
			envString = envedit.string;
			src.setn(\env, envString.interpret.asArray);
		});
		rightview.startRow;
		envedit = GUI.textView.new(rightview, 275@200)
		.string_("Env.perc(0.01, 0.2)");

		trigbus = Bus.control(s, 1);
		{	trigsynth = SynthDef(\percTrig, { |trigrate, out|
			Out.kr(out, Impulse.kr(trigrate))
			}).play(percGroup, [\trigrate, rateEdit.value, \out, trigbus], addAction: \addToHead);
		}.defer(0.2);

		buildSynthDef = {
			var	func, env;
			currentDef.notNil.if({
				s.sendMsg(\d_free, currentDef.name);
			});
			func = ("{ " ++ noiseString ++ " }").interpret;
			env = envString.interpret;
			currentDef = SynthDef(\source, { |trigbus, outbus|
				var	trig, sig, envCtl;
				trig = In.kr(trigbus, 1);
				//		Out.kr(trigbus, trig);
				envCtl = Control.names(\env).kr(0 ! 100);	// allow 25 env nodes maximum
				sig = SynthDef.wrap(func, nil, [trig]) * EnvGen.ar(envCtl, trig);
				Out.ar(outbus, sig)
			}).send(s);
			{	src.free;
				src = Synth(\source, [\trigbus, trigbus, \outbus, percBus], percGroup,
					addAction: \addToTail);
				src.setn(\env, env.asArray);
			}.defer(0.1);
		};

		helpview = GUI.textView.new(leftview, 458@161);
		if(GUI.scheme.name == 'CocoaGUI') {
			helpview.font = GUI.font.new("Helvetica", 12);
		};
		helpview.string = "Analog percussion tuner by James Harkins

Move the black knobs up and down to change the resonator characteristics.

The noise function and envelope control the exciter -- change the code and click 'evaluate' to hear it. Click 'new resonz' to add another filter (up to 10). Filters run in parallel, not in series.

The 'save' button stores the specs into a file; 'load' returns the editor to a saved state. 'stop' closes the window and removes all synths.";
		helpview.editable = false;

		window.onClose = stop;

		0.5.wait;

		buildSynthDef.value;
		newButton.doAction;
		window.front;
	}.fork(AppClock);
});
)
