/*

s.boot;

a = Volume(s, 0, s.options.numOutputBusChannels);
a.play;

z = {SinOsc.ar(440, 0, 0.1)}.play

s.volume.volume_(-9);
s.volume.volume;
a.mute;
a.unmute;
a.volume_(0);

a.gui;

a.free;

z.free;

*/

Volume {
	var startBus, numChans, <min, <max, server, persist, <ampSynth, <>window, <volume, spec;
	var <lag, sdname, gui = false, <muteamp, cpFun, <isMuted=false;
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
					if (persist) {
						this.volume_(this.volume)
					} {
						this.free;
					};
				});
			};
			if (server.serverRunning.not) {
				this.volume_(this.volume)
			}
		}
	}

	play {arg mute = false;
		if (ampSynth.isNil) {
			if(server.serverRunning) {
				ampSynth = Synth.after(server.defaultGroup, sdname,
					[\volumeAmp, volume.dbamp, \volumeLag, lag, \bus, startBus]);
				mute.if({this.mute});
			} {
				"Volume only works on a running Server. Please boot".warn;
			}
		}
	}

	free {
		ampSynth.release;
		ampSynth = nil;
	}

	numChans { ^numChans ? synthNumChans ? server.options.numOutputBusChannels }
	numChans_ { |num|
		if(ampSynth.notNil and: { num != synthNumChans }) {
			"Change in number of channels will not take effect until volume is reset to 0dB.".warn;
		};
		numChans = num;
	}

	mute {
		isMuted = true;
		muteamp = volume;
		this.changed(\mute, true);
		if (ampSynth.notNil) {
			ampSynth.set(\volumeAmp, 0);
		} {
			this.playVolume(true)
		}
	}

	unmute {
		isMuted = false;
		ampSynth.set(\volumeAmp, muteamp.dbamp);
		this.changed(\mute, false);
		if (this.muteamp == 0.0) {
			this.free;
		}
	}

	// sets volume back to 1 - removes the synth
	reset {
		this.free;
	}

	// cleaner with MVC - in db
	volume_ { arg aVolume;
		volume = aVolume;
		if (volume == 0.0) {
			if (ampSynth.notNil and: {this.isMuted.not}) {
				this.free;
			}
		} {
			if (server.serverRunning) {
				this.playVolume(isMuted);
			}
		};
		volume = volume.clip(min, max);
		if(isMuted) { muteamp = volume };
		if(ampSynth.notNil && isMuted.not) { ampSynth.set(\volumeAmp, volume.dbamp) };
		this.changed(\amp, volume);
	}

	playVolume { arg muted = false;
		if (ampSynth.isNil and: {
			(volume != 0.0) or: {muted} }) {
			this.play(muted);
		}
	}

	lag_ { arg aLagTime;
		lag = aLagTime;
		ampSynth.set(\volumeLag, lag);
	}

	setVolumeRange { arg argMin, argMax;
		argMin !? { min = argMin };
		argMax !? { max = argMax };
		this.changed(\ampRange, min, max);
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
