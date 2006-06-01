
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
			"WARNING:".postln;
			("MIDIClient-init requested " ++ inports ++ " inport(s) and " ++ outports
				++ " outport(s),").postln;
			("but found only " ++ sources.size ++ " inport(s) and " ++ destinations.size
				++ " outport(s).").postln;
			"Some expected MIDI devices may not be available.".postln;
		});
		("Sources: "
			++ sources.collect({ |x| x.device ++ " : " ++ x.name })
			++ "\nDestinations: "
			++ destinations.collect({ |x| x.device ++ " : " ++ x.name })
		).postln;
	}
	*list {
		var list;
		list = this.prList;
		if(list.notNil, {
			sources = list.at(0).collect({ arg id,i;
				MIDIEndPoint(list.at(1).at(i), list.at(2).at(i), id)
			});
			destinations = list.at(3).collect({arg id, i;
				MIDIEndPoint(list.at(5).at(i), list.at(4).at(i), id)
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


MIDIEvent {
	var <>status, <>port, <>chan, <>b, <>c, <>thread;
	
	*new { arg status, port, chan, b, c, thread;
		^super.newCopyArgs(status, port, chan, b, c, thread)
	}
	set { arg inStatus, inPort, inChan, inB, inC, inThread;
		status = inStatus;
		port = inPort;
		chan = inChan;
		b = inB;
		c = inC;
		inThread !? { thread = inThread };
	}
	match { arg inPort, inChan, inB, inC;
		^port.matchItem(inPort) and: {
			chan.matchItem(inChan) and: {
			b.matchItem(inB) and: {
			c.matchItem(inC)
		}}}
	}
	// convenience accessors
	note { ^b }
	veloc { ^c }
	ctlnum { ^b }
	ctlval { ^c }
}

MIDIIn {
	var <>port;
	classvar <>action, 
	<> noteOn, <> noteOff, <> polytouch, 
	<> control, <> program, 
	<> touch, <> bend, 
	<> sysex, sysexPacket, <> sysrt, <> smpte;	
	
	classvar 
	<> noteOnList, <> noteOffList, <> polyList, 
	<> controlList, <> programList, 
	<> touchList, <> bendList;
	
	
	*waitNoteOn { arg port, chan, note, veloc;
		var event;
		event = MIDIEvent(\noteOn, port, chan, note, veloc, thisThread);
		noteOnList = noteOnList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitNoteOff { arg port, chan, note, veloc;
		var event;
		event = MIDIEvent(\noteOff, port, chan, note, veloc, thisThread);
		noteOffList = noteOffList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitPoly { arg port, chan, note, veloc;
		var event;
		event = MIDIEvent(\poly, port, chan, note, veloc, thisThread);
		polyList = polyList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitTouch { arg port, chan, val;
		var event;
		event = MIDIEvent(\touch, port, chan, val, nil, thisThread);
		touchList = touchList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitControl { arg port, chan, num, val;
		var event;
		event = MIDIEvent(\control, port, chan, num, val, thisThread);
		controlList = controlList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitBend { arg port, chan, val;
		var event;
		event = MIDIEvent(\bend, port, chan, val, nil, thisThread);
		bendList = bendList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitProgram { arg port, chan, num;
		var event;
		event = MIDIEvent(\program, port, chan, num, nil, thisThread);
		programList = programList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	
	*doAction { arg src, status, a, b, c;
		action.value(src, status, a, b, c);
	}
	*doNoteOnAction { arg src, chan, num, veloc;
		noteOn.value(src, chan, num, veloc);
		this.prDispatchEvent(noteOnList, \noteOn, src, chan, num, veloc);
	}
	*doNoteOffAction { arg src, chan, num, veloc;
		noteOff.value(src, chan, num, veloc);
		this.prDispatchEvent(noteOffList, \noteOff, src, chan, num, veloc);
	}
	*doPolyTouchAction { arg src, chan, num, val;
		polytouch.value(src, chan, num, val);
		this.prDispatchEvent(polyList, \poly, src, chan, num, val);
	}
	*doControlAction { arg src, chan, num, val;
		control.value(src, chan, num, val);
		this.prDispatchEvent(controlList, \control, src, chan, num, val);
	}
	*doProgramAction { arg src, chan, val;
		program.value(src, chan, val);
		this.prDispatchEvent(programList, \program, src, chan, val);
	}
	*doTouchAction { arg src, chan, val;
		touch.value(src, chan, val);
		this.prDispatchEvent(touchList, \touch, src, chan, val);
	}
	*doBendAction { arg src, chan, val;
		bend.value(src, chan, val);
		this.prDispatchEvent(bendList, \bend, src, chan, val);
	}
	
	*doSysexAction { arg src,  packet;
		sysexPacket = sysexPacket ++ packet;
		if (packet.last == -9, {
			sysex.value(src, sysexPacket);
			sysexPacket = nil
		});
	}
	*doSysrtAction { arg src, index, val;
		sysrt.value(src, index, val);
	}
	
	*doSMPTEaction { arg src, frameRate, timecode;
		smpte.value(src, frameRate, timecode);
	}
	
	*connect { arg inport=0, device=0;
		var uid,source;
		if(device.isNumber, {
			if(device >= 0, {
				if(MIDIClient.initialized.not,{ MIDIClient.init });
				source = MIDIClient.sources.at(device);
				if(source.isNil,{ 
					"MIDIClient failed to init".warn;
				},{
					uid = MIDIClient.sources.at(device).uid;
				})
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
	
	*prDispatchEvent { arg eventList, status, port, chan, b, c;
		var selectedEvents;
		eventList ?? {^this};
		eventList.takeThese {| event |
			if (event.match(port, chan, b, c)) 
			{
				selectedEvents = selectedEvents.add(event);
				true
			}
			{ false };
		};
		selectedEvents.do{ |event|
				event.set(status, port, chan, b, c);
				event.thread.next;
		}
	}
}
	
MIDIOut {
	var <>port, <> uid;
	
	*new { arg port, uid;
		^super.newCopyArgs(port, uid);
	}
		
	write { arg len, hiStatus, loStatus, a=0, b=0;
		this.send(port, uid, len, hiStatus, loStatus, a, b);
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
	bend { arg chan, val=8192;
		this.write(3, 16rE0, chan, val bitAnd: 127, val >> 7);
	}
	allNotesOff { arg chan;
		this.control(chan, 123, 0);
	}
	smpte	{ arg frames=0, seconds=0, minutes=0, hours=0, frameRate = 3;
	var packet;
		packet = [frames, seconds, minutes, hours]
			.collect({ arg v, i; [(i * 2 << 4) | (v & 16rF), (i * 2 + 1 << 4) | (v >> 4) ] });
		packet = packet.flat;
		packet.put(7, packet.at(7) | ( frameRate << 1 ) );
		packet.do({ arg v; this.write(2, 16rF0, 16r01, v); });
	}
	songPtr { arg songPtr;
		this.write(4, 16rF0, 16r02, songPtr & 16r7f, songPtr >> 7 & 16r7f);
	}
	songSelect { arg song;
		this.write(3, 16rF0, 16r03, song);
	}	
	midiClock { 
		this.write(1, 16rF0, 16r08);
	}
	start {
		this.write(1, 16rF0, 16r0A);
	}
	continue {
		this.write(1, 16rF0, 16r0B);
	}
	stop {
		this.write(1, 16rF0, 16r0C);
	}
	reset {
		this.write(1, 16rF0, 16r0F);
	}
	
	send {arg outport, uid, len, hiStatus, loStatus, a=0, b=0, latency=0.1; //in ms
		_SendMIDIOut		
	}
	
	sysex { arg uid, packet;
		_SendSysex
	}
}
