
// see MIDIPlayer for all help files

MIDIPlayer : SynthlessPlayer { // InterfacePlayer, Control

	var <>value=0.0,<>spec;
	
	topMakePatchOut { arg group,bundle;
		patchOut = ScalarPatchOut(this);
	}
	synthArg { ^this.poll }
	instrArgFromControl { arg control;
		^control
	}

	didSpawn { arg patchIn,synthi;
		patchOut.connectTo(patchIn,false);
		// am i already connected to this client ?
		if(this.dependants.includes(patchOut.updater).not,{
			patchOut.updater = 
				Updater(this,{
					patchIn.value = value;
				});
		});
		this.connect;
	}
	free {
		if(patchOut.notNil and: {patchOut.updater.notNil},{
			patchOut.updater.remove;
			patchOut.updater = nil;
		});
		this.disconnect;
	}
}

MIDIGatePlayer : MIDIPlayer {
	var on,off;
	
	*new { arg spec=\amp;
		^super.new.spec_(spec.asSpec)
	}
	storeArgs { ^[spec] }
	connect {
		on = NoteOnResponder({ arg note,veloc; value = spec.map(veloc / 127.0); this.changed });
		off = NoteOffResponder({ arg note,veloc; value = 0.0; this.changed; });
	}
	disconnect {
		on.remove;
		off.remove;
	}
		
}

CCPlayer : MIDIPlayer {
	
	var <>num, resp;
	
	*new { arg num=1,spec=\linear;
		^super.new.num_(num).spec_(spec.asSpec)
	}
	storeArgs { ^[num,spec] }
	connect {
		resp = CCResponder(num,{ arg val; value = spec.map(val / 127.0); this.changed });
	}
	disconnect {
		resp.remove;
	}
}

MIDIFreqPlayer : MIDIPlayer {
	var resp;
	connect {
		resp = NoteOnResponder({ arg note,veloc; value = note.midicps; this.changed });
	}
	disconnect {
		resp.remove;
	}
}




