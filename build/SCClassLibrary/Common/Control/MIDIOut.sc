MIDIClient {
	classvar < initialized=false;
	*init {arg inports, outports;
		initialized = true;
		this.prInit(inports,outports);
	}
	*prInit {arg inports, outports;
		_InitMIDI;
	}
	*list {
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
	*connect {arg inport, uid;
		_ConnectMIDIIn		
	}
	*disconnect {arg inport, uid;
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
	send {arg outport, uid, len, stat, chan, a=0, b=0, latency=5000.0; //in ms
		_SendMIDIOut		
	}
}
	