RecNodeProxy : NodeProxy {
	var <buffer, <path, <recGroup;
	
	isRecording {
		^(recGroup.notNil && recGroup.isPlaying)
	}
	
	*control { ^this.notYetImplemented(thisMethod) }
		
	*initClass {
		SynthDef.writeOnce("system-diskout-2", { arg i_in, i_bufNum=0;
			DiskOut.ar(i_bufNum, InFeedback.ar(i_in, 2));
		});
		SynthDef.writeOnce("system-diskout-1", { arg i_in, i_bufNum=0;
			DiskOut.ar(i_bufNum, InFeedback.ar(i_in, 1));
		});
	}
	
	*newFrom { arg proxy, numChannels, onCompletion;
		^this.audio(proxy.server, numChannels ? proxy.numChannels)
			.put({ proxy.ar(numChannels) }, 0, true, onCompletion:onCompletion);
	}
	
	
	open { arg argPath, headerFormat="aiff", sampleFormat="int16";
		var msg, n, bundle;
		n = this.numChannels;
		if(server.serverRunning.not, { "server not running".inform; ^this  });
		if(buffer.notNil, { "already recording. use pause/unpause".inform; ^this  });
		path = argPath;
		buffer = Buffer.alloc(server, 65536, n);
		buffer.write(path, headerFormat, sampleFormat, 0, 0, true);
		("initialised recording:" + path).inform;
			
		CmdPeriod.add(this);
		
	}
	
	record { arg paused=true;
		var bundle, divider, n;
		if(server.serverRunning.not, { "server not running".inform; ^this  });
		if(buffer.isNil, { "not prepared. use open to prepare a file.".inform;  ^this });
		
		bundle = DebugBundle.new;
		n = this.numChannels;
		if(this.isPlaying.not, {
			this.wakeUpToBundle(bundle);
		});
		
		recGroup = Group.newToBundle(bundle, server);
		divider = if(n.even, 2, 1);
		(n div: divider).do({ arg i;
		bundle.add([9, "system-diskout-"++divider, 
					server.nextNodeID, 1, recGroup.nodeID,
					\i_in, outbus.index+(i*divider), \i_bufNum, buffer.bufnum
				])
		});
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
		})
	}
	
	unpause { 
		if(this.isPlaying && recGroup.notNil, {
			recGroup.run(true);
		})
	}
	
	close {
		
		recGroup.free;
		recGroup = nil;
		super.stop;
		
		if(buffer.notNil, {
			inform("closed file: " ++ path);
			buffer.close;
		});
		path = nil;
		//buffer.free;
	}
	
	free {
		super.free;
		this.stop;
	}
	
	guiClass {
		^RecNodeProxyGui
	}
	

}