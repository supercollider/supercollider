
MIDIEndPoint {
	var <>device, <>name, <>uid;
	*new{ arg device, name, uid;
		^super.newCopyArgs(device, name, uid)
	}
}

MIDIClient {
	classvar <sources, <destinations;
	classvar <initialized=false;
	*init { arg inports=1, outports=1;
		this.prInit(inports,outports);
		initialized = true;
		this.list;
		// might ask for 1 and get 2 if your device has it
		if(sources.size < inports or: {destinations.size < outports},{
			("MIDIClient init failed.  \nsources: "+sources+"destinations:"+destinations).die;
		});
	}
	*list {
		var list;
		list = this.prList;
		if(list.notNil, {
			sources = list.at(0).collect({ arg id,i;
				MIDIEndPoint(list.at(1).at(i), list.at(2).at(i), id)
			});
			destinations = list.at(3).collect({arg id, i;
				MIDIEndPoint(list.at(4).at(i), list.at(4).at(i), id)
			});
		});
	}
	*prInit { arg inports, outports;
		_InitMIDI
		^this.primitiveFailed
	}
	*prList {
		_ListMIDIEndpoints
		^this.primitiveFailed
	}
	*disposeClient {
		_DisposeMIDIClient
		^this.primitiveFailed
	}
	*restart {
		_RestartMIDI
		^this.primitiveFailed
	}
}

MIDIIn {
	var <>port;
	classvar <>action, 
	<> noteOn, <> noteOff, <> polytouch, 
	<> control, <> program, 
	<> touch, <> bend;	
	
	
	*doAction { arg src, status, a, b, c;
		action.value(src, status, a, b, c);
	}
	*doNoteOnAction { arg src, chan, num, veloc;
		noteOn.value(src, chan, num, veloc);
	}
	*doNoteOffAction { arg src, chan, num, veloc;
		noteOff.value(src, chan, num, veloc);
	}
	*doPolyTouchAction { arg src, chan, num, val;
		polytouch.value(src, chan, num, val);
	}
	*doControlAction { arg src, chan, num, val;
		control.value(src, chan, num, val);
	}
	*doProgramAction { arg src, chan, val;
		program.value(src, chan, val);
	}
	*doTouchAction { arg src, chan, val;
		touch.value(src, chan, val);
	}
	*doBendAction { arg src, chan, val;
		bend.value(src, chan, val);
	}
	*connect { arg inport=0, device=0;
		var uid;
		if(device.isNumber, {
			if(device >= 0, {
				if(MIDIClient.initialized.not,{ MIDIClient.init });
				uid = MIDIClient.sources.at(device).uid
			},{
				uid = device;
			});
		},{
			if(device.isKindOf(MIDIEndPoint), {uid = device.uid}); // else error
		});
		this.connectByUID(inport,uid);
	}
	*disconnect { arg inport=0, device=0;
		var uid;
		if(device.isKindOf(MIDIEndPoint), {uid = device.uid});
		if(device.isNumber, {
			if(device.isPositive, { 
				uid = MIDIClient.sources.at(device).uid
			},{
				uid = device;
			});
		});
		this.disconnectByUID(inport,uid);
	}
	*connectByUID {arg inport, uid;
		_ConnectMIDIIn		
	}
	*disconnectByUID {arg inport, uid;
		_DisconnectMIDIIn		
	}
	
	*new { arg port;
		^super.new.port_(port)
	}
	
}
	
MIDIOut {
	var <>port, <> uid;
	
	*new { arg port, uid;
		^super.newCopyArgs(port, uid);
	}
		
	write { arg len, hiStatus, chan, a=0, b=0;
		this.send(port, uid, len, hiStatus, chan, a, b);
	}
	
	noteOn { arg chan, note=60, veloc=64;
		this.write(3, 16r90, chan, note, veloc);
	}
	noteOff { arg chan, note=60, veloc=64;
		this.write(3, 16r80, chan, note, veloc);
	}
	polyTouch { arg chan, note=60, val=64;
		this.write(3, 16rA0, chan, note, val);
	}
	control { arg chan, ctlNum=7, val=64;
		this.write(3, 16rB0, chan, ctlNum, val);
	}
	program { arg chan, num=1;
		this.write(2, 16rC0, chan, num);
	}
	touch { arg chan, val=64;
		this.write(2, 16rD0, chan, val);
	}
	bend { arg chan, val=64;
		this.write(3, 16rE0, chan, val, 0);
	}
	allNotesOff { arg chan;
		this.control(chan, 123, 0);
	}
	send {arg outport, uid, len, stat, chan, a=0, b=0, latency=0.1; //in ms
		_SendMIDIOut		
	}
}
