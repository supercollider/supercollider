RecNodeProxy : NodeProxy {
	var <buffer, <path, <recGroup;
	
	isRecording {
		^recGroup.isPlaying
	}
	
	*control { ^this.notYetImplemented(thisMethod) }
		
	*initClass {
		for(1,8,{ arg i;
			SynthDef.writeOnce("system-diskout-" ++ i.asString, { arg i_in, i_bufNum=0;
				DiskOut.ar(i_bufNum, InFeedback.ar(i_in, i));
			});
		});
	}
	
	*newFrom { arg proxy, numChannels;
		^this.audio(proxy.server, numChannels ? proxy.numChannels)
			.source_({ proxy.ar(numChannels) });
	}
	
	
	open { arg argPath, headerFormat="aiff", sampleFormat="int16";
		var msg, n, bundle;
		n = this.numChannels;
		if(server.serverRunning.not, { "server not running".inform; ^this  });
		if(buffer.notNil, { "already recording. use pause/unpause".inform; ^this  });
		path = argPath;
		buffer = Buffer.alloc(server, 65536, n);
		buffer.write(path, headerFormat, sampleFormat, 0, 0, true);
		("initialised recording:" + path + 
			"(" ++ n + "channels on index" + buffer.bufnum ++ ")"
		).inform;
			
		CmdPeriod.add(this);
		
	}
	
	record { arg paused=true;
		var bundle, n;
		if(server.serverRunning.not, { "server not running".inform; ^this  });
		if(buffer.isNil, { "not prepared. use open to prepare a file.".inform;  ^this });
		
		bundle = MixedBundle.new;
		if(this.isPlaying.not, {
			this.wakeUpToBundle(bundle);
		});
		recGroup = Group.basicNew(server);
		bundle.add(recGroup.newMsg(server));
		NodeWatcher.register(recGroup);
		bundle.add([9, "system-diskout-" ++ this.numChannels, 
					server.nextNodeID, 1, recGroup.nodeID,
					\i_in, bus.index, \i_bufNum, buffer.bufnum
				]);
		if(paused, { recGroup.msgToBundle(bundle, "/n_run", 0); "recording (paused)".inform }); 
		bundle.schedSend(server);
	}
	
	
	cmdPeriod {
		CmdPeriod.remove(this);
		this.close;
	}
	
	pause { 
		if(this.isPlaying && recGroup.notNil, {
			recGroup.run(false);
			inform("_____now paused: " ++ path);
		}, { "not recording".inform })
	}
	
	unpause { 
		if(this.isPlaying && recGroup.notNil, {
			recGroup.run(true);
			inform("_____now recording: " ++ path);
		}, { "not ready for recording".inform })
	}
	
	close {
		if(recGroup.isPlaying, { recGroup.free });
		recGroup = nil;
		if(buffer.notNil, {
			inform("_____closed file: " ++ path);
			buffer.close;
		});
		path = nil;
		buffer.free;
		buffer = nil;
	}
	
	
	guiClass {
		^RecNodeProxyGui
	}
	

}