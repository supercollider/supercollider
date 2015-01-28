InBus : UGen {

	*ar { | bus, numChannels, offset = 0, clip |
		^this.multiNew('audio', Ref(bus), numChannels, offset, clip);
	}

	*kr { | bus, numChannels, offset = 0, clip |
		^this.multiNew('control', Ref(bus), numChannels, offset, clip);
	}

	*new1 { |rate, bus, numChannels, offset, clip|
		var out, index, n, busRate;
		bus = bus.value.asBus;
		busRate = bus.rate;
		n = bus.numChannels;
		numChannels = numChannels ? n;

		if(offset.isNumber and: { offset + numChannels <= n }) {
			index = bus.index + offset;  // then we can optimize
		} {
			offset = offset + (0 .. numChannels-1);
			offset = if(clip === \wrap) { offset % n } { min(offset, n - 1) };
			index = (bus.index + offset).unbubble;
			numChannels = 1;
		};


		out = if(index.isInteger) {
			if(busRate === 'audio') {
				InFeedback.ar(index, numChannels)
			} {
				In.kr(index, numChannels)
			}
		} {
			if(busRate === 'audio') {
				XInFeedback.ar(index, numChannels)
			} {
				XIn.kr(index, numChannels)
			}
		};

		^if(rate === busRate) { out } { // if not the same rate, convert rates
			if(rate === 'audio') { K2A.ar(out) } { A2K.kr(out) }
		};

	}

}

XIn {

	*ar { | which, n |

		^XFade2.ar( // use equal power crossfading for audio rate
			In.ar(which.round(2), n),
			In.ar(which.trunc(2) + 1, n),
			(which * 2 - 1).fold2(1)
		)

	}

	*kr { | which, n |
		^LinXFade2.kr( // use linear crossfading for control rate
			In.kr(which.round(2), n),
			In.kr(which.trunc(2) + 1, n),
			(which * 2 - 1).fold2(1)
		)
	}

}

XInFeedback {

	*ar { | which, n |
		^XFade2.ar(
			InFeedback.ar(which.round(2), n),
			InFeedback.ar(which.trunc(2) + 1, n),
			(which * 2 - 1).fold2(1)
		);
	}

}


// listens on a number of busses and plays out to various other busses

Monitor {

	var <ins, <outs, <amps, <fadeTimes;
	var <vol = 1.0, defaults;
	var <group, synthIDs;

	copy {
		^this.class.newCopyArgs(*[ins, outs, amps, fadeTimes, vol, defaults].deepCopy)
	}

	// mapping between multiple contiguous channels

	play { | fromIndex, fromNumChannels = 2, toIndex, toNumChannels,
		target, multi = false, volume, fadeTime, addAction |

		var bundle = List.new;
		var inGroup = target.asGroup;
		var server = inGroup.server;

		bundle = List.new;
		this.playToBundle(
			bundle, fromIndex, fromNumChannels, toIndex,
			toNumChannels, inGroup, multi, volume, fadeTime, addAction
		);
		server.listSendBundle(server.latency, bundle);
	}

	// mapping between multiple channels individually

	playN { | out, amp, in, vol, fadeTime, target, addAction, multi = false |

		var bundle = List.new;
		var inGroup = target.asGroup;
		var server = inGroup.server;

		this.playNToBundle(bundle, out, amp, in, vol, fadeTime, inGroup, addAction, multi: multi);
		server.listSendBundle(server.latency, bundle);
	}

	stop { | argFadeTime |
		var oldGroup = group, groupFreeTime;
		group = nil;
		synthIDs = [];
		if(oldGroup.isPlaying) {
			groupFreeTime = if(argFadeTime.notNil) { argFadeTime } { fadeTimes.asArray.maxItem ? 0.02 };
			oldGroup.release(argFadeTime);
			SystemClock.sched(groupFreeTime, { oldGroup.server.bind { oldGroup.free } })
		};
	}

	clear { | argFadeTime |
		this.stop(argFadeTime);
		ins = outs = amps = fadeTimes = nil;
		vol = 1.0;
	}

	isPlaying { ^group.isPlaying }
	numChannels { ^outs.size }

	vol_ { | val |
		if(val == vol) { ^this };
		vol = val; // todo: what to do if vol is a bus? is there a generalisation?
		if(this.isPlaying) { group.set(\vol, val) };
	}

	// backward compatibility
	// assuming that "out" refers to the first channel, like the Out UGen's out argument.

	out_ { | index |
		var offset;
		if(outs.isNil) { this.outs = index.asArray };
		offset = index - outs[0];
		this.outs = outs + offset
	}

	out {
		^outs !? { outs[0] }
	}

	fadeTime_ { | val |
		this.fadeTimes = val.asArray
	}

	fadeTime { | val |
		^fadeTimes !? { fadeTimes.unbubble }
	}

	// keeping default arguments

	updateDefault { |argName, value|
		defaults = defaults ? ();
		^if(value.isNil) {
			defaults.at(argName)
		} {
			defaults.put(argName, value);
			value
		}
	}

	// multi channel interface

	outs_ { | indices |
		outs = indices;
		this.updateDefault(\outs, indices);
		if(this.isPlaying) {
			group.server.listSendBundle(group.server.latency,
				[15, synthIDs, "out", outs.flat.keep(synthIDs.size)].flop
			)
		};
	}

	amps_ { | values |
		amps = values;
		if (this.isPlaying) {
			group.server.listSendBundle(group.server.latency,
				[15, synthIDs, "level", values.flat.keep(synthIDs.size)].flop
			);
		};
	}

	fadeTimes_ { | values |
		fadeTimes = values;
		if (this.isPlaying) {
			group.server.listSendBundle(group.server.latency,
				[15, synthIDs, "fadeTime", values.flat.keep(synthIDs.size)].flop
			);
		};
	}


	// bundling

	playNToBundle {
		| bundle, argOuts, argAmps, argIns, argVol, argFadeTime, inGroup, addAction,
		defName = "system_link_audio_1", multi = false |

		var synthArgs, server;

		ins = argIns ? ins;
		outs = argOuts ? outs;
		amps = argAmps ? amps;
		fadeTimes = argFadeTime ? fadeTimes;
		vol = (argVol ? vol).asControlInput;

		synthArgs = [
			ins,
			outs ?? { (0..ins.size-1) },
			amps ? #[1.0],
			fadeTimes ? #[0.02]
		].asControlInput.flop;

		inGroup = this.updateDefault(\inGroup, inGroup) ? inGroup;
		inGroup = inGroup.asGroup;

		if(this.isPlaying) {
			if(multi.not) {
				this.stopToBundle(bundle, argFadeTime);
				if(group.group != inGroup) {
					this.newGroupToBundle(bundle, inGroup, addAction)
				}
			}
		} {
			this.newGroupToBundle(bundle, inGroup, addAction)
		};

		server = group.server;

		synthArgs.do { | array, i |
			var in, out, amp, fadeTime;
			#in, out, amp, fadeTime = array;
			out = out.asArray;
			amp = amp.asArray;
			fadeTime = fadeTime.asArray;
			out.do { | item, j |
				var id = server.nextNodeID;
				synthIDs = synthIDs.add(id);
				bundle.add([9, defName,
					id, 1, group.nodeID,
					"out", item,
					"in", in,
					"level", amp.clipAt(j),
					"fadeTime", fadeTime.clipAt(j),
					"vol", vol
				])
			}
		};
	}


	playToBundle { | bundle, fromIndex, fromNumChannels, toIndex, toNumChannels,
		inGroup, multi = false, volume, argFadeTime, addAction |

		var outArray, inArray;

		toIndex = this.updateDefault(\toIndex, toIndex) ? 0;
		fromNumChannels = this.updateDefault(\fromNumChannels, fromNumChannels) ? 2;
		toNumChannels = this.updateDefault(\toNumChannels, toNumChannels) ? fromNumChannels;

		inArray = fromIndex + (0..fromNumChannels-1);
		outArray = toIndex + (0..toNumChannels-1);
		this.playNToBundle(bundle, outArray, nil, inArray, volume, argFadeTime, inGroup, addAction, multi: multi)
	}


	playNBusToBundle { | bundle, outs, amps, ins, bus, vol, fadeTime, group, addAction, multi = false |

		if(bus.rate == \control) {
			"Can't monitor a control rate bus.".warn;
			this.stopToBundle(bundle);
			^this
		};
		ins = this.updateDefault(\offsetIns, ins);
		outs = this.updateDefault(\outs, outs) ? 0;


		if (outs.unbubble.isNumber) { outs = (0 .. bus.numChannels - 1) + outs };

		ins = if(ins.notNil) {
			ins.wrap(0, bus.numChannels - 1).asArray
		} {
			(0..(bus.numChannels - 1))
		} + bus.index;

		this.playNToBundle(bundle, outs, amps, ins, vol, fadeTime, group, addAction, multi: multi)
	}

	newGroupToBundle { | bundle, target, addAction=(\addToTail) |
		target = target.asGroup;
		group = Group.basicNew(target.server);
		group.isPlaying = true;
		bundle.add(group.newMsg(target, addAction));
		NodeWatcher.register(group);
	}


	stopToBundle { | bundle, argFadeTime, clearMappings = false |
		if(synthIDs.notEmpty) {
			bundle.add(['/error', -1]);
			if(argFadeTime.notNil) {
				argFadeTime = argFadeTime.asControlInput.asArray;
				synthIDs.do { |id, i|
					bundle.add([15, id, "gate", 0, "fadeTime", argFadeTime.wrapAt(i)])
				}
			} {
				bundle.add([15, group.nodeID, "gate", 0]);
			};
			bundle.add(['/error', -2]);
			synthIDs = [];
		};
		if(clearMappings) {
			ins = outs = amps = fadeTimes = nil;
			defaults.clear;
		};
	}

	hasSeriesOuts {
		^outs.asArray.isSeries(1)
	}

	// this method is used to find the best representation for compileStrings.
	usedPlayN {
		^fadeTimes.isArray and: { fadeTimes.size > 1 }
		or: { outs.asArray.isSeries(1).not }
		or: { ins.asArray.isSeries(1).not }
	}

	*warnPlayN {
		this.deprecated(thisMethod);
	}
	*warnPlayN_ { |flag|
		this.deprecated(thisMethod);
	}
}
