
/*
	sc3d5 (os9) Instrument system from which crucial Instr derived.
	the gui system this uses is not currently working.
	
	
*/


Instrument {
	var <>name, ugenFuncDef, <>specs, <>windowMaker, <presets, <patches;

	*new { arg name, ugenFunc, specs = #[];//, windowMaker;
		^super.newCopyArgs(name, ugenFunc.def, specs
				//, windowMaker ?? { SimpleInstrumentWindowMaker.new }
				).init;
	}
	
	ugenFunc { ^ugenFuncDef.asFunction }
	
	// PRIVATE:
//	play { arg synthID;
//		#{ arg name, synthID;
//			var instrument;
//			instrument = Library.at(\instruments, name);
//			
//			{
//				Environment.use({
//					var func;
//					~instrument = instrument;
//					instrument.specs.do({ arg spec;
//						spec.addToEnvir;
//					});
//					instrument.ugenFunc.valueEnvir;
//				});
//			}.play(id: synthID); 
//			
//		}.send(1, name, synthID);
//	}
	
//	makeWindow { arg voice;
//		^windowMaker.makeWindow(voice);
//	}
//	createParams {
//		specs = windowMaker.createParams(this);
//	}
	
	init {
		//this.createParams;
		Library.put(\instruments, name, this);
//		this.loadPresets;
	}
	
//	presetFileName {
//		^":Presets:" ++ name.asString ++ ".presets";
//	}
	
//	savePreset { arg presetName, settings;
//		presets.put(presetName, settings);
//		this.writePresets;
//	}
//	savePatch { arg patchName, patch;
//		patches.put(patchName, patch);
//		this.writePresets;
//	}
//	writePresets {
//		[presets, patches].writeArchive( this.presetFileName );
//	}
//	loadPresets {
//		var presetFileName;
//		presetFileName = this.presetFileName;
//		if (File.exists(presetFileName), {
//			#presets, patches = Object.readArchive(presetFileName);
//		},{
//			presets = IdentityDictionary.new;
//			patches = IdentityDictionary.new;
//		});
//	}

}


/*
InstrumentParam {
	var <>guiName;
	makeLabel {
		^CXLabel( nil, nil, guiName )
	}
	makeNumberView { arg control;
		^NumberView( Pen( foreColor: Color.black, textSize: 12 ), control: control )
	}
	addToEnvir {}
	makeControlValue {}
}

InstrumentInputParam : InstrumentParam {
	var <>inputID;
	
	*new { arg guiName, inputID;
		guiName = guiName ? inputID;
		^super.newCopyArgs(guiName, inputID);
	}
	addToEnvir {
		var voice;
		currentEnvironment.put(inputID, 0.0);
	}
	addToGUI { arg voice;
		^HLayout(nil, [ 
			CXLabel( nil, nil, voice.inputs.at(inputID).instrument.name ),
			Button(nil, nil, [["Set",Color.black,Color.guiGrey,{ 0 }]]), 
			Button(nil, nil, [["Edit",Color.black,Color.guiGrey,{ 0 }]]), 
			CXLabel(nil,nil,inputID.asString)
		])
	}
	makeControlValue { arg voice;
		voice.inputs.put(inputID, InstrumentVoice(\default));
	}
}

InstrumentLabelParam : InstrumentParam {
	var <>argName;
	
	*new { arg guiName = "";
		^super.newCopyArgs(guiName);
	}
	addToEnvir {
		var voice;
		currentEnvironment.put(argName, 0.0);
	}
	addToGUI { arg voice;
		^CLabel( nil, nil, guiName )
	}
}

InstrumentControlParam : InstrumentParam {
	var <>plugID, <>spec, <>initialValue;
		
	*new { arg guiName, plugID, spec, initialValue;
		spec = spec.asControlSpec;
		guiName = guiName ? plugID;
		initialValue = initialValue ? spec.default;
		^super.newCopyArgs(guiName, plugID, spec, initialValue);
	}
	addToEnvir {
		currentEnvironment.put(plugID, ControlSpace.kr(plugID)) 
	}
	makeControlValue { arg voice;
		ControlSpace.put(voice.synthID, plugID, initialValue);
		voice.controls.put(plugID, ControlValue(spec, voice.synthID, plugID));
	}
}

InstrumentRangeParam : InstrumentParam {
	var <>loPlugID, <>hiPlugID, <>spec, <>initialLoValue, <>initialHiValue;
		
	*new { arg guiName, loPlugID, hiPlugID, spec, initialLoValue, initialHiValue;
		initialLoValue = initialLoValue ? spec.minVal;
		initialHiValue = initialHiValue ? spec.maxVal;
		^super.newCopyArgs(guiName, loPlugID, hiPlugID, spec, initialLoValue, initialHiValue);
	}
	addToEnvir {
		currentEnvironment.put(loPlugID, ControlSpace.kr(loPlugID));
		currentEnvironment.put(hiPlugID, ControlSpace.kr(hiPlugID));
	}
	makeControlValue { arg voice;
		ControlSpace.put(voice.synthID, loPlugID, initialLoValue);
		ControlSpace.put(voice.synthID, hiPlugID, initialHiValue);
		voice.controls.put(loPlugID, ControlValue(spec, voice.synthID, loPlugID));
		voice.controls.put(hiPlugID, ControlValue(spec, voice.synthID, hiPlugID));
	}
	addToGUI { arg voice;
		var loControl, hiControl, control;
		loControl = voice.controls.at(loPlugID);
		hiControl = voice.controls.at(hiPlugID);
		control = MultiControlValue([loControl, hiControl]);
		^HLayout(nil, [ 
			this.makeNumberView( loControl ),
			this.makeNumberView( hiControl ),
			RangeSlider( control: control ), 
			this.makeLabel
		])
	}
}

InstrumentInputSliderParam : InstrumentControlParam {
	var <>inputID;
	*new { arg guiName, inputID, plugID, spec, initialValue;
		^super.new(guiName, plugID, spec, initialValue).inputID_(inputID);
	}
	addToGUI { arg voice;
		var control;
		control = voice.controls.at(plugID);
		^HLayout(nil, [ 
			HLayout(nil, [ 
				Button(nil, nil, [["Set",Color.black,Color.guiGrey,{ 0 }]]), 
				Button(nil, nil, [["Edit",Color.black,Color.guiGrey,{ 0 }]])
			]),
			this.makeNumberView(control),
			Slider( control: control, layoutSize: LayoutSize( weight: 2 ) ), 
			CXLabel( nil, nil, voice.inputs.at(inputID).instrument.name )
		])
	}
	makeControlValue { arg voice;
		super.makeControlValue(voice);
		voice.inputs.put(inputID, InstrumentVoice(\sine));
	}
	addToEnvir {
		super.addToEnvir;
		currentEnvironment.put(inputID, 0.0);
	}
}

InstrumentSliderParam : InstrumentControlParam {
	addToGUI { arg voice;
		var control;
		control = voice.controls.at(plugID);
		^HLayout(nil, [ 
			this.makeNumberView(control),
			Slider( control: control, layoutSize: LayoutSize( weight: 2 ) ), 
			this.makeLabel
		])
	}
}

InstrumentKnobParam : InstrumentControlParam {
	addToGUI { arg voice;
		var control;
		control = voice.controls.at(plugID);
		^HLayout(nil, [ 
			this.makeNumberView(control),
			Knob( control: control, layoutSize: LayoutSize( weight: 2 ) ), 
			this.makeLabel
		])
	}
}

InstrumentNumericalParam : InstrumentControlParam {
}

InstrumentVoice {
	var <instrument, <window, <synthID, <controls, <inputs, <>playing = false;
	
	*new { arg instrumentName;
		^super.new.init(instrumentName);
	}
	init { arg instrumentName;
		instrument = Library.at(\instruments, instrumentName);
		synthID = UniqueID.next;
		this.makeControls;
	}
		
	play {
		playing = true;
		instrument.play( synthID );
	}
	stopPlaying {
		playing = false;
		Synth.sendFadeEnd( 1, synthID );
	}
	makeWindow {
		instrument.makeWindow(this);
	}
	makeControls {
		controls = IdentityDictionary.new;
		inputs = IdentityDictionary.new;
		instrument.specs.do({ arg spec;
			spec.makeControlValue(this);
		});
	}
	makeInstrumentInputSpec {
		//^InstrumentInputSpec(instrument.name, settings, inputs)
	}
	randomSetting {
		controls.do({ arg control;
			control.set(1.0.rand);
		});
	}
	
	saveSetting {
		var presetName, dict;
		presetName = String.fromUser("enter Preset name :", "");
		if (presetName.isNil, { ^this });
		dict = this.toDict;
		instrument.savePreset(presetName, dict);
	}
	recallSetting {
	}
	
	fromDict { arg dict;
		controls.do({ arg control;
			var val;
			val = dict.at(control.plugID);
			if (val.notNil, { control.setMapped(val); });
		});
	}
	toDict {
		var dict;
		dict = IdentityDictionary.new;
		controls.do({ arg control;
			dict.put(control.plugID, control.getMapped);
		});
		^dict		
	}
	
	makePatch {
		var inputPatches;
		inputPatches = inputs.collect({ arg input; input.makePatch });
		^InstrumentPatch(instrument.name, this.toDict, inputPatches);
	}
}

InstrumentPatch {
	var <>name, <>settings, <>inputs;
	
	*new { arg name, settings, inputs;
		^super.new.copyArgs( name, settings, inputs )
	}
	
	makeVoice {
		var voice;
		voice = InstrumentVoice(name);
		voice.fromDict(settings)
		^voice
	}
}
*/



/*
SimpleInstrumentWindowMaker
{	
	createParams { arg instrument;
		^instrument.specs.collect({ arg msg; this.perform(msg); });
	}
	createParamViews { arg voice;
		if (voice.controls.size > 0, {
			^[ CLabel(nil,nil,"Parameters", LayoutSize( weight: 0.5 )) ]
			++ 
			voice.instrument.specs.collect({ arg spec; spec.addToGUI(voice); })
		},{ 
			^nil
		})
	}
	createPresetViews { arg voice, color;
		// if has no controls then no presets.
		// if has no inputs then no patches.
		^HLayout(nil,[
			Button(nil, nil, [
				["Save", Color.black, color, { voice.saveSetting; 0 }]
			]),
			Button(nil, nil, [
				["Recall", Color.black, color, { voice.recallSetting; 0 }]
			]),
			Button(nil, nil, [
				["Save", Color.black, color, { voice.saveSetting; 0 }]
			]),
			Button(nil, nil, [
				["Recall", Color.black, color, { voice.recallSetting; 0 }]
			])
		])		
	}
	makeWindow { arg voice;
		var screen, instrument, bounds, color;
		
		instrument = voice.instrument;
		if (instrument.specs.isNil, { ^nil });
		bounds = Rect(80,80, 480, 80 + ((instrument.specs.size + 3) * 24));
		//color = Color(rrand(160,210), rrand(160,210), rrand(160,210));
		color = Color.guiGrey;
		screen = GUIScreen.new(instrument.name, bounds,
			LView(Pen(foreColor: Color.black, backColor: color), 
				[
				LView(Pen(action: \erase), bounds.moveTo(0,0)),
				VLayout(Pen(textFont: \Gadget, textSize: 12), 
					[
						HLayout(nil,[
							Button(nil, nil, [
								["Start", Color.black, Color.green, {
									voice.play;
									1
								}],
								["Stop", Color.black, Color.red, { voice.stopPlaying; 0 }]
							], state: if(voice.playing,1,0)),
							Button(nil, nil, [
								["Random", Color.black, color, {
									voice.randomSetting;
									0
								}]
							], state: if(voice.playing,1,0))
						]),
						HLayout(nil,[
							CLabel(nil, nil, "Presets"),
							CLabel(nil, nil, "Patches")
						], layoutSize: LayoutSize( weight: 0.5 ))
					]
					++ this.createPresetViews(voice, color)
					++ this.createParamViews(voice)
				)
			])
		);
		screen.onClose = { 
			Synth.sendFadeEnd(1, voice.synthID); 
			ControlSpace.removeAt(voice.synthID); // wipe out the synth's control dictionary.
		};
		screen.refresh;
	}

	range { arg guiName, loPlugID, hiPlugID, spec, initialLoValue, initialHiValue;
		^InstrumentRangeParam(guiName, loPlugID, hiPlugID, spec, initialLoValue, initialHiValue);
	}
	slider { arg guiName, plugID, spec, initialValue;
		^InstrumentSliderParam(guiName, plugID, spec, initialValue);
	}
	knob { arg guiName, plugID, spec, initialValue;
		^InstrumentKnobParam(guiName, plugID, spec, initialValue);
	}
	numerical { arg guiName, plugID, spec, initialValue;
		^InstrumentNumericalParam(guiName, plugID, spec, initialValue);
	}
	input { arg guiName, argName;
		^InstrumentInputParam(guiName, argName);
	}
	inputSlider { arg guiName, plugID, spec, initialValue;
		^InstrumentInputSliderParam(guiName, plugID, spec, initialValue);
	}
	label { arg guiName, argName;
		^InstrumentLabelParam(guiName);
	}
}

*/

/*


RTVM
InstrumentRT {
	var <>name, <>ugenFunc, <>plugNames, <>inputNames;
}

InstrumentNRT {
	var <>name, <>specs, <presets, <patches, <>windowMaker;
}

ugenFunc
create the plugs

NRTVM

ModuleDef
	name ugenFunc windowMaker presets patches
	
ModuleObj
	def		ModuleDef
	controls	IdentDict[ Symbol -> ControlValue ]  
	inputs 	IdentDict[ Symbol -> ModuleObj ]
	window 	
	playing 
	synthID
---
	makeWindow
	play
	spawn ? 


ModuleMemento
	name settings inputs

*/

