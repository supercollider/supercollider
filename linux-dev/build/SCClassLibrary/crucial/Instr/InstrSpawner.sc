

InstrSpawner : Patch {

	var <>delta;
	
	var deltaStream,streams,sendArray,aintSeenNil = true,spawnTask;
	
	*new { arg name,args,delta = 1.0;
		^super.new(name,args).delta_(delta)
	}
	createArgs { arg argargs;
		var argsSize;
		argsForSynth = [];
		patchIns = [];
		synthPatchIns = [];
		argsSize = this.instr.argsSize;
		synthArgsIndices = Array.newClear(argsSize);

		args=Array.fill(argsSize,{arg i; 
			var proto,spec,ag,patchIn,darg;
			ag = 
				argargs.at(i) // explictly specified
				?? 
				{ //  or auto-create a suitable control...
					spec = instr.specs.at(i);
					proto = spec.defaultControl;
					proto.tryPerform('spec_',spec); // make sure it does the spec
					if((darg = instr.initAt(i)).isNumber,{
						proto.tryPerform('value_',darg);// set its value
					});
					proto
				};
				
			patchIn = PatchIn.newByRate(instr.specs.at(i).rate);
			patchIns = patchIns.add(patchIn);

			if(instr.specs.at(i).rate != \scalar
				/**
					 arg does not override that, we stream
					 
					// and: {ag.instrArgRate != \scalar}
				**/
			,{
				argsForSynth = argsForSynth.add(ag);
				synthPatchIns = synthPatchIns.add(patchIn);
				synthArgsIndices.put(i,synthPatchIns.size - 1);
			});
			
			ag		
		});
	}
	
	asSynthDef {
		^synthDef ?? {
			var initArgs;
			initArgs = this.args.collect({ arg a,i;
						if(synthArgsIndices.at(i).notNil,{
							if(a.isNumber,{
								a
							},{
								// better, use something that causes an i_
								KrNumberEditor(this.instr.defArgAt(i),this.instr.specs.at(i));
							})
						},{
							a
						})
					});
			synthDef = InstrSynthDef.build(this.instr,initArgs,Out);
			defName = synthDef.name;
			numChannels = synthDef.numChannels;
			rate = synthDef.rate;
			synthDef
		}
	}
	
	prepareToBundle { arg group,bundle;
		super.prepareToBundle(group,bundle);
		deltaStream = delta.asStream;

		streams = Array(argsForSynth.size);
		sendArray = Array(argsForSynth.size * 2);
		synthArgsIndices.do({ arg ind,i;
			if(ind.notNil,{
				streams.add(args.at(i).asStream);
				sendArray.add(this.instr.argNameAt(i));
				sendArray.add(nil); // where the value will go
			})
		});
		sendArray.add( \out );
		sendArray.add( this.patchOut.synthArg );
		
		aintSeenNil = true;
		//sched = OSCSched.new; // should use
	}

	synthDefArgs { 
		streams.do({ arg s,i;
			sendArray.put(i * 2 + 1,s.next ?? {aintSeenNil = false; nil });
		});
		^sendArray 
	}

	didSpawn {
		spawnTask = Task({
			var aintSeenNil=true;
			deltaStream.next.wait;
			while({ 
				aintSeenNil 
			},{
				// no release, just replace
				synth = Synth.grain(defName,sendArray);
				deltaStream.next.wait;
				this.synthDefArgs; // make next array
			})	
		});
		spawnTask.play(SystemClock)
	}
	stop { spawnTask.stop }
	guiClass { ^InstrSpawnerGui }
	
}


// jt 
InstrSpawner2 : InstrSpawner {

	var <>beatsPerStep,<>tempo,tempoUpdater;
	
	*new { arg name,args,noteOn = 1.0, beatsPerStep = 0.25,tempo;
		^super.new(name,args,noteOn).beatsPerStep_(beatsPerStep).tempo_(tempo ? Tempo.default)
	}

	didSpawn {
		var beatWait;
		beatWait = tempo.beats2secs(beatsPerStep);
		tempoUpdater = Updater(tempo,{
			beatWait = tempo.beats2secs(beatsPerStep);
		});
		spawnTask = Task({
			var aintSeenNil=true, aNoteOn,group;
			group = this.group;
			beatWait.wait;
			while({
				aintSeenNil
			},{
				aNoteOn = deltaStream.next;
				if(aNoteOn > 0,{
					synth = Synth(defName,sendArray,group);
					beatWait.wait;
					this.synthDefArgs; // make next array
				},{
					if(aNoteOn < 0,{ // legato
						synth.setWithArray(sendArray);
						beatWait.wait;
						this.synthDefArgs; // make next array
					},{
						// rest
						beatWait.wait
					});
				});
			});
			
		});
		spawnTask.play(SystemClock)
	}
	free {
		tempoUpdater.remove;
		super.free;
	}

}


InstrSpawner3 : InstrSpawner2 {

	didSpawn {
		var beatWait;
		beatWait = tempo.beats2secs(beatsPerStep);
		tempoUpdater = Updater(tempo,{
			beatWait = tempo.beats2secs(beatsPerStep);
		});
		spawnTask = Task({
			var aintSeenNil=true, aNoteOn,group,lastNoteOn=0;
			group = this.group;
			
			// ISSUE first note maybe shouldn't have played
			beatWait.wait;
			// release first synth
			
			while({
				aintSeenNil
			},{
				aNoteOn = deltaStream.next;
				if(lastNoteOn != 0,{
					if(aNoteOn.isStrictlyPostive,{ // sustain
						beatWait.wait;
					},{
						if(aNoteOn.isNegative,{ // legato
							synth.setWithArray(sendArray);
							beatWait.wait;
							this.synthDefArgs; // make next array
						},{
							// note off
							synth.release;
							beatWait.wait;
							// don't send free message
							synth.server.nodeAllocator.free(synth.nodeID);
						});
					});
				},{
					if(aNoteOn.isStrictlyPostive,{ // note on
						synth = Synth(defName,sendArray,group);
						beatWait.wait;
						this.synthDefArgs; // make next array
					},{
						// rest
						beatWait.wait;
					});
				});
				lastNoteOn = aNoteOn;
			});
			
		});
		spawnTask.play(SystemClock)
	}

}

