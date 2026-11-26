MIDIEndPoint {
	var <>device, <>name, <>uid;
	*new{ |device, name, uid|
		^super.newCopyArgs(device, name, uid)
	}
	printOn { |stream|
		stream << this.class.name << "(" <<<*
			[device, name] << ")"
	}
}

MIDIClient {
	classvar <myinports, <myoutports; // for linux it is useful to keep track of how many we open ourselves
	classvar <sources, <destinations;
	classvar <initialized = false;
	*init { |inports, outports, verbose = true| // by default initialize all available ports
		// you still must connect to them using MIDIIn.connect
		this.disposeClient; // remove old sources and destinations, in case init is called repeatedly
		this.prInitClient;
		this.list;
		if(inports.isNil) { inports = sources.size };
		if(outports.isNil) { outports = destinations.size };
		// this.disposeClient;

		this.prInit(inports, outports);
		initialized = true;
		// might ask for 1 and get 2 if your device has it
		// or you might ask for a 1 and get 0 of nothing is plugged in
		// so we warn you
		if(sources.size < inports or: { destinations.size < outports }) {
			"WARNING:".postln;
			("MIDIClient-init requested " ++ inports ++ " inport(s) and " ++ outports
					++ " outport(s),").postln;
			("but found only " ++ sources.size ++ " inport(s) and " ++ destinations.size
					++ " outport(s).").postln;
			"Some expected MIDI devices may not be available.".postln
		};
		myinports = inports;
		myoutports = outports;

		this.list;

		ShutDown.add { this.disposeClient };

		if(verbose) {
			Post << "MIDI Sources:" << Char.nl;
			sources.do { |x| Post << Char.tab << x << Char.nl };
			Post << "MIDI Destinations:" << Char.nl;
			destinations.do { |x| Post << Char.tab << x << Char.nl }
		}
	}
	*list {
		var list;
		list = this.prList;
		if(list.notNil) {
			sources = list.at(0).collect { |id, i|
				MIDIEndPoint(list.at(1).at(i), list.at(2).at(i), id)
			};
			destinations = list.at(3).collect { |id, i|
				MIDIEndPoint(list.at(5).at(i), list.at(4).at(i), id)
			}
		}
	}
	*prInit { |inports, outports|
		_InitMIDI
		^this.primitiveFailed
	}
	*prInitClient {
		_InitMIDIClient
		^this.primitiveFailed
	}
	*prList {
		_ListMIDIEndpoints
		^this.primitiveFailed
	}
	*disposeClient {
		this.prDisposeClient;
		initialized = false
	}
	*prDisposeClient {
		_DisposeMIDIClient
		^this.primitiveFailed
	}
	*restart {
		_RestartMIDI
		^this.primitiveFailed
	}

	// overridden in Linux:
	*externalSources{
		^sources
	}

	*externalDestinations{
		^destinations
	}

	*getClientID {
		if(thisProcess.platform.name != \linux) {
			warn("% is only implemented in linux,"
				"and should never be called directly in user code."
				.format(thisMethod)
			)
		};
		^nil
	}
}


MIDIEvent {
	var <>status, <>port, <>chan, <>b, <>c, <>thread;

	*new { |status, port, chan, b, c, thread|
		^super.newCopyArgs(status, port, chan, b, c, thread)
	}
	set { |inStatus, inPort, inChan, inB, inC, inThread|
		status = inStatus;
		port = inPort;
		chan = inChan;
		b = inB;
		c = inC;
		inThread !? { thread = inThread }
	}
	match { |inPort, inChan, inB, inC|
		^port.matchItem(inPort) and: {
			chan.matchItem(inChan) and: {
				b.matchItem(inB) and: {
					c.matchItem(inC)
		} } }
	}
	// convenience accessors
	note { ^b }
	veloc { ^c }
	ctlnum { ^b }
	ctlval { ^c }
}

MIDIIn {
	var port;
	classvar <>action,
		<> noteOn, <> noteOff, <> polytouch,
		<> control, <> program,
		<> touch, <> bend,
		<> sysex, sysexPacket, <> sysrt, <> smpte, <> invalid;

	classvar
		<> noteOnList, <> noteOffList, <> polyList,
		<> controlList, <> programList,
		<> touchList, <> bendList;

	classvar
		<> noteOnZeroAsNoteOff = true;

	// safer than global setters
	*addFuncTo { |what, func|
		this.perform(what.asSetter, this.perform(what).addFunc(func))
	}

	*removeFuncFrom { |what, func|
		this.perform(what.asSetter, this.perform(what).removeFunc(func))
	}

	*replaceFuncTo { |what, func, newFunc|
		this.perform(what.asSetter, this.perform(what).replaceFunc(func, newFunc))
	}

	*waitNoteOn { |port, chan, note, veloc|
		var event;
		event = MIDIEvent(\noteOn, port, chan, note, veloc, thisThread);
		noteOnList = noteOnList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitNoteOff { |port, chan, note, veloc|
		var event;
		event = MIDIEvent(\noteOff, port, chan, note, veloc, thisThread);
		noteOffList = noteOffList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitPoly { |port, chan, note, veloc|
		var event;
		event = MIDIEvent(\poly, port, chan, note, veloc, thisThread);
		polyList = polyList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitTouch { |port, chan, val|
		var event;
		event = MIDIEvent(\touch, port, chan, val, nil, thisThread);
		touchList = touchList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitControl { |port, chan, num, val|
		var event;
		event = MIDIEvent(\control, port, chan, num, val, thisThread);
		controlList = controlList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitBend { |port, chan, val|
		var event;
		event = MIDIEvent(\bend, port, chan, val, nil, thisThread);
		bendList = bendList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}
	*waitProgram { |port, chan, num|
		var event;
		event = MIDIEvent(\program, port, chan, num, nil, thisThread);
		programList = programList.add(event); // add to waiting list
		nil.yield; // pause the thread.
		^event
	}

	*doAction { |src, status, a, b, c|
		action.value(src, status, a, b, c)
	}
	*doNoteOnAction { |src, chan, num, veloc|
		if(noteOnZeroAsNoteOff and: (veloc == 0)) {
			noteOff.value(src, chan, num, veloc);
			this.prDispatchEvent(noteOffList, \noteOff, src, chan, num, veloc)
		} {
			noteOn.value(src, chan, num, veloc);
			this.prDispatchEvent(noteOnList, \noteOn, src, chan, num, veloc)
		}
	}
	*doNoteOffAction { |src, chan, num, veloc|
		noteOff.value(src, chan, num, veloc);
		this.prDispatchEvent(noteOffList, \noteOff, src, chan, num, veloc)
	}
	*doPolyTouchAction { |src, chan, num, val|
		polytouch.value(src, chan, num, val);
		this.prDispatchEvent(polyList, \poly, src, chan, num, val)
	}
	*doControlAction { |src, chan, num, val|
		control.value(src, chan, num, val);
		this.prDispatchEvent(controlList, \control, src, chan, num, val)
	}
	*doProgramAction { |src, chan, val|
		program.value(src, chan, val);
		this.prDispatchEvent(programList, \program, src, chan, val)
	}
	*doTouchAction { |src, chan, val|
		touch.value(src, chan, val);
		this.prDispatchEvent(touchList, \touch, src, chan, val)
	}
	*doBendAction { |src, chan, val|
		bend.value(src, chan, val);
		this.prDispatchEvent(bendList, \bend, src, chan, val)
	}

	*doSysexAction { |src, packet|
		sysexPacket = sysexPacket ++ packet;
		if(packet.last == -9) {
			sysex.value(src, sysexPacket);
			sysexPacket = nil
		}
	}
	*doInvalidSysexAction { |src, packet|
		invalid.value(src, packet)
	}

	*doSysrtAction { |src, index, val|
		sysrt.value(src, index, val)
	}

	*doSMPTEaction { |src, frameRate, timecode|
		smpte.value(src, frameRate, timecode)
	}

	*findPort { |deviceName, portName|
		^MIDIClient.sources.detect({ |endPoint| endPoint.device == deviceName and: { endPoint.name == portName } })
	}

	*disconnectAll {
		if(MIDIClient.initialized) {
			MIDIClient.externalSources.do { |src, i|
				MIDIIn.disconnect(i, src)
			}
		}
	}

	*connectAll { |verbose = true|
		if(MIDIClient.initialized.not)
			{ MIDIClient.init(verbose: verbose) }
			{ MIDIIn.disconnectAll; MIDIClient.list };
		MIDIClient.externalSources.do { |src, i|
			MIDIIn.connect(i, src)
		}
	}

	*connect { |inport = 0, device = 0|
		var uid, source;
		if(MIDIClient.initialized.not) { MIDIClient.init };
		if(device.isNumber) {
			if(device >= 0) {
				if(device > MIDIClient.sources.size) { // on linux the uid's are very large numbers
					source = MIDIClient.sources.detect { |it| it.uid == device };
					if(source.isNil) {
						("MIDI device with uid" + device + "not found").warn
					} {
						uid = source.uid
					}
				} {
					source = MIDIClient.sources.at(device);
					if(source.isNil) {
						"MIDIClient failed to init".warn
					} {
						uid = MIDIClient.sources.at(device).uid
					}
				}
			} { // elsewhere they tend to be negative
				uid = device
			}
		} {
			if(device.isKindOf(MIDIEndPoint)) { uid = device.uid } // else error
		};
		this.connectByUID(inport, uid)
	}
	*disconnect { |inport = 0, device = 0|
		var uid, source;
		if(device.isKindOf(MIDIEndPoint)) { uid = device.uid };
		if(device.isNumber) {
			if(device.isPositive) {
				if(device > MIDIClient.sources.size) {
					source = MIDIClient.sources.select { |it| it.uid == device }.first;
					if(source.isNil) {
						("MIDI device with uid" + device + "not found").warn
					} {
						uid = source.uid
					}
				} {
					source = MIDIClient.sources.at(device);
					if(source.isNil) {
						"MIDIClient failed to init".warn
					} {
						uid = MIDIClient.sources.at(device).uid
					}
				}
			} {
				uid = device
			}
		};
		this.disconnectByUID(inport, uid)
	}
	*connectByUID { |inport, uid|
		_ConnectMIDIIn
		^this.primitiveFailed
	}
	*disconnectByUID { |inport, uid|
		_DisconnectMIDIIn
		^this.primitiveFailed
	}

	*prDispatchEvent { |eventList, status, port, chan, b, c|
		var selectedEvents;
		eventList ?? { ^this };
		eventList.takeThese { |event|
			if(event.match(port, chan, b, c)) {
				selectedEvents = selectedEvents.add(event);
				true
			} { false }
		};
		selectedEvents.do { |event|
			event.set(status, port, chan, b, c);
			event.thread.next
		}
	}
}

MIDIOut {
	var <>port, <>uid, <>latency = 0.2;

	*new { |port, uid|
		if(thisProcess.platform.name != \linux) {
			^super.newCopyArgs(port, uid ?? { MIDIClient.destinations[port].uid })
		} {
			^super.newCopyArgs(port, uid ?? 0)
		}
	}
	*newByName { |deviceName, portName, dieIfNotFound = true|
		var endPoint, index;
		endPoint = MIDIClient.destinations.detect { |ep, epi|
			index = epi;
			ep.device == deviceName and: { ep.name == portName }
		};
		if(endPoint.isNil) {
			if(dieIfNotFound) {
				Error("Failed to find MIDIOut port " + deviceName + portName).throw
			} {
				("Failed to find MIDIOut port " + deviceName + portName).warn
			}
		};
		if(thisProcess.platform.name != \linux) {
			^this.new(index, endPoint.uid)
		} {
			if(index < MIDIClient.myoutports) {
				// 'index' here is for "SuperCollider:out0", ":out1" etc.
				// Practically speaking, this establishes associations:
				// out0 --> destinations[0]
				// out1 --> destinations[1] and so on.
				// It looks weird but, in fact, it does ensure a 1-to-1 connection.
				// Explained further in MIDIOut help.
				^this.new(index, endPoint.uid)
			} {
				// If you didn't initialize enough MIDI output ports,
				// it will connect the new device to 0.
				// Connections with a UID are always 1-to-1.
				^this.new(0, endPoint.uid)
			}
		}
	}
	*findPort { |deviceName, portName|
		^MIDIClient.destinations.detect { |endPoint|
			endPoint.device == deviceName and: { endPoint.name == portName }
		}
	}

	write { |len, hiStatus, loStatus, a = 0, b = 0|
		this.send(port, uid, len, hiStatus, loStatus, a, b, latency)
	}

	noteOn { |chan, note = 60, veloc = 64|
		this.write(3, 16r90, chan.asInteger, note.asInteger, veloc.asInteger)
	}
	noteOff { |chan, note = 60, veloc = 64|
		this.write(3, 16r80, chan.asInteger, note.asInteger, veloc.asInteger)
	}
	polyTouch { |chan, note = 60, val = 64|
		this.write(3, 16rA0, chan.asInteger, note.asInteger, val.asInteger)
	}
	control { |chan, ctlNum = 7, val = 64|
		this.write(3, 16rB0, chan.asInteger, ctlNum.asInteger, val.asInteger)
	}
	program { |chan, num = 1|
		this.write(2, 16rC0, chan.asInteger, num.asInteger)
	}
	touch { |chan, val = 64|
		this.write(2, 16rD0, chan.asInteger, val.asInteger)
	}
	bend { |chan, val = 8192|
		val = val.asInteger;
		this.write(3, 16rE0, chan, val bitAnd: 127, val >> 7)
	}
	allNotesOff { |chan|
		this.control(chan, 123, 0)
	}
	smpte	{ |frames = 0, seconds = 0, minutes = 0, hours = 0, frameRate = 3|
		var packet;
		packet = [frames, seconds, minutes, hours]
			.asInteger
			.collect { |v, i| [(i * 2 << 4) | (v & 16rF), (i * 2 + 1 << 4) | (v >> 4)] };
		packet = packet.flat;
		packet.put(7, packet.at(7) | (frameRate << 1));
		packet.do { |v| this.write(2, 16rF0, 16r01, v) }
	}
	songPtr { |songPtr|
		songPtr = songPtr.asInteger;
		this.write(4, 16rF0, 16r02, songPtr & 16r7f, songPtr >> 7 & 16r7f)
	}
	songSelect { |song|
		this.write(3, 16rF0, 16r03, song.asInteger)
	}
	midiClock {
		this.write(1, 16rF0, 16r08)
	}
	start {
		this.write(1, 16rF0, 16r0A)
	}
	continue {
		this.write(1, 16rF0, 16r0B)
	}
	stop {
		this.write(1, 16rF0, 16r0C)
	}
	reset {
		this.write(1, 16rF0, 16r0F)
	}

	sysex { |packet|
		^this.prSysex(uid, packet)
	}

	send { |outport, uid, len, hiStatus, loStatus, a = 0, b = 0, late|
		_SendMIDIOut
		^this.primitiveFailed
	}

	prSysex { |uid, packet|
		_SendSysex
		^this.primitiveFailed
	}
}
