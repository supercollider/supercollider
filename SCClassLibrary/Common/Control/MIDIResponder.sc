// see MIDIResponder help for all classes on this page


MIDIResponder {
	var	<>function, <>swallowEvent = false,
		<>matchEvent;		// for matching ports, channels, and parameters
	init { |install|
		if(this.class.initialized.not) { this.class.init };
		matchEvent.port = matchEvent.port.asMIDIInPortUID;
		if(install) { this.class.add(this) }
	}
	respond { |src, chan, num, value|
		if(this.match(src, chan, num, value)) {
			this.value(src, chan, num, value)
			^swallowEvent
		};
		^false
	}
	match { |src, chan, num, value|
		^matchEvent.match(src, chan, num, value)
	}
	value { |src, chan, a, b|
		function.value(src, chan, a, b)
	}

	remove {
		this.class.remove(this)
	}
	*removeAll {
		if(this == MIDIResponder) {
			this.allSubclasses.do { |responderClass| responderClass.removeAll }
		} {
			this.init
		}
	}
}



NoteOnResponder : MIDIResponder {
	classvar <norinit = false, <nonr;

	*new { |function, src, chan, num, veloc, install = true, swallowEvent = false|
		num = if(num.isNumber, { num.asInteger }, num);
		num = if(num.isCollection, { num.collect(_.asInteger) }, num);
		^super.new.function_(function)
			.matchEvent_(MIDIEvent(nil, src, chan, num, veloc))
			.swallowEvent_(swallowEvent)
			.init(install)
	}
	*initialized { ^norinit }
	*responders { ^nonr }
	*init {
		if(MIDIClient.initialized.not) { MIDIIn.connectAll };
		if(norinit.not) {
			MIDIIn.addFuncTo(\noteOn, { |src, chan, note, veloc|
				nonr.any({ |r|
					r.respond(src, chan, note, veloc)
				})
			})
		};
		norinit = true;
		nonr = []
	}
	*add { |resp|
		nonr = nonr.add(resp)
	}
	*remove { |resp|
		nonr.remove(resp)
	}
	learn {
		var oneShot;
		oneShot = this.class.new({ |src, chan, num, value|
			this.matchEvent_(MIDIEvent(nil, src, chan, nil, nil));
			oneShot.remove
		}, nil, nil, nil, nil, true, true)
	}
}

NoteOffResponder : NoteOnResponder {
	classvar <noffinit = false, <noffr;

	*init {
		if(MIDIClient.initialized.not) { MIDIIn.connectAll };
		if(noffinit.not) {
			MIDIIn.addFuncTo(\noteOff, { |src, chan, note, veloc|
				noffr.any({ |r|
					r.respond(src, chan, note, veloc)
				})
			})
		};
		noffinit = true;
		noffr = []
	}
	*initialized { ^noffinit }
	*responders { ^noffr }

	*add { |resp|
		noffr = noffr.add(resp)
	}
	*remove { |resp|
		noffr.remove(resp)
	}
}

CCResponder : MIDIResponder {
	classvar <ccinit = false, <ccr, <ccnumr;

	*new { |function, src, chan, num, value, install = true, swallowEvent = false|
		num = if(num.isNumber, { num.asInteger }, num);
		num = if(num.isCollection, { num.collect(_.asInteger) }, num);
		^super.new.function_(function).swallowEvent_(swallowEvent)
			.matchEvent_(MIDIEvent(nil, src, chan, num, value))
			.init(install)
	}
	*initialized { ^ccinit }
	*responders { ^ccnumr.select(_.notNil).flat ++ ccr }
	*add { |resp|
		var temp;
		if(this.initialized.not) { this.init };
		if((temp = resp.matchEvent.ctlnum).isNumber) {
			ccnumr[temp] = ccnumr[temp].add(resp)
		} {
			ccr = ccr.add(resp)
		}
	}
	*remove { |resp|
		var temp;
		if((temp = resp.matchEvent.ctlnum).isNumber) {
			ccnumr[temp].remove(resp)
		} {
			ccr.remove(resp)
		}
	}
	*init {
		if(MIDIClient.initialized.not) { MIDIIn.connectAll };
		if(ccinit.not) {
			MIDIIn.addFuncTo(\control, { |src, chan, num, val|
				// first try cc num specific
				// then try non-specific (matches any cc )
				[ccnumr[num], ccr].any({ |stack|
					stack.notNil and: { stack.any({ |r| r.respond(src, chan, num, val) }) }
				})
			})
		};
		ccinit = true;
		ccr = [];
		ccnumr = Array.newClear(128)
	}
	learn {
		var oneShot;
		oneShot = CCResponder({ |src, chan, num, value|
			this.matchEvent_(MIDIEvent(nil, src, chan, num, nil));
			oneShot.remove
		}, nil, nil, nil, nil, true, true)
	}

	matchEvent_ { |midiEvent|
		// if ctlnum changes from non-number to number, or vice versa,
		// this responder is going to move between ccr and ccnumr
		if(matchEvent.notNil and:
			{ matchEvent.ctlnum.isNumber !== midiEvent.ctlnum.isNumber }) {
				this.remove;
				matchEvent = midiEvent;
				this.class.add(this)
			} {
				matchEvent = midiEvent
			}
	}
}

TouchResponder : MIDIResponder {
	classvar <touchinit = false, <touchr;

	*new { |function, src, chan, value, install = true, swallowEvent = false|
		^super.new.function_(function).swallowEvent_(swallowEvent)
			.matchEvent_(MIDIEvent(nil, src, chan, nil, value))
			.init(install)
	}
	*init {
		if(MIDIClient.initialized.not) { MIDIIn.connectAll };
		if(touchinit.not) {
			MIDIIn.addFuncTo(\touch, { |src, chan, val|
				touchr.any({ |r|
					r.respond(src, chan, nil, val)
				})
			})
		};
		touchinit = true;
		touchr = []
	}
	value { |src, chan, num, val|
		// num is irrelevant
		function.value(src, chan, val)
	}
	*initialized { ^touchinit }
	*responders { ^touchr }

	*add { |resp|
		touchr = touchr.add(resp)
	}
	*remove { |resp|
		touchr.remove(resp)
	}
	learn {
		var oneShot;
		oneShot = this.class.new({ |src, chan, num, value|
			this.matchEvent_(MIDIEvent(nil, src, chan, nil, nil));
			oneShot.remove
		}, nil, nil, nil, true, true)
	}
}

BendResponder : TouchResponder {
	classvar <bendinit = false, <bendr;

	*init {
		if(MIDIClient.initialized.not) { MIDIIn.connectAll };
		if(bendinit.not) {
			MIDIIn.addFuncTo(\bend, { |src, chan, val|
				bendr.any({ |r|
					r.respond(src, chan, nil, val)
				})
			})
		};
		bendinit = true;
		bendr = []
	}
	*initialized { ^bendinit }
	*responders { ^bendr }

	*add { |resp|
		bendr = bendr.add(resp)
	}
	*remove { |resp|
		bendr.remove(resp)
	}
}

/*
NoteOnOffResponder
	the note on function would return an object which is stored.
	when a matching note off event occurs, the object is passed into the note off function
PolyTouchResponder
*/



ProgramChangeResponder : MIDIResponder {
	classvar <pcinit = false, <pcr;

	*new { |function, src, chan, value, install = true|
		^super.new.function_(function)
			.matchEvent_(MIDIEvent(nil, src.asMIDIInPortUID, chan, nil, value))
			.init(install)
	}
	*init {
		if(MIDIClient.initialized.not) { MIDIIn.connectAll };
		if(pcinit.not) {
			MIDIIn.addFuncTo(\program, { |src, chan, val|
				pcr.do { |r|
					if(r.matchEvent.match(src, chan, nil, val))
						{ r.value(src, chan, val) }
				}
			})
		};
		pcinit = true;
		pcr = []
	}
	value { |src, chan, val|
		function.value(src, chan, val)
	}
	*initialized { ^pcinit }
	*responders { ^pcr }

	*add { |resp|
		pcr = pcr.add(resp)
	}
	*remove { |resp|
		pcr.remove(resp)
	}
}
