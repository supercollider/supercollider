
InstrSpawner : Patch {

	var <>delta;	
	var deltaStream,streams,sendArray,aintSeenNil = true,spawnTask;
	
	*new { arg name,args,delta = 1.0;
		^super.new(name,args).delta_(delta)
	}
	storeArgs { ^[this.instr.name,args,delta] }
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

			if(ag.rate != \scalar and: { instr.specs.at(i).rate != \scalar },{
				//ag.insp(instr.argNameAt(i));
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
						var spec;
						if(a.rate == \stream,{
							spec = this.instr.specs.at(i);								IrNumberEditor(spec.default,spec);
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
		
		streams = Array(argsForSynth.size);
		sendArray = Array(argsForSynth.size * 2);
		synthArgsIndices.do({ arg ind,i;
			if(ind.notNil,{
				if(args.at(i).isKindOf(AbstractPlayer),{
					// temp put the arg in there
					// after making the patchOut, insert synthArg
					streams.add(args.at(i))
				},{
					// a pattern 
					streams.add(args.at(i).asStream);
				});
				sendArray.add(this.instr.argNameAt(i));
				sendArray.add(nil); // where the value will go
			})
		});
		sendArray = sendArray ++ synthDef.secretDefArgs(args) 
					++ [\out, this.patchOut.synthArg];
	}
	
	makePatchOut {arg group,private = false,bus;
		super.makePatchOut(group,private,bus);
		// replace players with their outputs
		streams.do({ arg s,i;
			if(s.isKindOf(AbstractPlayer),{
				streams.put(i,s.synthArg);
			})
		});
		// last slot is my \out
		sendArray.put(sendArray.size - 1, this.patchOut.synthArg);
	}
	synthDefArgs {
		streams.do({ arg s,i;
			sendArray.put(i * 2 + 1,s.value ?? {aintSeenNil = false; nil });
		});		
		^sendArray
	}

	didSpawn {
		deltaStream = delta.asStream;
		spawnTask = Task({
			aintSeenNil=true;
			deltaStream.next.wait;
			while({ 
				this.synthDefArgs; // make next array
				aintSeenNil 
			},{
				this.server.listSendBundle(0.05, // slop
					[["/s_new",defName,-1,1,this.group.nodeID] ++ sendArray]);
				deltaStream.value.wait;
			})	
		}, SystemClock);
		spawnTask.play
	}
	stop { 
		spawnTask.stop;
	}
	guiClass { ^InstrSpawnerGui }	
}


InstrGateSpawner : InstrSpawner {
	var <>duration,durationStream,<>maxPolyphony = 16,synths;
	
	*new { arg name,args,delta = 1.0,duration=0.8;
		^super.new(name,args).delta_(delta).duration_(duration)
	}
	storeArgs { ^[this.instr.name,args,delta,duration] }
	createArgs { arg argargs;
		var gateIndex;
		argargs = argargs.asArray.extend(instr.argsSize,nil);
		gateIndex = instr.argNames.detectIndex({ arg agn; agn == \gate });
		// stick a Kr there to hold the input as control
		if(gateIndex.notNil,{
			argargs.put(gateIndex, KrNumberEditor(1.0,[0.0,1.0]).lag_(0.0) );
		});
		super.createArgs(argargs);
	}
	prepareToBundle { arg group,bundle;
		super.prepareToBundle(group,bundle);
		synths = Array.fill(maxPolyphony,{ arg i;
						if(i > 0,{ // first synth will be from spawnToBundle
							Synth.basicNew(defName,this.server);
						})
					});
	}
	synthDefArgs { arg  beat=0;
		streams.do({ arg s,i;
			sendArray.put(i * 2 + 1,s.value(beat) ?? {aintSeenNil = false; nil });
		});
		^sendArray 
	}
	didSpawn {
		deltaStream = delta.asStream;
		durationStream = duration.asStream;
		synths.put(0,synth);
		
		spawnTask = Task({
		
			var delta,dur,server,latency,beat,itSynth,index=1;
			aintSeenNil=true;
			beat = TempoClock.default.elapsedBeats;
			
			server = this.server;
			latency = server.latency;
			
			//first synth already started
			delta = deltaStream.next;
			dur = durationStream.next(delta,beat);
			// sched first gate off
			server.listSendBundle(Tempo.beats2secs(dur), [ synth.releaseMsg ]);
			
			// small slippage if tempo changes during first event !
			(delta - Tempo.secs2beats(latency)).wait;
			
			while({
				beat = TempoClock.default.elapsedBeats;
				this.synthDefArgs(beat);
				delta = deltaStream.value(beat);
				dur = durationStream.value(delta,beat);
				aintSeenNil and: delta.notNil and: dur.notNil
			},{
				itSynth = synths.wrapAt(index = index + 1);
				// voice stealing: if(itSynth.isPlaying,{ add the release for it });
				server.listSendBundle(server.latency,
					[ itSynth.addToTailMsg(this.group,sendArray) ] );
					
				// tempo change during play screws up release time
				server.listSendBundle(server.latency + Tempo.beats2secs(dur),
					[	["/n_set", itSynth.nodeID, \gate, 0] ]);
				delta.wait // we are running slightly ahead of arrival time
			});

		}, TempoClock.default);
		spawnTask.play
	}
	freeSynth {
		synths.do({ arg synth;
			synth.free(synth.isPlaying);
		});
	}
}

InstrGateSleepSpawner : InstrGateSpawner {
	// assumes doneAction 1 (sleep)
	//  just wakes and sets the gate back to 1

	prepareToBundle { arg group,bundle;
		super.prepareToBundle(group,bundle);
		synths.do({ arg synth,i;
			if(i > 0 and: aintSeenNil,{
				bundle.add(synth.addToTailMsg(this.group,this.synthDefArgs.copy));
				bundle.add([12, synth.nodeID,0])
			})
		});
	}
	didSpawn {
		deltaStream = delta.asStream;
		durationStream = duration.asStream;
		synths.put(0,synth);
		
		spawnTask = Task({
		
			var delta,dur,server,latency,beat,itSynth,index=1;
			aintSeenNil=true;
			beat = TempoClock.default.elapsedBeats;
			
			server = this.server;
			latency = server.latency;
			
			//first synth already started
			delta = deltaStream.next;
			dur = durationStream.next(delta,beat);
			// sched first gate off
			server.listSendBundle(Tempo.beats2secs(dur), [ synth.releaseMsg ]);
			
			// small slippage if tempo changes during first event !
			(delta - Tempo.secs2beats(latency)).wait;
			
			while({
				beat = TempoClock.default.elapsedBeats;
				this.synthDefArgs(beat);
				delta = deltaStream.value(beat);
				dur = durationStream.value(delta,beat);
				aintSeenNil and: delta.notNil and: dur.notNil
			},{
				itSynth = synths.wrapAt(index = index + 1);
				server.listSendBundle(server.latency,
					[ 
						["/n_set",itSynth.nodeID,\gate,1] ++ sendArray,
						[12,itSynth.nodeID ,1] // wake up
					]);
					
				// tempo change during play screws up release time
				server.listSendBundle(server.latency + Tempo.beats2secs(dur),
					[	["/n_set", itSynth.nodeID, \gate, 0] ]);
				delta.wait // we are running slightly ahead of arrival time
			});

		}, TempoClock.default);
		spawnTask.play
	}
}








// older pre-TempoClock...

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
			
		}, SystemClock);
		spawnTask.play
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

