Volume {

	var <server, <startBus, numChannels, <min, <max, <persist;

	var <volume = 0.0, <lag = 0.1, <isMuted = false;

	var <ampSynth, <numOutputChannels, <defName, updateFunc, initFunc;
	var <>window;
	var <group, <ampSynthRunning = false, <listener;

	*new { | server, startBus = 0, numChannels, min = -90, max = 6, persist = false |
		^super.newCopyArgs(server ?? { Server.default }, startBus, numChannels, min, max, persist).init;
	}

	init {
		// hardcode group nodeID to 2, so multiple clients can use it
		group = Group.basicNew(server, 2);
		defName = (\volumeAmpControl ++ this.numChannels).asSymbol;
		// hardcode synth nodeID to 3, so multiple clients can use it
		ampSynth = Synth.basicNew(defName, server, 3);
		ampSynth.group = group;

		// execute immediately if we're already past server tree functions
		if(server.serverRunning) {
			this.sendSynthDef;
			this.updateSynth;
		};

		initFunc = {
			ampSynthRunning = false;
			server.sendMsg(*group.newMsg(RootNode(server), \addToTail));
			this.sendSynthDef;

			// only create synth now if it won't be created by ServerTree
			if (persist.not) { this.updateSynth };

			// only start listener is multiple clients expected
			if (server.options.maxLogins > 1) {
				this.startListener
			}
		};
		ServerBoot.add(initFunc, server);

		updateFunc = {
			ampSynthRunning = false;
			if(persist) { this.updateSynth }
		};

		ServerTree.add(updateFunc, server);

	}

	startListener {
		listener = listener ?? {
			OSCFunc({ |msg|
				var invol, inlag, ingate, inbus;
				#invol, inlag, ingate, inbus = msg.drop(3);

				// server.post; msg.round(0.001).postcs;

				invol = invol.ampdb;
				if (invol.equalWithPrecision(volume).not) {
					"sync vol: ".postln;
					volume = invol;
					this.changed(\amp, volume);
				};
				if (inlag.equalWithPrecision(lag).not) {
					"sync lag:".postln;
					lag = inlag;
				};
				if (ingate <= 0.0) {
					"ampSynth ended".postln;
					ampSynthRunning = false;
					msg.round(0.001).postcs;
					volume = invol;
					this.changed(\amp, volume);
				} {
					ampSynthRunning = true;
				};
				if (inbus != startBus) {
					"*** startBus changed!".postln;
					startBus = inbus;
				};

			}, '/volumeUpdate', server.addr).permanent_(true)
		};
		listener.enable;
	}

	sendSynthDef {
		if (server.hasBooted) {
			fork {
				numOutputChannels = this.numChannels;
				// update defName in case numChannels has changed
				defName = (\volumeAmpControl ++ numOutputChannels).asSymbol;
				ampSynth.defName = defName;

				SynthDef(defName, { | volumeAmp = 1, volumeLag = 0.1, gate=1, bus |

					var controls = [volumeAmp, volumeLag, gate, bus];
					var updateTrig = Impulse.kr(1) + HPZ1.kr(controls).abs.sum;
					// what replyID to use for shared remote server name?
					SendReply.kr(updateTrig, '/volumeUpdate', controls);

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
		if(ampSynthRunning and: { num != this.numChannels }) {
			"Change in number of channels will not take effect until volume is reset to 0dB.".warn;
		};
		numChannels = num;
	}

	updateSynth {
		var amp = if(isMuted.not) { volume.dbamp } { 0.0 };
		var active = amp != 1.0;

		thisMethod.postln;

		if(active) {
			if(server.hasBooted) {
				if(ampSynthRunning.not) {
					// Synth.after(server.defaultGroup, defName,
					// [\volumeAmp, amp, \volumeLag, lag, \bus, startBus])

					server.sendMsg(*group.newMsg(RootNode(server), \addToTail));
					server.sendMsg(9, //"s_new"
						defName, ampSynth.nodeID,
						Node.addActions[\addToTail],
						group.nodeID,
						*[\volumeAmp, amp, \volumeLag, lag, \bus, startBus]
					);

					ampSynthRunning = true;
				} {
					ampSynth.set(\volumeAmp, amp);
				}
			}
		} {
			if(ampSynthRunning) {
				ampSynth.set(\volumeAmp, 1).release;
				ampSynthRunning = false
			}
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
		ampSynth.release;
		ampSynthRunning = false;
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
