Volume {
	var startBus, numChans, <min, <max, server, persist, <ampSynth, <>window, <volume, spec;
	var <lag, sdname, gui = false, cpFun, updateSynth, <isMuted=false;
	var <synthNumChans;	// the actual number of channels, which might be set automatically
	var sdInitialized;

	*new { arg server, startBus = 0, numChans, min = -90, max = 6, persist = false;
		^super.newCopyArgs(startBus, numChans, min, max, server, persist).initVolume;
	}

	initVolume {
		var cond;
		server = server ?? {Server.default};
		volume = 0;
		lag = 0.1;
		gui = false;
		updateSynth = {
			var amp = if (isMuted.not) { volume.dbamp } { 0.0 };
			var active = amp != 1.0;
			if (active) {
				if(server.serverRunning) {
					if (ampSynth.isNil) {
						ampSynth = Synth.after(server.defaultGroup, sdname,
							[\volumeAmp, amp, \volumeLag, lag, \bus, startBus]);
					}{
						ampSynth.set(\volumeAmp, amp);
					}
				}
			}{
				if (ampSynth.notNil) { ampSynth.release; ampSynth = nil }
			}
		};

		sdInitialized = Condition();
		if (server.serverRunning) {
			this.sendSynthDef
		} {
			ServerBoot.add ({
				ampSynth = nil;
				this.sendSynthDef;
			}, server)
		};
	}

	sendSynthDef {
		if(numChans.isNil) {
			synthNumChans = server.options.numOutputBusChannels;
		} {
			synthNumChans = numChans;
		};

		sdname = (\volumeAmpControl ++ synthNumChans).asSymbol;

		sdInitialized = Condition();

		SynthDef(sdname,
			{ arg volumeAmp = 1, volumeLag = 0.1, gate=1, bus;
				XOut.ar(bus,
					Linen.kr(gate, releaseTime: 0.05, doneAction:2),
					In.ar(bus, synthNumChans) * Lag.kr(volumeAmp, volumeLag) );
		}).send(server);

		fork {
			server.sync(sdInitialized);
			if (cpFun.isNil) {
				ServerTree.add(cpFun = {
					ampSynth = nil;
					if (persist) { updateSynth.value; }
				});
			};
		}
	}

	numChans { ^numChans ? synthNumChans ? server.options.numOutputBusChannels }
	numChans_ { |num|
		if(ampSynth.notNil and: { num != synthNumChans }) {
			"Change in number of channels will not take effect until volume is reset to 0dB.".warn;
		};
		numChans = num;
	}

	mute {
		if (isMuted.not) {
			isMuted = true;
			this.changed(\mute, true);
			updateSynth.value;
		}
	}

	unmute {
		if (isMuted) {
			isMuted = false;
			this.changed(\mute, false);
			updateSynth.value;
		}
	}

	// sets volume back to 1 - removes the synth
	reset {
		isMuted = false;
		volume = 0.0;
		updateSynth.value;
	}

	// cleaner with MVC - in db
	volume_ { arg aVolume;
		volume = aVolume.clip(min, max);
		this.changed(\amp, volume);
		updateSynth.value;
	}

	lag_ { arg aLagTime;
		lag = aLagTime;
		if (ampSynth.notNil) { ampSynth.set(\volumeLag, lag) };
	}

	setVolumeRange { arg argMin, argMax;
		var clippedVolume;
		argMin !? { min = argMin };
		argMax !? { max = argMax };
		this.changed(\ampRange, min, max);
		clippedVolume = volume.clip(min, max);
		if (clippedVolume != volume) { this.volume_(clippedVolume) }
	}


	gui { arg window, bounds;
		//		this.debug(\gui);
		^VolumeGui(this, window, bounds)
	}

	close {
		window.close;
	}
}

VolumeGui{
	var <>model;
	var window, spec, slider, box, simpleController;

	*new{|model, win, bounds|
		^super.new.model_(model).init(win, bounds)
	}

	init{|win, bounds|
		spec = [model.min, model.max, \db].asSpec;
		bounds = bounds ?? {Rect(100, 100, 80, 330)};
		window = win ?? {GUI.window.new("Volume", bounds).front};
		box = GUI.numberBox.new(window, Rect(10, 10, 60, 30))
		.value_(model.volume) ;
		slider = GUI.slider.new(window, Rect(10, 40, 60, 280))
		.value_(spec.unmap(model.volume)) ;
		slider.action_({ arg item ;
			model.volume_(spec.map(item.value));
		}) ;
		box.action_({ arg item ;
			model.volume_(item.value) ;
		}) ;
		window.onClose_({
			simpleController.remove;
		});
		simpleController = SimpleController(model)
		.put(\amp, {|changer, what, volume|
			this.debug(volume);
			box.value_(volume.round(0.01)) ;
			slider.value_(spec.unmap(volume)) ;
		})
		.put(\ampRange, {|changer, what, min, max|
			spec = [min, max, \db].asSpec.debug;
			slider.value_(spec.unmap(model.volume)) ;
		})
	}
}
