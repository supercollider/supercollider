
// see MIDIResponder help for all classes on this page

// rather than hog the midi inputs here, make a global hog for all piglets to suckle from

MIDIResponder {
	var	<>function,
		<>matchEvent;		// for matching ports, channels, and parameters
	init { arg install;
		if(this.class.initialized.not,{ this.class.init });
		if(install,{this.class.add(this);});
	}
	remove {
		this.class.remove(this)
	}
	*removeAll { this.init }
	*fixSrc { |src|
			// low numbers are not uid's, but indices to sources
		(src.size > 0).if({
			^src.collect(this.fixSrc(_))
		});
		(src.isNumber and: { src < MIDIClient.sources.size }).if({			^MIDIClient.sources[src].uid
		});
		^src
	}
}

ResponderArray {
	var <>array;
	*new { arg array;
		^super.new.array_(array)
	}
	add { arg thing;
		array = array.add(thing)
	}
	value { arg ... vals;
		array.do({ arg resp; resp.performList(\value,vals) })
	}
	remove { arg resp;
		array.remove(resp);
		^array.size
	}
}

NoteOnResponder : MIDIResponder {
	classvar <norinit = false,<nonr;

	*new { arg function, src, chan, num, veloc, install=true;
		^super.new.function_(function)
			.matchEvent_(MIDIEvent(nil, this.fixSrc(src), chan, num, veloc))
			.init(install)
	}
	*initialized { ^norinit }
	*responders { ^nonr }
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		norinit = true;
		nonr = [];
		MIDIIn.noteOn = { arg src, chan, num, veloc;
			nonr.do({ arg r;
				if(r.matchEvent.match(src, chan, num, veloc))
					{ r.value(src,chan,num,veloc); };
			});
		}
	}
	value { arg src,chan,note,veloc;
		function.value(src, chan, note, veloc)
	}
	*add { arg resp;
		nonr = nonr.add(resp);
	}
	*remove { arg resp;
		nonr.remove(resp);
	}		
}

NoteOffResponder : NoteOnResponder {
	classvar <noffinit = false,<noffr;

	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		noffinit = true;
		noffr = [];
		MIDIIn.noteOff = { arg src, chan, num, veloc;
			noffr.do({ arg r;
				if(r.matchEvent.match(src, chan, num, veloc))
					{ r.value(src,chan,num,veloc); };
			});
		}
	}
	*initialized { ^noffinit }
	*responders { ^noffr }

	*add { arg resp;
		noffr = noffr.add(resp);
	}
	*remove { arg resp;
		noffr.remove(resp);
	}
}

CCResponder : MIDIResponder {
	classvar <ccinit = false,<ccr,<ccnumr;

	*new { arg function, src, chan, num, value, install=true;
		^super.new.function_(function)
			.matchEvent_(MIDIEvent(nil, this.fixSrc(src), chan, num, value))
			.init(install)
	}
	*initialized { ^ccinit }
	*responders { ^ccnumr.select(_.notNil).flat ++ ccr }
	value { arg src,chan,ccnum,val;
		function.value(src,chan,ccnum,val);
	}
	*add { arg resp;
		var temp;
		if(this.initialized.not,{ this.init });
		if((temp = resp.matchEvent.ctlnum).isNumber) {
			ccnumr[temp] = ccnumr[temp].add(resp);
		} {
			ccr = ccr.add(resp);
		};
	}
	*remove { arg resp;
		var temp;
		if((temp = resp.matchEvent.ctlnum).isNumber) {
			ccnumr[temp].remove(resp)
		} {
			ccr.remove(resp);
		};
	}
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		ccinit = true;
		ccr = [];
		ccnumr = Array.newClear(128);
		MIDIIn.control = { arg src,chan,num,val;
				// no need to check for nil because Nil-do does nothing
			ccnumr[num].do({ |r|
				if(r.matchEvent.match(src, chan, num, val))
					{ r.value(src,chan,num,val) };
			});
			ccr.do({ arg r;
				if(r.matchEvent.match(src, chan, num, val))
					{ r.value(src,chan,num,val) };
			});
		};
	}
}

TouchResponder : MIDIResponder {
	classvar <touchinit = false,<touchr;

	*new { arg function, src, chan, value, install=true;
		^super.new.function_(function)
			.matchEvent_(MIDIEvent(nil, this.fixSrc(src), chan, nil, value))
			.init(install)
	}
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		touchinit = true;
		touchr = [];
		MIDIIn.touch = { arg src, chan, val;
			touchr.do({ arg r;
				if(r.matchEvent.match(src, chan, nil, val))
					{ r.value(src,chan,val) };
			});
		}
	}
	value { arg src,chan,val;
		function.value(src,chan,val);
	}
	*initialized { ^touchinit }
	*responders { ^touchr }

	*add { arg resp;
		touchr = touchr.add(resp);
	}
	*remove { arg resp;
		touchr.remove(resp);
	}
}

BendResponder : TouchResponder {
	classvar <bendinit = false,<bendr;

	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connect });
		bendinit = true;
		bendr = [];
		MIDIIn.bend = { arg src, chan, val;
			bendr.do({ arg r;
				if(r.matchEvent.match(src, chan, nil, val))
					{ r.value(src,chan,val) };
			});
		}
	}
	*initialized { ^bendinit }
	*responders { ^bendr }

	*add { arg resp;
		bendr = bendr.add(resp);
	}
	*remove { arg resp;
		bendr.remove(resp);
	}
}

/*
PolyTouchResponder
*/

