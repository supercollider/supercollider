InBus {
	
	*ar { arg bus, numChannels=2, offset=0;
		^this.getOutput(bus.asBus, 'audio', numChannels, offset);
	}
	
	*kr { arg bus, numChannels=1, offset=0;
		^this.getOutput(bus.asBus, 'control', numChannels, offset);
	}
	
	*getOutput { arg bus, argRate, numChannels, offset=0;
		var rate, n, out, startIndex, index;
		rate = bus.rate;
		startIndex = bus.index + offset;
		n = bus.numChannels;
		if(n >= numChannels) {
			index = startIndex.min(n + bus.index);
		} {
			index = Array.fill(numChannels, { arg i; startIndex + (i % n) });
			numChannels = 1;
		};
		
		out = if(offset.isInteger) {
					if(rate === 'audio')
						{ InFeedback.ar(index, numChannels) }
						{ In.kr(index, numChannels) }
			} {
					if(rate === 'audio')
						{ XInFeedback.ar(index, numChannels) }
						{ XIn.kr(index, numChannels) }
			};
		^if(argRate === rate) { out } { // if not the same rate, convert rates
			if(argRate === 'audio') { K2A.ar(out) } { A2K.kr(out) }
		};
	
	}

}

// crossfde In by AdC and JRh

XIn { 
	*ar { arg which, n;
		^XFade2.ar( // use equal power crossfading for audio rate
			In.ar(which.round(2), n), 
			In.ar(which.trunc(2) + 1, n), 
			(which * 2 - 1).fold2(1)
		);
	}
	*kr { arg which, n;
		^LinXFade2.kr( // use linear crossfading for control rate
			In.kr(which.round(2), n), 
			In.kr(which.trunc(2) + 1, n), 
			(which * 2 - 1).fold2(1)
		);
	}
	
}

XInFeedback { 
	*ar { arg which, n;
		^XFade2.ar(
			InFeedback.ar(which.round(2), n), 
			InFeedback.ar(which.trunc(2) + 1, n), 
			(which * 2 - 1).fold2(1)
		);
	}
}



// listens on a fixed index (or several)
// plays out to various other indices.


Monitor {
	var <ins, <outs, <amps = #[1.0], <vol=1;
	var <group, synthIDs, synthAmps, <>fadeTime=0.02;
	
	
	
	play { arg fromIndex, fromNumChannels=2, toIndex, toNumChannels, 
			target, multi=false, volume, fadeTime=0.02, addAction;
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
	
	stop { arg argFadeTime;
		fadeTime = argFadeTime ? fadeTime;
		synthIDs = [];
		synthAmps = [];
		if(group.isPlaying) {
			group.release(fadeTime);
			SystemClock.sched(fadeTime, { group.free; group = nil })
		}
	}
	
	isPlaying {Ê^group.isPlaying }
	numChannels {Ê^outs.size }
	
	// multichannel support
	
	playN { arg out, amp, in, vol, fadeTime, target, addAction;
		var bundle = List.new;
		var server, inGroup;
		inGroup = target.asGroup;
		server = inGroup.server;
		
		this.playNToBundle(bundle, out, amp, in, vol, fadeTime, inGroup, addAction);
		server.listSendBundle(server.latency, bundle);
		
	}
	
	// setting volume and output offset.
	// lists are only flat lists for now.
		
	vol_ { arg val;
		if(val == vol) {Ê^this };
		vol = val;
		this.amps = amps;
	}
	
	// first channel interface
	
	out_ { arg index;
		var offset = index - outs[0];
		this.outs = outs + offset
	}
	
	out {
		^outs !? { outs[0] }
	}
	
	// multi channel interface
	
	outs_ { arg indices;
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
	
	amps_ { arg values;
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
	
	playNToBundle { arg bundle, argOuts=(outs ?? {(0..ins.size-1)}), argAmps=(amps), argIns=(ins),
					argVol=(vol), argFadeTime=(fadeTime), inGroup, addAction, 					defName="system_link_audio_1"; 
		
		var triplets, server; 

		outs = argOuts; ins = argIns; amps = argAmps; vol = argVol; fadeTime = argFadeTime;
		
		
		if (ins.size != outs.size)
			{ Error("wrong size of outs and ins" ++ [outs, amps, ins]).throw };
			
		triplets = [ins, outs, amps].flop;
		
		if(this.isPlaying) { 
			this.stopToBundle(bundle) 
		} {
			this.newGroupToBundle(bundle, inGroup)
		};
		synthIDs = [];
		synthAmps = [];
		
		server = group.server;
		
		triplets.do {|trip, i|
			var in, out, amp;
			#in, out, amp = trip;
			out = out.asArray;
			amp = amp.asArray;
			out.do { |item, j|
				var id = server.nextNodeID;
				synthIDs = synthIDs.add(id);
				synthAmps = synthAmps.add(amp[j]);
				bundle.add([9, defName, 
					id, Node.actionNumberFor(addAction), group.nodeID,
					"out", item, 
					"in", in,
					"vol", amp.clipAt(j) * vol
				]);
			};
		};
		bundle.add([15, group.nodeID, "fadeTime", fadeTime]);
	}
	
	// optimizes ranges of channels
	
	playToBundle { arg bundle, fromIndex, fromNumChannels=2, toIndex, toNumChannels, 
			inGroup, multi, volume, fadeTime=0.02, addAction;
		var server, numChannels, defname, chanRange, n;
					
		toIndex = toIndex ?? { if(outs.notNil, { outs[0] }, 0) }; 
		
		vol = volume ? vol;
		toNumChannels = toNumChannels ? fromNumChannels;
		server = inGroup.server;
		
		if(this.isPlaying) { 
			if(multi.not) { 
				this.stopToBundle(bundle);
				outs = [];
			}
		} {
			this.newGroupToBundle(bundle, inGroup);
			if (multi.not) { outs = [] }
		};
		
		amps = [];
		
		numChannels = max(fromNumChannels, toNumChannels);
		chanRange = if(toNumChannels.even and: { fromNumChannels.even }, 2, 1);
		defname = "system_link_audio_" ++ chanRange;
		(numChannels div: chanRange).do { arg i;
			var id = server.nextNodeID;
			var out = toIndex + (i * chanRange % toNumChannels);
			var in = fromIndex + (i * chanRange % fromNumChannels);
			synthIDs = synthIDs.add(id);
			outs = outs.add(out);
			ins = ins.add(in);
			amps = amps.add(1.0);
			bundle.add([9, defname, id, Node.actionNumberFor(addAction), 
						group.nodeID, "out", out, "in", in]);
		};
		bundle.add([15, group.nodeID, "fadeTime", fadeTime, "vol", vol]);
	}
	
	// shorter, but far less efficient
	//playToBundle { arg bundle, fromIndex, fromNumChannels=2, toIndex, toNumChannels, 
//			inGroup, multi, volume, fadeTime;
//			var numChannels = max(fromNumChannels, toNumChannels);
//			var chanRange = if(toNumChannels.even and: { fromNumChannels.even }, 2, 1);
//			var n = numChannels div: chanRange;
//			var outputChannels = Array.fill(n, {|i| toIndex + (i * chanRange % toNumChannels) });
//			var inputChannels = Array.fill(n, {|i| fromIndex + (i * chanRange % fromNumChannels) });
//			//[bundle, outputChannels, nil, inputChannels, 
//			//	volume, fadeTime, inGroup, "system_link_audio_" ++ chanRange].postln;
//			this.playNToBundle(bundle, outputChannels, nil, inputChannels, 
//				volume, fadeTime, inGroup, "system_link_audio_" ++ chanRange);
//				
//	}
	
	playNBusToBundle { arg bundle, outs, amps, bus, vol, fadeTime, group;
		var size, ins;
		outs = outs ?? {this.outs.unbubble} ? 0;	// remember old ones if none given
		if (outs.isNumber) { outs = (0 .. bus.numChannels - 1) + outs };
		size = outs.size;
		ins = (0..(size - 1)) + bus.index;
		this.playNToBundle(bundle, outs, amps, ins, vol, group, fadeTime)
	}
	
		
	newGroupToBundle { arg bundle, inGroup;
				inGroup = inGroup.asGroup;
				group = Group.basicNew(inGroup.server);
				group.isPlaying = true;
				bundle.add(group.newMsg(inGroup, \addToTail));
				NodeWatcher.register(group);
	}

	
	stopToBundle { arg bundle; // maybe with fade later.
		bundle.add([15, group.nodeID, "gate", 0]);
		synthIDs = [];
		synthAmps = [];
	}
	
	hasSeriesOuts { 
		if (outs.isNil, { ^true });
		^(outs.size < 1) or: { ^outs.differentiate.drop(1).every(_ == 1) };
	} 
}




// todo: 
// analysis of contiguous channels for efficiency
// maybe: merging of playN and play
// recording function.


/*
	record { arg argAmps=(amps), argIns=(ins), vol, fadeTime;
		var numChannels = ins.size;
		if(numChannels > SystemSynthDefs.numChannels) { 
				"too few channels defined in SystemSynthDefs".error ^this 
		};
		
		if(this.isPlaying.not) { 
			this.newGroupToBundle(bundle, inGroup)
		};	 
	}
	*/

