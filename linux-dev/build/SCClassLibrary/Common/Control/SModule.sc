
/*
create a patch
open patch window
add by setting input.
open module window
*/

/*
SPatch
{
	var <>name, <>modules, <>sortedModules, <>unconnectedModules, <>connections;
	var <synthID, playing = false, <>window;
	
	*new { arg name;
		^super.newCopyArgs(name).init;
	}
	init {
		modules = [];
		connections = [];
		synthID = UniqueID.next;
	}
	makeWindow {
		var screen, bounds, color;
		
		if (window.notNil, { window.toFront },{
			bounds = Rect(80,80, 480, 80 + ((modules.size + 1) * 24));
			//color = Color(rrand(160,210), rrand(160,210), rrand(160,210));
			color = Color.guiGrey;
			screen = GUIScreen.new(name, bounds,
				LView(Pen(foreColor: Color.black, backColor: color), 
					[
					LView(Pen(action: \erase), bounds.moveTo(0,0)),
					VLayout(Pen(textFont: \Gadget, textSize: 12), 
						[
							HLayout(nil,[
								Button(nil, nil, [
									["Start", Color.black, Color.green, {
										this.play;
										1
									}],
									["Stop", Color.black, Color.red, { this.stopPlaying; 0 }]
								], state: if(playing,1,0)),
							])
						]
						++ this.addModuleViews(color).postln;
					)
				])
			);
			screen.onClose = { 
				Synth.sendFadeEnd(1, synthID); 
				ControlSpace.removeAt(synthID); // wipe out the synth's control dictionary.
			};
			screen.refresh;
		});
	}
	addModuleViews { arg color;
		^modules.collect({ arg module; module.makePatchView(color).postln });
	}
	
	makeSynth {
		Synth.new({
			var out;
			sortedModules.do({ arg module;
				out = module.makeUGens;
			});
			out;
		});
	}
	play { arg dur;
		playing = true;
		//this.makeSynth.play(dur);
	}
	stopPlaying {
		playing = false;
		Synth.sendFadeEnd( 1, synthID );
	}
	sortModules {
		_SPatch_TopologicalSort
		^this.primitiveFailed
	}
	disconnectAll {
		sortedModules.do({ arg module;
			module.disconnectAll;
		});
		connections = [];
	}
	add { arg module; 
		module.patchIndex = modules.size;
		modules = modules.add(module);
	}
}

SWire
{
	var fromModule, fromOutputID, value;
	
	getInput {
		if (fromModule.isNil, { ^nil });
		^fromModule.outputs.at(fromOutputID).value;
	}
	getInputModuleName {
		if (fromModule.isNil, { ^'zero' });
		^fromModule.name;
	}
}

SModuleDef
{
	var <name, <inputDefs, <outputDefs, <controlDefs, <func, <windowMaker;
	
	var presets;
	*new { arg name, inputDefs, outputDefs, controlDefs, func, windowMaker;
		^super.newCopyArgs(name, inputDefs, outputDefs, controlDefs, func, windowMaker).init;
	}
	init {
	}
	presetFileName {
		^":Presets:" ++ name.asString ++ ".presets";
	}
	writePresets {
		presets.writeArchive( this.presetFileName );
	}
	loadPresets {
		var presetFileName;
		presetFileName = this.presetFileName;
		if (File.exists(presetFileName), {
			presets = Object.readArchive(presetFileName);
		},{
			presets = IdentityDictionary.new;
		});
	}
	savePreset { arg presetName, settings;
		presets.put(presetName, settings);
		this.writePresets;
	}
	makeControlEnvir {
		controlDefs.do({ arg controlDef;
			controlDef.addToEnvir;
		});
	}
	makeUGens {
		func.value(this);
	}
}

SModule
{
	var <def, <patch, <>patchIndex;
	var <>inputs, <>outputs, <>controls, <>window;
	
	*new { arg def, patch;
		^super.newCopyArgs(def, patch).init;
	}
	init {
		patch.add(this);
					
		inputs = IdentityDictionary.new(def.inputDefs.size);
		if (def.inputDefs.notNil, {
			def.inputDefs.do({ arg inputID;	
				inputs.put(inputID, nil);
			});
		});
		
		outputs = IdentityDictionary.new(def.outputDefs.size);
		if (def.outputDefs.notNil, {
			def.outputDefs.do({ arg outputID;
				outputs.put(outputID, nil);
			});
		});
		
		controls = IdentityDictionary.new(def.controlDefs.size);
		if (def.controlDefs.notNil, {
			def.controlDefs.do({ arg def;
				var plugID, spec, initialValue;
				#plugID, spec, initialValue = def;
				controls.put(plugID, ControlValue(spec, patch.synthID, UniqueID.next, initialValue));
			});
		});
	}
		
	getInput { arg name;
		^inputs.at(name).getInput;
	}
	getInputModuleName { arg name;
		var input;
		input = inputs.at(name).postln;
		if (input.isNil, { ^'zero' });
		^inputs.at(name).getInputModuleName;
	}
	
	
	makeWindow { 
		if (window.notNil, { window.toFront },{
			window = def.windowMaker.value(this); 
		});
	}
	makePatchView { arg color;
		^HLayout(nil, [ 
			CXLabel(nil,nil, def.name, layoutSize: LayoutSize( weight: 4 )), 
			Button(nil, nil, [["Edit", Color.black, color, { this.makeWindow; 0 }]])
		]);
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
		def.savePreset(presetName, dict);
	}
	recallSetting {
	}

	fromDict { arg dict;
		controls.keysValuesDo({ arg key, control;
			var val;
			val = dict.at(key);
			if (val.notNil, { control.setMapped(val); });
		});
	}
	toDict {
		var dict;
		dict = IdentityDictionary.new;
		controls.keysValuesDo({ arg key, control;
			dict.put(key, control.getMapped);
		});
		^dict		
	}
	disconnectAll {
		inputs.keysValuesChange(nil); // set all dictionary entries to nil
	}
}

SModuleGUIDef
{
	var <>guiName;
	makeLabel { arg name;
		^CXLabel( nil, nil, name )
	}
	makeNumberView { arg control;
		^NumberView( Pen( foreColor: Color.black, textSize: 12 ), control: control )
	}
}


SInput : SModuleGUIDef {
	var <>inputID;
	
	*new { arg guiName, inputID;
		guiName = guiName ? inputID;
		^super.newCopyArgs(guiName, inputID);
	}
	value { arg module;
		^HLayout(nil, [ 
			CXLabel( nil, nil, module.getInputModuleName(inputID) ),
			Button(nil, nil, [["Set",Color.black, Color.guiGrey,{ 0 }]]), 
			CXLabel(nil, nil, guiName)
		])
	}
	addToEnvir { arg module;
		currentEnvironment.put(inputID, module.getInput(inputID)) 
	}
}

SSlider : SModuleGUIDef 
{
	var <>plugID;
	*new { arg guiName, plugID;
		guiName = guiName ? plugID;
		^super.newCopyArgs(guiName, plugID);
	}
	value { arg module;
		var control;
		control = module.controls.at(plugID);
		^HLayout(nil, [ 
			this.makeNumberView(control),
			Slider( control: control, layoutSize: LayoutSize( weight: 2 ) ), 
			this.makeLabel(guiName)
		])
	}
	addToEnvir { arg module;
		currentEnvironment.put(plugID, ControlSpace.kr(plugID)) 
	}
}

SModuleWindowMaker
{	
	var <>guidefs, <>color;
	
	*new { arg guidefs, color;
		^super.newCopyArgs(guidefs, color)
	}	
	value { arg module;
		var def, screen, bounds, numcontrols;
		
		def = module.def;
		numcontrols = guidefs.size;
		if (numcontrols == 0, { ^nil });
		bounds = Rect(80,80, 480, 80 + ((numcontrols + 2) * 24));
		color = color ?? { Color(rrand(120,210), rrand(120,210), rrand(120,210)).postln };
		//color = color ? Color.guiGrey;
		screen = GUIScreen.new(def.name, bounds,
			LView(Pen(foreColor: Color.black, backColor: color), 
				[
				LView(Pen(action: \erase), bounds.moveTo(0,0)),
				VLayout(Pen(textFont: \Gadget, textSize: 12), 
					[
						HLayout(nil,[
							Button(nil, nil, [
								["Save Preset", Color.black, color, 
									{ module.saveSetting; 0 }]
							]),
							Button(nil, nil, [
								["Recall Preset", Color.black, color, 
									{ module.recallSetting; 0 }]
							]),
							Button(nil, nil, [
								["Random", Color.black, color, 
									{ module.randomSetting; 0 }]
							])
						]
						),
						CLabel(nil,nil,"Controls"),
					] 
					++
					this.makeControls(module);
				)]
			)
		);
		screen.onClose = { module.window = nil };
		^screen
	}
	makeControls { arg module;
		^guidefs.collect({ arg def; def.value(module); });
	}
}

// objects to implement the real time side..

RTPatch {
	var <>modules;
	*new { arg modules; ^super.newCopyArgs(modules) }
	make { arg synthID;
		Environment.push({
			modules.do({ arg module; module.make(synthID) });
		});
		modules.last.outputs.at(0);
	}
}

RTInput {
	var <>name, <>module, <>output;
	*new { arg name, module, output;^super.newCopyArgs(name, module, output) }
}
	
RTControl {
	var <>name, <>plugID;
	*new { arg name, plugID; ^super.newCopyArgs(name, plugID) }
}
	
RTModule {
	var <>func, <>inputs, <>outputs, <>controls;
	*new { arg func, inputs, outputs, controls; 
		^super.newCopyArgs(func, inputs, outputs, controls) 
	}
	
	make { arg synthID;
		currentEnvironment.makeEmpty;
		inputs.do({ arg input;
			currentEnvironment.put(input.name, input.module.outputs.at(input.output));
		});
		controls.do({ arg control;
			currentEnvironment.put(control.name, ControlSpace.kr(synthID, control.plugID));
		});
		outputs = func.valueEnvir;
	}
}

*/

