
// see MIDIPlayer for all help files

MIDIPlayer : SynthlessPlayer { // InterfacePlayer, Control

	var <>value=0.0,<>spec;
	var resp;
	
	topMakePatchOut { arg group,bundle;
		patchOut = UpdatingScalarPatchOut(this);
	}
	synthArg { ^this.poll }
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
}

MIDIGatePlayer : MIDIPlayer {
	var off;
	
	*new { arg spec=\amp;
		^super.new.spec_(spec.asSpec)
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
		^super.new.num_(num).spec_(spec.asSpec)
	}
	storeArgs { ^[num,spec] }
	initResponders {
		resp = CCResponder(num,{ arg val; value = spec.map(val / 127.0); this.changed });
	}
}

MIDIFreqPlayer : MIDIPlayer {
	initResponders {
		resp = NoteOnResponder({ arg note,veloc; value = note.midicps; this.changed });
	}
}

// MIDINotePlayer(spec)
// MIDIVelocityPlayer(spec)


