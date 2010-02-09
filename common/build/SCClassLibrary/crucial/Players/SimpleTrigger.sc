
// thanks to Maurizio for help working on thisSimpleTrigger : SynthlessPlayer {	var <>action,<>spec,<value;		*new { arg spec=\trig;		^super.new.spec_(spec.asSpec).init	}	init {
		value = spec.maxval;
	}
	trig { arg val;
		action.value(this);
		value = val ? spec.maxval;
		this.changed(this);
	}

	synthArg { ^spec.minval }	rate { ^\control } 	addToSynthDef {  arg synthDef,name; 		synthDef.addTr(name,this.synthArg); 	} 	instrArgFromControl { arg control; ^control } 	makePatchOut { 		patchOut = UpdatingScalarPatchOut(this,enabled: false); 	} 	didStop {
		super.didStop;
		patchOut.free;
		patchOut = nil;
	}		guiClass { ^SimpleTriggerGui }}