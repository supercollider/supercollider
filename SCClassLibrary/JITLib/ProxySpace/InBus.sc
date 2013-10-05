InBus {

	*ar { | bus, numChannels = 2, offset = 0 |
		^this.getOutput(bus.asBus, 'audio', numChannels, offset);
	}

	*kr { | bus, numChannels=1, offset = 0 |
		^this.getOutput(bus.asBus, 'control', numChannels, offset);
	}

	*getOutput { | bus, argRate, numChannels, offset = 0 |

		var out, index;
		var rate = bus.rate;
		var startIndex = bus.index + offset;
		var n = bus.numChannels;
		if(n >= numChannels) {
			index = startIndex.min(n + bus.index);
		} {
			index = Array.fill(numChannels, { arg i; startIndex + (i % n) });
			numChannels = 1;
		};

		out = if(offset.isInteger) {
			if(rate === 'audio') {
				InFeedback.ar(index, numChannels)
			} {
				In.kr(index, numChannels)
			}
		} {
			if(rate === 'audio') {
				XInFeedback.ar(index, numChannels)
			} {
				XIn.kr(index, numChannels)
			}
		};

		^if(argRate === rate) { out } { // if not the same rate, convert rates
			if(argRate === 'audio') { K2A.ar(out) } { A2K.kr(out) }
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

	var <ins, <outs, <amps;
	var <vol = 1.0, <>fadeTime = 0.02;
	var <group, synthIDs, synthAmps;

	play { | fromIndex, fromNumChannels = 2, toIndex, toNumChannels,
		target, multi = false, volume, fadeTime = 0.02, addAction |

		var server, inGroup, numChannels, bundle, divider;

		inGroup = target.asGroup;
		server = inGroup.server;

		bundle = List.new;
		this.playToBundle(
			bundle, fromIndex, fromNumChannels, toIndex,
			toNumChannels, inGroup, multi, volume, fadeTime, addAction
		);
		server.listSendBundle(server.latency, bundle);
	}

	stop { | argFadeTime |
		var oldGroup = group;
		group = nil;
		fadeTime = argFadeTime ? fadeTime;
		synthIDs = [];
		synthAmps = [];
		if(oldGroup.isPlaying) {
			oldGroup.release(fadeTime);
			SystemClock.sched(fadeTime, { oldGroup.server.bind { oldGroup.free } })
		};
	}

	isPlaying { ^group.isPlaying }
	numChannels { ^outs.size }

	// mapping between multiple channels

	playN { | out, amp, in, vol, fadeTime, target, addAction, multi = false |
		var bundle = List.new;
		var server, inGroup;
		inGroup = target.asGroup;
		server = inGroup.server;

		this.playNToBundle(bundle, out, amp, in, vol, fadeTime, inGroup, addAction, multi: multi);
		server.listSendBundle(server.latency, bundle);
	}

	// setting volume and output offset.
	// lists are only flat lists for now.

	vol_ { | val |
		if(val == vol) { ^this };
		vol = val;
		this.amps = amps;
	}

	// first channel interface

	out_ { | index |
		var offset = index - outs[0];
		this.outs = outs + offset
	}

	out {
		^outs !? { outs[0] }
	}

	// multi channel interface

	outs_ { | indices |
		if (outs.isNil) {
			"Monitor - initialising  outs: %\n".postf(indices);
			outs = indices;
			^this
		};
		if (outs.collect(_.size) != indices.collect(_.size)) {
			"new outs do not match old outs shape:".warn;
			("old:" + outs).postln;
			("new:" + indices).postln;
			" use playN to change topology!".postln;
			^this;
		};

		outs = indices;
		if(this.isPlaying) {
			group.server.listSendBundle(group.server.latency,
				[15, synthIDs, "out", outs.flat].flop
			)
		}
	}

	amps_ { | values |
		if (amps.isNil) {
			"Monitor - initialising  amps: %\n".postf(values);
			amps = values;
			^this
		};
		if (values.collect(_.size) != amps.collect(_.size)) {
			"new amps do not match old amps shape:".warn;
			("old:" + amps).postln;
			("new:" + values).postln;
			" use playN to change topology!".postln;
			^this;
		};

		synthAmps = values.flat * vol;
		amps = values;
		if (this.isPlaying) {
			group.server.listSendBundle(group.server.latency,
				[15, synthIDs, "vol", synthAmps].flop
			);
		};
	}

	// bundling

	playNToBundle {
		| bundle, argOuts, argAmps, argIns, argVol, argFadeTime, inGroup, addAction,
		defName = "system_link_audio_1", multi = false |

		var triplets, server;


		fadeTime = argFadeTime;
		vol = argVol ? 1.0;

		//if (ins.size != outs.size) { Error("wrong size of outs and ins" ++ [outs, amps, ins]).throw };
		// here is the question whether we want the channels to be reduced to avoid multiples.
		// this may cause quite complicated results though.

		if(multi) {
			ins = ins.addAll(argIns);
			outs = outs.addAll(argOuts);
			amps = amps.addAll(argAmps);
		} {
			ins = argIns;
			outs = argOuts ? outs ?? { (0..ins.size-1) };
			amps = argAmps ? amps ? #[1.0];
		};

		triplets = [ins, outs, amps].postln.flop.postln;


		if(this.isPlaying) {
			this.stopToBundle(bundle)
		} {
			this.newGroupToBundle(bundle, inGroup, addAction)
		};
		synthIDs = [];
		synthAmps = [];

		inGroup = inGroup.asGroup;
		server = group.server;

		triplets.do { | trip, i |
			var in, out, amp;
			#in, out, amp = trip;
			out = out.asArray;
			amp = amp.asArray;
			out.do { | item, j |
				var id = server.nextNodeID;
				synthIDs = synthIDs.add(id);
				synthAmps = synthAmps.add(amp[j]);
				bundle.add([9, defName,
					id, 1, group.nodeID,
					"out", item,
					"in", in,
					"vol", amp.clipAt(j) * vol
				])
			}
		};
		bundle.add([15, group.nodeID, "fadeTime", fadeTime])
	}

	playToBundle { | bundle, fromIndex, fromNumChannels=2, toIndex, toNumChannels,
		inGroup, multi = false, volume, argFadeTime, addAction |

		var server, numChannels;
		var outArray, inArray, ampArray;

		toIndex = toIndex ?? { if(outs.notNil, { outs[0] }, 0) };
		toNumChannels = toNumChannels ? fromNumChannels;
		inGroup = inGroup.asGroup;
		server = inGroup.server;

		inArray = fromIndex + (0..fromNumChannels-1);
		outArray = toIndex + (0..toNumChannels-1);
		this.playNToBundle(bundle, outArray, ampArray, inArray, volume, argFadeTime, inGroup, addAction, multi: multi)
	}


	playNBusToBundle { | bundle, outs, amps, ins, bus, vol, fadeTime, group, addAction, multi = false |
		var size;
		outs = outs ?? { this.outs.unbubble } ? 0;	// remember old ones if none given
		if (outs.isNumber) { outs = (0 .. bus.numChannels - 1) + outs };
		size = outs.size;
		ins = if(ins.notNil) {
			ins.wrap(0, bus.numChannels - 1).asArray
		} {
			(0..(bus.numChannels - 1))
		} + bus.index;

		ins = ins.wrapExtend(outs.size); // should maybe be done in playNToBundle, in flop?
		this.playNToBundle(bundle, outs, amps, ins, vol, fadeTime, group, addAction, multi: multi)
	}


	newGroupToBundle { | bundle, target, addAction=(\addToTail) |
		target = target.asGroup;
		group = Group.basicNew(target.server);
		group.isPlaying = true;
		bundle.add(group.newMsg(target, addAction));
		NodeWatcher.register(group);
	}


	stopToBundle { | bundle | // maybe with fade later.
		bundle.add([15, group.nodeID, "gate", 0]);
		synthIDs = [];
		synthAmps = [];
	}

	hasSeriesOuts {
		if (outs.isNil) { ^true };
		^(outs.size < 1) or: { ^outs.differentiate.drop(1).every(_ == 1) };
	}

	usedPlayN_ { |flag|
		this.deprecated(thisMethod);
	}
	*warnPlayN {
		this.deprecated(thisMethod);
	}
	*warnPlayN_ { |flag|
		this.deprecated(thisMethod);
	}
}
