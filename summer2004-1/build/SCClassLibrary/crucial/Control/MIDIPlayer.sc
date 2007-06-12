
// see MIDIPlayer for all help files

MIDIPlayer : SynthlessPlayer { // InterfacePlayer, Control

	var <>value=0.0,<>spec;
	var resp;
	
	topMakePatchOut { arg group,bundle;
		patchOut = UpdatingScalarPatchOut(this);
	}
	synthArg { ^this.poll }
	addToSynthDef {  arg synthDef,name;
		// value doesn't matter so much, we are going to pass in a real live one
		synthDef.addKr(name,this.synthArg ? 0);
	}
	instrArgFromControl { arg control;
		^control
	}
	
	didSpawn {
		super.didSpawn;
		this.initResponders;
	}

	removeResponders {
		resp.remove;
	}
	freeToBundle { arg bundle;
		bundle.addAction(this,\removeResponders);
	}
	rate { ^\control }
}

MIDIGatePlayer : MIDIPlayer {
	var off;
	
	*new { arg spec=\amp;
		^super.new.spec_(spec = spec.asSpec).value_(0.0)
	}
	storeArgs { ^[spec] }
	initResponders {
		resp = NoteOnResponder({ arg note,veloc; value = spec.map(veloc / 127.0); this.changed });
		off = NoteOffResponder({ arg note,veloc; value = 0.0; this.changed; });
	}
	removeResponders {
		resp.remove;
		off.remove;
	}	
}

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

MIDIFreqPlayer : MIDIPlayer {
	*new { arg init=440.0; ^super.new.value_(init) }
	initResponders {
		resp = NoteOnResponder({ arg note,veloc; value = note.midicps; this.changed });
	}
}

// MIDINotePlayer(spec,scale)
// MIDIVelocityPlayer(spec)


