+ MIDIClient {

	*externalSources{
		^MIDIClient.sources.select({ |src,i|
			// src.device != "SuperCollider"
			(src.uid >> 16) != this.getClientID;
		})
	}

	*externalDestinations{
		^MIDIClient.destinations.select({ |src,i|
			// src.device != "SuperCollider"
			(src.uid >> 16) != this.getClientID;
		})
	}

	*getClientID {
		_GetMIDIClientID;
		^this.primitiveFailed;
	}
}

+ MIDIIn{
	*connectAll { |verbose=true|
		if(MIDIClient.initialized.not,
			{ MIDIClient.init(verbose: verbose) },
			{ MIDIClient.disposeClient; MIDIClient.init(verbose: verbose); }
			// on Linux, supercollider creates as many MIDI ports for SC as there are devices to connect to; MIDIClient.list will find the new sources, but there will not be a matching port initialized for SC to connect it to; therefor we reinitialize the client.
		);
		MIDIClient.externalSources.do({ |src,i|
			MIDIIn.connect(i,src);
		});
	}
}

+ MIDIOut{
	// uid is not set by connect, in order to enable several connections to one output. set the uid directly, if you only want to send data to one MIDI destination.
	connect{ arg device = 0;
		MIDIOut.connect( port, device );
		/*
		var cuid, dest;
		if(device.isNumber, {
			if(device >= 0, {
				if(MIDIClient.initialized.not,{ MIDIClient.init });
				if ( device > MIDIClient.destinations.size, {
					cuid = device;
				},{
					dest = MIDIClient.destinations.at(device);
					if(dest.isNil,{
						"MIDIClient failed to init".warn;
					},{
						cuid = MIDIClient.destinations.at(device).uid;
					})
				})
			},{
				cuid = device;
			});
		},{
			if(device.isKindOf(MIDIEndPoint), {cuid = device.uid}); // else error
			});
			MIDIOut.connectByUID( port,cuid );*/
	}
	disconnect{ |cuid|
		var res;
		MIDIOut.disconnect( port, cuid );
		//		MIDIOut.disconnectByUID(port,cuid);
		// reset the uid to 0
		// uid = 0;
	}
	*connect { arg outport=0, device=0;
		var uid,dest;
		if(MIDIClient.initialized.not,{ MIDIClient.init });
		if(device.isNumber, {
			if(device >= 0, {
				if ( device > MIDIClient.destinations.size, {
					dest = MIDIClient.destinations.select{ |it| it.uid == device }.first;
					if(dest.isNil,{
						("MIDI device with uid"+device+ "not found").warn;
					},{
						uid = dest.uid;
					})
				},{
					dest = MIDIClient.destinations.at(device);
					if(dest.isNil,{
						"MIDIClient failed to init".warn;
					},{
						uid = MIDIClient.destinations.at(device).uid;
					})
				})
			},{
				uid = device;
			});
		},{
			if(device.isKindOf(MIDIEndPoint), {uid = device.uid}); // else error
		});
		this.connectByUID(outport,uid);
	}
	*disconnect { arg outport=0, device=0;
		var uid,dest;
		if(device.isKindOf(MIDIEndPoint), {uid = device.uid});
		if(device.isNumber, {
			if(device.isPositive, {
				if ( device > MIDIClient.destinations.size, {
					dest = MIDIClient.destinations.select{ |it| it.uid == device }.first;
					if(dest.isNil,{
						("MIDI device with uid"+device+ "not found").warn;
					},{
						uid = dest.uid;
					})
				},{
					uid = MIDIClient.destinations.at(device).uid
				});
			},{
				uid = device;
			});
		});
		this.disconnectByUID(outport,uid);
	}
	*connectByUID {arg outport, uid;
		_ConnectMIDIOut
		^this.primitiveFailed;
	}
	*disconnectByUID {arg outport, uid;
		_DisconnectMIDIOut
		^this.primitiveFailed;
	}
}
