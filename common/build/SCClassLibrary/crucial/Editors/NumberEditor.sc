
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
	stopToBundle { arg b; this.freePatchOutToBundle(b); }
	freePatchOutToBundle { arg bundle;
		bundle.addFunction({ patchOut.free; patchOut = nil; })
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
		this.activeValue_(val).changed;
	}
	activeValue_ { arg val;
		this.value_(val);
		action.value(value);
	}
	spec { ^thisMethod.subclassResponsibility }
	copy { ^this.class.new.value_(value.copy) }

}

NumberEditor : Editor {

	var <spec;

	*new { arg value=1.0,spec='amp';
		^super.new.init(value,spec)
	}
	init { arg val,aspec;
		spec = (aspec.asSpec ?? {ControlSpec.new});
		this.value_(spec.constrain(val));
	}
	spec_ { arg aspec;
		spec = aspec.asSpec;
		this.value = spec.constrain(value);
		this.changed(\spec);
	}
	value_ { |v,change=true|
		value = spec.constrain(v);
		if(change,{ this.changed });
	}
	setUnmappedValue { arg unipolar,change=true;
		value = spec.map(unipolar);
		if(change,{this.changed});
		^value
	}
	unmappedValue {
		^spec.unmap(value)
	}
	rand { arg standardDeviation = 0.15,mean;
		// gaussian centered on the spec default
		var default,unmapped;
		default = spec.unmap( mean ? spec.default );
		unmapped = (sqrt(-2.0 * log(1.0.rand)) * sin(2pi.rand) * standardDeviation + default);
		this.setUnmappedValue(unmapped);
	}
	numChannels { ^1 }

	addToSynthDef { arg synthDef,name;
		synthDef.addInstrOnlyArg(name,this.synthArg)
	}
	instrArgFromControl { arg control;
		^value
	}
	rate { ^spec.rate } // probably scalar

	copy { ^this.class.new(value,spec) }
	guiClass { ^NumberEditorGui }

}

KrNumberEditor : NumberEditor {
	classvar <>defaultLag = 0.1;
 	var <>lag;

	init { arg val,aspec;
		super.init(val, aspec);
		if(spec.isKindOf(NoLagControlSpec).not,{
			lag = defaultLag;
		});
	}

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
	copy { ^this.class.new(value,spec).lag_(lag) }

	guiClass { ^KrNumberEditorGui }

}

IrNumberEditor : NumberEditor {
	init { arg val,aspec;
		super.init(val, aspec);
		if((spec.isKindOf(ScalarSpec) or: spec.isKindOf(StaticSpec)).not,{
			// let's assume you are lazy and specifying \freq
			// we are going to guess you are using this for i_freq
			spec = spec.as(ScalarSpec);
			// if you wanted it for patterns or instr then use a simple NumberEditor
		});
	}
	rate { ^spec.rate } // \scalar or \static
	addToSynthDef {  arg synthDef,name;
		synthDef.addIr(name,this.synthArg);
	}
	instrArgFromControl { arg control;
		^control
	}
	makePatchOut {
		patchOut = ScalarPatchOut(this);
	}
	connectToPatchIn { } // nothing doing.  we are ir only
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
	// this means this could only be a noncontrol
	// i should change this
	instrArgFromControl { arg control;
		^value
	}
	value_ { |v|
		value = v;
	}

	guiClass { ^BooleanEditorGui }
}

