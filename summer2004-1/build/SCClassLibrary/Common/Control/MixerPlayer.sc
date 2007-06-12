/*

object transporter:

class name
indexed size
fixed vars..
indexed vars..

*/

// need a global screen for the edit window.
// need a msg forwarder to the voice's synth (port ids?)
// named specs..
// onClose methods for GUIWindows, Groups, 

/*

Instrument {
	var <>name, <>func, <>specs, <>patches;
	*new { arg name, func, specs, patches;
		^super.newCopyArgs(name, func, specs, patches).init;
	}
	init {
		if (patches.isNil, {
			patches = IdentityDictionary.new; 
		});
		if (specs.notNil, {
			specs.do({ arg specAssn; specAssn.value = specAssn.value.asControlSpec; });
		});
	}	
	play {
		#{ arg func, args; Synth.play(func.valueList(args)) }.send;
	}
	makeWindow { arg voice;
		var window;
		if (GUIScreen.mainScreen.isNil, {^nil});
		window = GUIWindow(name, Rect.newBy(40, 40, 180, specs.size * 22),
			HLayout(nil, [
				VLayout(nil, 
					specs.collect({ arg specAssn; 
						Slider(action: { arg value; 
							voice.send(specAssn.key, specAssn.value.map(value));
						}) 
					})
				),
				VLayout(nil,
					specs.collect({ arg specAssn; CXLabel(nil, nil, specAssn.key) })
				)
			])
		);
		^window
	}
}


InstrumentOld {
	var <>func, <>name;
	
	*new { arg name, func, specs;
		^super.new.func_(func).name_(name)
	}
	
	*play { arg name, func, volume = 0.5;
		this.new(name, func).play(volume);
	}
	
	play { arg volume = 0.5;
		MixerPlayer.play(MixerVoice.new(this), volume);
	}
	
	// act like a UGen
	ar { arg ... inputs;
		^func.valueArray(inputs);
	}
	
	// Library support
	*libPut { arg names, func;
		var instr;
		instr = this.new(names.last, func);
		Library.put(names, instr);
		^instr
	}
	libMenuAction { arg names;
		this.play;
	}
	
	// OrcScore support
	spawn { arg spawn, eventCount, synth, inputs;
		^{ arg deltaTime, instrumentNum ... inputs;
			// strip off deltaTime and instrumentNum
			func.valueArray(inputs)
		}.valueArray(inputs);
	}
}


MixerPlayer {
	classvar tspawn, <>mixer, <inputs, <>numChannels = 2, <>numInputs = 12, env;
	classvar recBtn, recFile, recID=0;

	*reset {
		// in case of an error you may need to reset the MixerPlayer
		tspawn = nil;
		mixer = nil;
		inputs = nil;
		env = nil;
		recBtn = nil;
		recFile = nil;
	}
	*play { arg voice, volume = 0.5;
		var input;
		
		this.makeMixer;
		input = this.findFreeInput;
		if (input.isNil, {^this});
		
		this.initInput(input, voice, voice.instrument.name, volume);
		
		if (tspawn.isNil, {
			this.prepareRecord;
			Synth.play(this.playFunc(input));
			this.synthDone;
		},{
			tspawn.trigger(input);
		});
	}
	
	// PRIVATE
	*initInput { arg input, voice, name, volume;
		input.voice = voice;
		input.name = name.asString;
		input.nameView.label = input.name;
		if (voice.canMakeEditWindow, {
			input.editBtn.label = "Edit";
		});
		input.volumeSlider.value = volume;
		input.killBox.value = 1;
		input.killBox.action = {
			if (input.killBox.value == 0, {
				if ( tspawn.notNil and: { input.synth.notNil }, {
					input.synth.release;
				});
				input.voice.close;
				input.voice = nil;
				input.name = nil;
				input.synth = nil;
				input.volumeSlider.value = 0.0005;
				input.killBox.action = nil;
				input.copyBtn.action = nil;
				input.editBtn.action = nil;
				input.nameView.label = "-";
				input.editBtn.label = "_";
			});
		};
		input.copyBtn.action = {
			this.play(input.voice.copy, input.volumeSlider.value);
		};
		input.editBtn.action = {
			input.voice.makeEditWindow;
		};
	}
	*playFunc { arg input;
		^{ arg synth;
			var outs;
			
			outs = TSpawn.ar({ arg argTSpawn, i, tsynth, argInput;
				argInput.synth = tsynth;
				Pause.ar({
					argInput.voice.play(synth);
				}, argInput.volumeSlider.kr - 0.001) * EnvGen.kr(env);
			}, numChannels);
			
			tspawn = outs.source;
			synth.sched(0, { 
				tspawn.trigger(input) 
			});
			
			if (recFile.notNil, {
				// record to disk
				Pause.ar({ DiskOut.ar(recFile, 32768, outs); }, recBtn.kr);
			});
			
			outs
		}
	}
	*synthDone {
		if (recFile.notNil, { recFile.endRecord; recID = recID + 1; });
		recBtn.value = 0;
		tspawn = nil;
		inputs.do({ arg anInput;
			anInput.killBox.value_(0).doAction;
		});	
	}
	*makeMixer {	
		if (mixer.notNil, { 
			//mixer.toFront; 
			^mixer 
		});
		
		// make cutoff envelope
		env = Env.new(#[1,1,0],[1,0.01],'linear',1);
		
		mixer = GUIWindow.new("Mixer", Rect.newBy( 80, 80, 550, 80 + (24 * numInputs)) );
		recBtn = CheckBoxView.new( mixer, Rect.newBy( 10, 4, 100, 20 ), "Record", 0, 0, 1, 0, 'linear');

		inputs = Array.new(numInputs);
		numInputs.do({ arg i;
			var vol, box, copybtn, editbtn, str, top;
			top = 24 * i + 30;
			vol = SliderView.new( mixer, Rect.newBy( 10, top, 128, 20 ), "SliderView", 0.0, 0.0005, 1, 0, 'exponential');
			box = CheckBoxView.new( mixer, Rect.newBy( 142, top, 20, 20 ), "", 0, 0, 1, 0, 'linear');
			copybtn = ButtonView.new( mixer, Rect.newBy( 168, top, 40, 20 ), "Copy", 0, 0, 1, 0, 'linear');
			editbtn = ButtonView.new( mixer, Rect.newBy( 212, top, 40, 20 ), "_", 0, 0, 1, 0, 'linear');
			str = StringView.new( mixer, Rect.newBy( 264, top, 128, 20 ), "-");
			inputs.add(
				MixerInputSpec.new(i, nil, nil, nil, vol, box, copybtn, editbtn, str)
			);
		});
		mixer.onClose = { 
			inputs.do({ arg anInput;
				if (anInput.notNil, {
					anInput.killBox.value_(0).doAction;
				});
			});	
			mixer = nil; 
		};
		^mixer;
	
	}
	*findFreeInput {
		^inputs.detect({ arg anInput; anInput.voice.isNil });
	}
	*prepareRecord {
		var filename;
		
		if (thisProcess.isDemo.not, {
			// make record file
			recFile = SoundFile.new;
			recFile.numChannels = numChannels;
			recFile.headerFormat = 'AIFF';
			recFile.sampleFormat = '16 big endian signed';
			filename = ":Sounds:MixerOut_" ++ (recID%10);
			if (recFile.writeHeader(filename), {
				recFile.prepareRecord;
			},{
				("write record file header failed : " ++ filename).postln;
				recFile = nil;
			});
		});
	}
	*forInput { arg channel, func;
		var inputs, input;
		inputs = MixerPlayer.inputs;
		inputs.notNil.if({
			input = inputs.at(channel - 1);
			input.notNil.if({
				^func.value(input);
			});
		});
		^nil
	}

}


MixerVoice {
	var <>instrument, <>editWindow, <>plugs, <>params;

	*new { arg instrument;
		^super.new.instrument_(instrument);
	}
	
	init {
	}
	
	copy { 
		var copy;
		copy = this.class.new(instrument) ;
		copy.params = Array.fill(plugs.size, { arg i; plugs.at(i).source.value; });
		^copy
	}
	
	canMakeEditWindow {
		^instrument.func.def.argNames.size >= 1
	}
	makeEditWindow {
		var def, defargs, defvals;
		if (editWindow.notNil, { ^editWindow });
		if (plugs.isNil, { ^nil });
		
		def = instrument.func.def;
		defargs = def.argNames;
		defvals = def.prototypeFrame;
		
		if (defargs.size < 1, { ^nil });
		
		editWindow = GUIWindow.new(instrument.name.asString, Rect.newBy( 80, 80, 550, 60 + (24 * defargs.size)) );
		editWindow.onClose = { editWindow = nil; };
		
		defargs.do({ arg argName, i;
			var spec, top, val, numer, slider, plug;
			top = 24 * i + 30;
			
			StringView.new( editWindow, Rect.newBy( 10, top, 100, 20 ), argName );

			spec = ControlSpec.specs.at(argName);
			plug = plugs.at(i);
			if (spec.notNil, {
				//val = defvals.at(i) ? 0.0;
				val = plug.source.value;
				slider = SliderView.new( editWindow, Rect.newBy( 114, top, 128, 20 ), 
					nil, val, 	spec.minval, spec.maxval, spec.step, spec.warp
				);
				numer = NumericalView.new( editWindow, Rect.newBy( 246, top, 60, 20 ), 
					nil, val, 	spec.minval, spec.maxval, spec.step, spec.warp
				);
				slider.action = { numer.value = slider.value };
				numer.action = { slider.value = numer.value };
			},{
				numer = NumericalView.new( editWindow, Rect.newBy( 114, top, 60, 20 ), 
					nil, defvals.at(i) ? 0.0, 	-1e10, 1e10
				);
			});
			plugs.at(i).source = numer;
		});
	}
	close {
		if (editWindow.notNil, { 
			editWindow.close;
		});
	}
	
	play { arg synth;
		var def, size;
		def = instrument.func.def;
		size = def.argNames.size;
		plugs = Array.new(size);
		if (params.notNil, {
			size.do({ arg i;
				plugs.add(Plug.kr(params.at(i)));
			}); 
		},{
			size.do({ arg i;
				plugs.add(Plug.kr(def.prototypeFrame.at(i)));
			}); 
		});
		^instrument.func.valueArray(plugs)
	}
	
}

MixerInputSpec {
	var <>index, <>voice, <>name, <>synth, <>volumeSlider, 
		<>killBox, <>copyBtn, <>editBtn, <>nameView;
	
	*new { arg index, voice, name, synth, volumeSlider, killBox, copyBtn, editBtn, nameView;
		^super.new.init(index, voice, name, synth, volumeSlider, killBox, copyBtn, editBtn, nameView)
	}
	init { arg argIndex, argMixerVoice, argName, argSynth, argVolumeSlider, 
				argKillBox, argCopyBtn, argEditBtn, argNameView;
		index = argIndex;
		voice = argMixerVoice;
		name = argName;
		synth = argSynth;
		volumeSlider = argVolumeSlider;
		killBox = argKillBox;
		copyBtn = argCopyBtn;
		editBtn = argEditBtn;
		nameView = argNameView;
	}
}

// scheduler.tempo = event.at(\tempo);
// event.delta

MonoEventPlayer
{
	var <>plugs, <>func;
	doEvent { arg event;
		plugs.do({ arg plug; plug.source = event; });
		func.value(event); 
	}
}

PolyEventPlayer
{
	var <>spawn;
	doEvent { arg event;
		var innersynth;
		event.use({
			var freqs, lag;
			freqs = ~freq;
			if ( freqs.isKindOf(Symbol), {
				nil;					// rest
			},{
				Synth.new({
					innersynth = thisGroup;
					thisGroup.releaseTime = ~sustain;
					if (freqs.isKindOf(SequenceableCollection), {
						// mix chords
						Mix.arFill(freqs.size, { arg i;
							~freq = freqs.at(i);
							~ugenFunc.valueEnvir;
						});
					},{
						~ugenFunc.valueEnvir;		// normal case
					});
				});
			});
			if (innersynth.notNil, {
				lag = ~lag;
				if (lag <= 0.0, {
					spawn.add(innersynth);
				},{
					// schedule lagged event in the future
					spawn.synth.sched(lag, 1, { 
						spawn.add(innersynth); 
						nil;
					});
				});
			});
		});
	}
}

*/


