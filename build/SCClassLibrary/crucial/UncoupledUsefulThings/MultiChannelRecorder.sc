
MultiChanRecorder {

	var <>limit = true, busses, monitor,<>path;
	var recordBufs,recordNode,<>recHeaderFormat="aiff", <>recSampleFormat="int24";

	*new { arg busses;
		^super.new.init(busses);
	}
	init { arg buses;
		busses = buses;
	}
	prepareForRecord { arg basePath;
		path = basePath ?? (thisProcess.platform.recordingsDir +/+ "SC_" ++ Date.localtime.stamp);
		recordBufs = busses.collect({ arg b,i;
			Buffer.alloc(b.server, 65536, b.numChannels,{ arg buf;
				var chanNum="",chanPath;
				if(busses.size > 1,{ chanNum = "_" ++ i; });
				chanPath = path ++ chanNum ++ "." ++ recHeaderFormat;
				chanPath.debug("Preparing:");
				buf.writeMsg(chanPath,
					recHeaderFormat, recSampleFormat, 0, 0, true);
			});
		});

		recordNode = Patch({
			recordBufs.collect({ arg b,i;
				if(limit,{
					DiskOut.ar(b.bufnum, Limiter.ar(busses[i].ar,0.999) )
				},{
					DiskOut.ar(b.bufnum, busses[i].ar )
				})
			});
			// no audio out
			0.0
		});
		// better : add a node responder watching for n_end
		// then close the buffer and free

		// cmdPeriod support
		CmdPeriod.add(this);
	}
	isPrepared { ^recordBufs.notNil }
	record { arg atTime;
		if(recordBufs.isNil,{ // if they be nil
			"Please execute prepareForRecord before recording".warn;
		},{
			if(recordNode.isPlaying.not,{
				recordNode.play(0,atTime);
			},{
				recordNode.run(true,atTime)
			});
			"Recording".inform;
		});
	}
	isRecording { ^recordNode.isPlaying }
	pauseRecording { arg atTime;
		if(recordNode.notNil,{
			recordNode.run(false,atTime); "Paused".inform
		},{
			"Not Recording".warn
		});
	}

	stop { //arg atTime;
		if(recordNode.notNil,{
			recordNode.free;
			recordNode = nil;
			recordBufs.do({ |rb| rb.close(rb.freeMsg); });
			recordBufs = nil;
			"Recording Stopped".inform },
		{ "Not Recording".warn });
	}
	// mix outs to main out if they aren't already playing there
	monitor { arg boo = true;
		if(monitor.isNil,{ // assuming they are all on the same server
			monitor = Patch({
				Mix.new(
					busses.reject(_.isAudioOut).collect({ |b| b.ar });
				)
			});
		});
		if(boo,{ if(monitor.isPlaying.not,{ monitor.play }) },{ if(monitor.isPlaying,{ monitor.stop }) });
	}
	// private
	cmdPeriod {
		if(recordNode.notNil) { recordNode.free; recordNode = nil; };
		if(recordBufs.notNil) {
			recordBufs.do({ |buf| buf.close(buf.freeMsg); });
			recordBufs = nil;
		};
		CmdPeriod.remove(this);
	}
}
