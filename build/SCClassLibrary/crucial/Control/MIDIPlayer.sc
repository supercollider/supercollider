
// see MIDIPlayer for all help files
// abstract class

MIDIPlayer : SynthlessPlayer { // InterfacePlayer, Control

	var <>value=0.0,<>spec;
	var resp;

	topMakePatchOut { arg group,bundle;
		patchOut = UpdatingScalarPatchOut(this);
	}
	synthArg { ^this.poll }
	addToSynthDef {  arg synthDef,name;
		// the value doesn't matter, its a placeholder,
		// we are going to pass in a real one during actual play
		synthDef.addKr(name,this.synthArg ? 0);
	}
	instrArgFromControl { arg control;
		^control
	}

	didSpawn {
		super.didSpawn;
		CmdPeriod.add(this);
		this.initResponders;
	}

	removeResponders {
		resp.remove;
	}
	stopToBundle { arg bundle;
		super.stopToBundle(bundle);
		bundle.addFunction({ CmdPeriod.remove(this) });
	}
	freeToBundle { arg bundle;
		super.freeToBundle(bundle);
		bundle.addMessage(this,\removeResponders);
	}
	cmdPeriod { this.free; CmdPeriod.remove(this); }
	rate { ^\control }
}

// abstract class
MIDIHoldsNotes : MIDIPlayer {
	var off,heldNotes;
	removeResponders {
		resp.remove;
		off.remove;
	}
}

// does not release until the last key is released
MIDIGatePlayer : MIDIHoldsNotes {

	*new { arg spec=\amp;
		^super.new.spec_(spec = spec.asSpec).value_(0.0)
	}
	storeArgs { ^[spec] }
	initResponders {
		resp = NoteOnResponder({ arg note,veloc;
			heldNotes = heldNotes.add(note); value = spec.map(veloc / 127.0); this.changed;
		});
		off = NoteOffResponder({ arg note,veloc;
			heldNotes.remove(note);
			if(heldNotes.isEmpty,{
				value = 0.0; this.changed;
			});
		});
	}
}

// implements classic analog monophonic style including trills
MIDIFreqPlayer : MIDIHoldsNotes {
	*new { arg init=440.0; ^super.new.value_(init) }
	initResponders {
		resp = NoteOnResponder({ arg note,veloc;
				heldNotes = heldNotes.add(note);
				value = note.midicps;
				this.changed
			});
		off = NoteOffResponder({ arg note,veloc;
				heldNotes.remove(note);
				if(heldNotes.notEmpty,{
					value = heldNotes.last.midicps;
					this.changed;
				});
			});
	}
}
/*
	as a midi responder
		2,3,4 note chord function
			functions for each note
*/

CCPlayer : MIDIPlayer {

	var <>num;

	*new { arg num=1,spec=\linear;
		^super.new.num_(num).spec_(spec = spec.asSpec).value_(spec.default)
	}
	storeArgs { ^[num,spec] }
	initResponders {
		resp = CCResponder(num,{ arg val; value = spec.map(val / 127.0); this.changed });
	}
}

/**
CatchingCCPlayer : MIDIPlayer {
	// for knobs: waits for you to find the position before it 'catches'

	var <>num,>lastRawVal;

	*new { arg num=1,spec=\linear;
		^super.new.num_(num).spec_(spec = spec.asSpec).value_(spec.default)
	}
	storeArgs { ^[num,spec] }
	initResponders {
		lastRawVal = spec.unmap(value) * 127.0;
		resp = CCResponder(num,{ arg val;
				if((val - lastRawVal).abs < 10,{
					lastRawVal = val;
					value = spec.map(val / 127.0);
					this.changed;
				})
			});
	}
}
**/


// MIDINotePlayer(spec,scale)
// MIDIVelocityPlayer(spec)


