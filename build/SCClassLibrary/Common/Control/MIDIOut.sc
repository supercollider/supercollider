MIDIEndPoint {
var <> name, <>uid, <> device;
	*new{arg name, uid;
		^super.newCopyArgs(name, uid)
		}
}

MIDIClient {
	classvar < sources, < destinations;
	classvar < initialized=false;
	classvar < numberOfSources, < numberOfDestinations;
	classvar < sourceUID;
	*init {arg inports=1, outports=1;
		initialized = true;
		this.prInit(inports,outports);
		this.list;
	}
	*list{
		var out, arr, endp;
		out = this.prList;

	}
	*prSetNumberOfSources {arg anum;
		numberOfSources = anum;
		anum.do({ sources = sources.add(MIDIEndPoint("no", nil))});
		}
	*prSetNumberOfDestinations {arg anum;
		numberOfDestinations = anum;
		anum.do({ destinations = destinations.add(MIDIEndPoint("no", nil))});
		}
	*prSetSourceDeviceName{arg idx, aname;
		sources.at(idx).device = aname;
		}
	*prSetSourcePortName{arg idx, aname;
		sources.at(idx).name = aname;
		}
	*prSetSourceUID{arg idx, aname;
		sources.at(idx).uid = aname;
		}
	*prSetDestinationDeviceName{arg idx, aname;
		destinations.at(idx).device = aname;
		}
	*prSetDestinationPortName{arg idx,aname;
		destinations.at(idx).name = aname;
		}
	*prSetDestinationUID{arg idx,aname;
		destinations.at(idx).uid = aname;
		}
	*prInit {arg inports, outports;
		_InitMIDI;
	}
	*prList {
		_ListMIDIEndpoints
	}
	*disposeClient{
		_DisposeMIDIClient
	}
	*restart{
		_RestartMIDI
	}
}

MIDIIn {
	var <>port;
	classvar <>action, 
	<> note, <> polytouch, 
	<> control, <> program, 
	<> touch, <> bend;	
	
	
	*new { arg port;
		^super.new.port_(port)
	}
	
	*doAction{arg src, status, a, b, c;
		action.value(src, status, a, b, c);
		}
	*doNoteAction{arg src, chan, num, veloc;
		//note.postln;
		note.value(src, chan, num, veloc);
	}
	*doPolyTochAction{arg src, chan, num, val;
		polytouch.value(src, chan, num, val);
	}
	*doControlAction{arg src, chan, num, val;
		control.value(src, chan, num, val);
	}
	*doProgramAction{arg src, chan, val;
		program.value(src, chan, val);
	}
	*doTouchAction{arg src, chan, val;
		touch.value(src, chan, val);
	}
	*doBendAction{arg src, chan, val;
		bend.value(src, chan, val);
	}
	*connect{arg inport, device;
		var uid;
		if(device.isKindOf(MIDIEndPoint), {uid = device.uid});
		if(device.isKindOf(SimpleNumber), {
			if(device>=0, { 
				uid = MIDIClient.sources.at(device).uid
			},{
				uid = device;});
			});
		this.connectByUID(inport,uid);
		}
	*disconnect{arg inport, device;
		var uid;
		if(device.isKindOf(MIDIEndPoint), {uid = device.uid});
		if(device.isKindOf(SimpleNumber), {
			if(device>=0, { 
				uid = MIDIClient.sources.at(device).uid
			},{
				uid = device;});
			});
		this.disconnectByUID(inport,uid);
		}
	*connectByUID {arg inport, uid;
		_ConnectMIDIIn		
	}
	*disconnectByUID {arg inport, uid;
		_DisconnectMIDIIn		
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
	send {arg outport, uid, len, stat, chan, a=0, b=0, latency= 0.01; //in ms
		_SendMIDIOut		
	}
}
	