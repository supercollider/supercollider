
Editor {
	var  <>action, // { arg value,theEditor; }
		<>value, <patchOut;
	
	guiClass { ^ObjectGui }

	storeOn { arg stream;
		value.storeOn(stream)
	}
	
	next { ^this.value }// Object would return this
	poll { ^value }
	embedInStream { arg inval; ^this.asStream.embedInStream(inval); }
	asStream { ^FuncStream(this) }
	
	setPatchOut { arg po; patchOut = po }
	makePatchOut {
		patchOut = ScalarPatchOut(this)
	}
	synthArg { ^this.poll }
	instrArgFromControl { arg control;
		^control
	}
	
	editWithCallback { arg callback;
		ModalDialog({ arg layout;
			this.gui(layout);
		},{
			callback.value(value);
		})
	}
	set { arg val;
		this.activeValue_(val);
	}
	activeValue_ { arg val;
		this.value_(val);
		action.value(value);
	}

}

NumberEditor : Editor {
	
	var <spec;
	
	*new { arg value=1.0,spec='amp';
		^super.new.init(value,spec)
	}
	init { arg val,aspec;
		spec = aspec.asSpec ?? {ControlSpec.new};
		this.value_(spec.constrain(val));
	}
	spec_ { arg aspec;
		spec = aspec.asSpec;
		value = spec.constrain(value);
		this.changed(\spec);
	}
	setUnmappedValue { arg unipolar;
		value = spec.map(unipolar);
		this.changed;
		^value
	}
	unmappedValue {
		^spec.unmap(value)
	}
	numChannels { ^1 }

	addToSynthDef { arg synthDef,name;
		synthDef.addInstrOnlyArg(name,this.synthArg)
	}
	instrArgFromControl { arg control;
		^value
	}
	rate { ^\scalar }

	guiClass { ^NumberEditorGui }

}

KrNumberEditor : NumberEditor { 
	classvar <>defaultLag = 0.1;
 	var <>lag;
 
	init { arg val,aspec;
		super.init(val, aspec);
		lag = defaultLag;
	}
	rate { ^\control }
 	canDoSpec { arg aspec; ^aspec.isKindOf(ControlSpec) }

	addToSynthDef {  arg synthDef,name;
		synthDef.addKr(name,this.synthArg);
	}
	instrArgFromControl { arg control;
		// should request a LagControl
		// either way it violates the contract
		// by putting the player inside the patch
		if(lag.notNil,{
			^Lag.kr(control,lag)
		},{
			^control
		})
	}
	makePatchOut { 
		patchOut = UpdatingScalarPatchOut(this,enabled: false);
	}
	connectToPatchIn { arg patchIn,needsValueSetNow = true;
		patchOut.connectTo(patchIn,needsValueSetNow);
	}
	stopToBundle { arg b; b.addFunction({ patchOut.free; patchOut = nil; }); }
	freePatchOut { arg bundle;
		bundle.addFunction({ patchOut.free; patchOut = nil; })
	}
	
	guiClass { ^KrNumberEditorGui }

}

IrNumberEditor : NumberEditor {
	rate { ^\control } // irate icontrol ?
	addToSynthDef {  arg synthDef,name;
		synthDef.addIr(name,this.synthArg);
	}
	instrArgFromControl { arg control;
		^control
	}
}


// paul.crabbe@free.fr
PopUpEditor : KrNumberEditor {

	var <>labels, <>values,<selectedIndex;
	
	*new { arg initVal,labels,values;
		^super.new.pueinit(initVal,values,labels)
	}
	init {} // should be a shared superclass above this
			// not using spec or constrain
	pueinit { arg initVal,v,l;
		value = initVal;
		if(l.isNil,{
			values = v ?? { Array.series(10,0,0.1) };
			labels = (l ? values).collect({ arg l; l.asString });
		},{
			values = v ?? { Array.series(labels.size,0,1) };
			labels = l.collect({ arg l; l.asString });
		});	
		selectedIndex = values.indexOf(value) ? 0;
	}
	value_ { arg val;
		value = val;
		selectedIndex = values.indexOf(value) ? 0;
	}
	selectByIndex { arg index;
		value = values.at(index);
		selectedIndex = index;
	}
	guiClass { ^PopUpEditorGui }
}

IntegerEditor : NumberEditor {

	value_ { arg val,changer;
		value = val.asInteger;
		this.changed(\value,changer);
	}
}

BooleanEditor : NumberEditor {

	*new { arg val=false;
		^super.new.value_(val)
	}
	// value returns true/false
	instrArgFromControl { arg control;
		^value
	}
	guiClass { ^BooleanEditorGui }
}

