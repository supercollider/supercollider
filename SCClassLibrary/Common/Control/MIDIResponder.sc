
// see MIDIResponder help for all classes on this page


MIDIResponder : AbstractResponder {
	var	<>action,<>swallowEvent=false,
		<>matchEvent;		// for matching ports, channels, and parameters
	init { arg install;
		if(this.class.initialized.not,{ this.class.init });
		matchEvent.port = matchEvent.port.asMIDIInPortUID;
		if(install,{this.add;});
	}
	respond { arg src,chan,num,value;
		if(this.match(src,chan,num,value),{
			this.value(src,chan,num,value)
			^swallowEvent
		});
		^false;
	}
	match { arg src,chan,num,value;
		^matchEvent.match(src,chan,num,value);
	}
	value { arg src,chan,a,b;
		action.value(src, chan, a, b)
	}
	
	function {
		this.deprecated(thisMethod, this.findMethod(\action));
		^action
	}
	
	function_ {|function|
		this.deprecated(thisMethod, this.findMethod(\action_));
		this.action_(function);
	}
	
	*add {arg resp; resp.add }

	*remove {arg resp;
		resp.remove;
	}
	*removeAll {
		if(this == MIDIResponder,{
			this.allSubclasses.do({ |responderClass| responderClass.removeAll })
		},{
			this.init
		})
	}
}



NoteOnResponder : MIDIResponder {
	classvar <norinit = false,<nonr;

	*new { arg action, src, chan, num, veloc, install=true, 
			swallowEvent=false, removeOnCmdPeriod=true;
		^super.new.action_(action)
			.matchEvent_(MIDIEvent(nil, src, chan, num, veloc))
			.swallowEvent_(swallowEvent)
			.removeOnCmdPeriod_(removeOnCmdPeriod)
			.init(install)
	}
	*initialized { ^norinit }
	*responders { ^nonr }
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connectAll });
		norinit = true;
		nonr = [];
		MIDIIn.noteOn = { arg src, chan, note, veloc;
			nonr.any({ arg r;
				r.respond(src,chan,note,veloc)
			});
		}
	}
	add { 
		if(removeOnCmdPeriod, {CmdPeriod.add(this)});
		added = true;
		nonr = nonr.add(this);
	}
	remove {
		if(removeOnCmdPeriod, {CmdPeriod.remove(this)});
		added = false;
		nonr.remove(this);
	}
	learn {
		var oneShot;
		oneShot = this.class.new({ |src,chan,num,value|
					this.matchEvent_(MIDIEvent(nil,src,chan,nil,nil));
					oneShot.remove;
				},nil,nil,nil,nil,true,true)
	}
}

NoteOffResponder : NoteOnResponder {
	classvar <noffinit = false,<noffr;

	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connectAll });
		noffinit = true;
		noffr = [];
		MIDIIn.noteOff = { arg src, chan, note, veloc;
			noffr.any({ arg r;
				r.respond(src,chan,note,veloc)
			});
		}
	}
	*initialized { ^noffinit }
	*responders { ^noffr }

	add {
		if(removeOnCmdPeriod, {CmdPeriod.add(this)});
		added = true;
		noffr = noffr.add(this);
	}
	remove {
		if(removeOnCmdPeriod, {CmdPeriod.remove(this)});
		added = false;
		noffr.remove(this);
	}
}

CCResponder : MIDIResponder {
	classvar <ccinit = false,<ccr,<ccnumr;

	*new { arg action, src, chan, num, value, install=true, 
			swallowEvent=false, removeOnCmdPeriod=true;
		^super.new.action_(action).swallowEvent_(swallowEvent)
			.matchEvent_(MIDIEvent(nil, src, chan, num, value))
			.removeOnCmdPeriod_(removeOnCmdPeriod)
			.init(install)
	}
	*initialized { ^ccinit }
	*responders { ^ccnumr.select(_.notNil).flat ++ ccr }
	add { 
		var temp;
		if(this.class.initialized.not,{ this.class.init });
		if(removeOnCmdPeriod, {CmdPeriod.add(this)});
		added = true;
		if((temp = matchEvent.ctlnum).isNumber) {
			ccnumr[temp] = ccnumr[temp].add(this);
		} {
			ccr = ccr.add(this);
		};
	}
	remove {
		var temp;
		if(removeOnCmdPeriod, {CmdPeriod.remove(this)});
		added = false;
		if((temp = matchEvent.ctlnum).isNumber) {
			ccnumr[temp].remove(this)
		} {
			ccr.remove(this);
		};
	}
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connectAll });
		ccinit = true;
		ccr = [];
		ccnumr = Array.newClear(128);
		MIDIIn.control = { arg src,chan,num,val;
			// first try cc num specific
			// then try non-specific (matches any cc )
			[ccnumr[num], ccr].any({ |stack|
				stack.notNil and: {stack.any({ |r| r.respond(src,chan,num,val) })}
			})
		};
	}
	learn {
		var oneShot;
		oneShot = CCResponder({ |src,chan,num,value|
					this.matchEvent_(MIDIEvent(nil,src,chan,num,nil));
					oneShot.remove;
				},nil,nil,nil,nil,true,true)
	}

	matchEvent_ { |midiEvent|
			// if ctlnum changes from non-number to number, or vice versa,
			// this responder is going to move between ccr and ccnumr
		if(matchEvent.notNil and:
				{ matchEvent.ctlnum.isNumber !== midiEvent.ctlnum.isNumber })
		{
			this.remove;
			matchEvent = midiEvent;
			this.add;
		} {
			matchEvent = midiEvent;
		}
	}
}

TouchResponder : MIDIResponder {
	classvar <touchinit = false,<touchr;

	*new { arg action, src, chan, value, install=true, 
			swallowEvent=false, removeOnCmdPeriod=true;
		^super.new.action_(action).swallowEvent_(swallowEvent)
			.matchEvent_(MIDIEvent(nil, src, chan, nil, value))
			.removeOnCmdPeriod_(removeOnCmdPeriod)
			.init(install)
	}
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connectAll });
		touchinit = true;
		touchr = [];
		MIDIIn.touch = { arg src, chan, val;
			touchr.any({ arg r;
				r.respond(src,chan,nil,val)
			})
		}
	}
	value { arg src,chan,num,val;
		// num is irrelevant
		action.value(src,chan,val);
	}
	*initialized { ^touchinit }
	*responders { ^touchr }

	add {
		if(removeOnCmdPeriod, {CmdPeriod.add(this)});
		added = true;
		touchr = touchr.add(this);
	}
	remove {
		if(removeOnCmdPeriod, {CmdPeriod.remove(this)});
		added = false;
		touchr.remove(this);
	}
	learn {
		var oneShot;
		oneShot = this.class.new({ |src,chan,num,value|
					this.matchEvent_(MIDIEvent(nil,src,chan,nil,nil));
					oneShot.remove;
				},nil,nil,nil,true,true)
	}
}

BendResponder : TouchResponder {
	classvar <bendinit = false,<bendr;

	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connectAll });
		bendinit = true;
		bendr = [];
		MIDIIn.bend = { arg src, chan, val;
			bendr.any({ arg r;
				r.respond(src,chan,nil,val)
			});
		}
	}
	*initialized { ^bendinit }
	*responders { ^bendr }

	add { 
		if(removeOnCmdPeriod, {CmdPeriod.add(this)});
		added = true;
		bendr = bendr.add(this);
	}
	remove {
		if(removeOnCmdPeriod, {CmdPeriod.remove(this)});
		added = false;
		bendr.remove(this);
	}
}

/*
NoteOnOffResponder
	the note on action would return an object which is stored.
	when a matching note off event occurs, the object is passed into the note off action
PolyTouchResponder
*/



ProgramChangeResponder : MIDIResponder {
	classvar <pcinit = false,<pcr;

	*new { arg action, src, chan, value, install=true, removeOnCmdPeriod=true;
		^super.new.action_(action)
			.matchEvent_(MIDIEvent(nil, src.asMIDIInPortUID, chan, nil, value))
			.removeOnCmdPeriod_(removeOnCmdPeriod)
			.init(install)
	}
	*init {
		if(MIDIClient.initialized.not,{ MIDIIn.connectAll });
		pcinit = true;
		pcr = [];
		MIDIIn.program = { arg src, chan, val;
			pcr.do({ arg r;
				if(r.matchEvent.match(src, chan, nil, val))
					{ r.value(src,chan,val) };
			});
		}
	}
	value { arg src,chan,val;
		action.value(src,chan,val);
	}
	*initialized { ^pcinit }
	*responders { ^pcr }

	add {
		if(removeOnCmdPeriod, {CmdPeriod.add(this)});
		added = true;
		pcr = pcr.add(this);
	}
	remove {
		if(removeOnCmdPeriod, {CmdPeriod.remove(this)});
		added = false;
		pcr.remove(this);
	}
}

