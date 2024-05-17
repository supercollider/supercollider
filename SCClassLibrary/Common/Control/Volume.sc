Volume {

	var <server, <startBus, numChannels, <min, <max, <persist;

	var <volume = 0.0, <lag = 0.1, <isMuted = false;

	var <ampSynth, <numOutputChannels, <defName, updateFunc, initFunc;
	var <>window;
	var <group, baseAmpSynth;

	*new { | server, startBus = 0, numChannels, min = -90, max = 6, persist = false |
		^super.newCopyArgs(server ?? { Server.default }, startBus, numChannels, min, max, persist).init;
	}

	init {
		// group nodeID should always be 2, so multiple clients can use it
		group = Group.basicNew(server, server.nextPermNodeID);
		defName = (\volumeAmpControl ++ this.numChannels).asSymbol;
		// synth nodeID should always be 3, so multiple clients can use it
		baseAmpSynth = Synth.basicNew(defName, server, server.nextPermNodeID);
		baseAmpSynth.group = group;

		// execute immediately if we're already past server tree functions
		if(server.serverRunning) {
			this.sendSynthDef;
			this.updateSynth;
		};

		initFunc = {
			ampSynth = nil;
			server.sendMsg(*group.newMsg(RootNode(server), \addToTail));
			this.sendSynthDef;

			// only create synth now if it won't be created by ServerTree
			if (persist.not) { this.updateSynth };
		};
		ServerBoot.add(initFunc, server);

		updateFunc = {
			ampSynth = nil;
			if(persist) { this.updateSynth }
		};

		ServerTree.add(updateFunc, server);

	}

	sendSynthDef {
		if (server.hasBooted) {
			fork {
				numOutputChannels = this.numChannels;
				// update defName in case numChannels has changed
				defName = (\volumeAmpControl ++ numOutputChannels).asSymbol;
				baseAmpSynth.defName = defName;

				SynthDef(defName, { | volumeAmp = 1, volumeLag = 0.1, gate=1, bus |
					XOut.ar(bus,
						Linen.kr(gate, releaseTime: 0.05, doneAction:2),
						In.ar(bus, numOutputChannels) * Lag.kr(volumeAmp, volumeLag)
					);
				}).send(server);

				server.sync;

			}
		};
	}

	numChannels { ^numChannels ? server.options.numOutputBusChannels }
	numChannels_ { |num|
		if(ampSynth.notNil and: { num != this.numChannels }) {
			"Change in number of channels will not take effect until volume is reset to 0dB.".warn;
		};
		numChannels = num;
	}

	updateSynth {
		var amp = if(isMuted.not) { volume.dbamp } { 0.0 };
		var active = amp != 1.0;

		if(active) {
			if(server.hasBooted) {
				if(ampSynth.isNil) {
					// Synth.after(server.defaultGroup, defName,
					// [\volumeAmp, amp, \volumeLag, lag, \bus, startBus])

					ampSynth = baseAmpSynth;

					server.sendMsg(*group.newMsg(RootNode(server), \addToTail));
					server.sendMsg(9, //"s_new"
						defName, baseAmpSynth.nodeID,
						Node.addActions[\addToTail],
						group.nodeID,
						*[\volumeAmp, amp, \volumeLag, lag, \bus, startBus]
					);

				} {
					ampSynth.set(\volumeAmp, amp);
				}
			}
		} {
			if(ampSynth.notNil) { ampSynth.release; ampSynth = nil }
		}
	}

	mute {
		if(isMuted.not) {
			isMuted = true;
			this.changed(\mute, true);
			this.updateSynth;
		}
	}

	unmute {
		if(isMuted) {
			isMuted = false;
			this.changed(\mute, false);
			this.updateSynth;
		}
	}

	freeSynth {
		ServerTree.remove(updateFunc, server);
		updateFunc = nil;
		ampSynth.release;
		ampSynth = nil
	}

	// sets volume back to 1 - removes the synth
	reset {
		isMuted = false;
		this.volume = 0.0;
	}

	volume_ { | aVolume |
		volume = aVolume.clip(min, max);
		this.changed(\amp, volume);
		this.updateSynth;
	}

	lag_ { | aLagTime |
		lag = aLagTime;
		if(ampSynth.notNil) { ampSynth.set(\volumeLag, lag) }
	}

	setVolumeRange { | argMin, argMax |
		var clippedVolume;
		argMin !? { min = argMin };
		argMax !? { max = argMax };
		this.changed(\ampRange, min, max);
		clippedVolume = volume.clip(min, max);
		if(clippedVolume != volume) { this.volume_(clippedVolume) }
	}

	gui { | window, bounds |
		^VolumeGui(this, window, bounds)
	}

	close {
		window.close;
	}

}

VolumeGui {

	var <model, <window;

	*new { | model, win, bounds |
		^super.newCopyArgs(model).init(win, bounds)
	}

	init { | win, bounds |
		var slider, box, simpleController;
		var spec = [model.min, model.max, \db].asSpec;
		bounds = bounds ?? { Rect(100, 100, 80, 330) };
		window = win ?? { Window.new("Volume", bounds).front };
		box = NumberBox(window, Rect(10, 10, 60, 30))
		.value_(model.volume);

		slider = Slider(window, Rect(10, 40, 60, 280))
		.value_(spec.unmap(model.volume));

		slider.action_({ | item |
			model.volume_(spec.map(item.value));
		});
		box.action_({ | item |
			model.volume_(item.value);
		});
		window.onClose_({
			simpleController.remove;
		});

		simpleController = SimpleController(model)
		.put(\amp, {|changer, what, volume|
			box.value_(volume.round(0.01));
			slider.value_(spec.unmap(volume));
		})
		.put(\ampRange, {|changer, what, min, max|
			spec = [min, max, \db].asSpec.debug;
			slider.value_(spec.unmap(model.volume));
		})
	}
}
