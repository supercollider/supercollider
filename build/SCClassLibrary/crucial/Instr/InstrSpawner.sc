
InstrSpawner : Patch {
	
	classvar <>latency=0.07;
	
	var <>deltaPattern,deltaStream,<delta;
	var streams,sendArray,firstSecretDefArgIndex,spawnTask,<clock,spawnGroup;
	
	*new { arg instrName,args,deltaPattern = 1.0;
		^super.new(instrName,args).deltaPattern_(deltaPattern)
	}
	
	storeArgs { ^[instr.name,args,deltaPattern] }
	
	asSynthDef {
		var initArgs;
		^synthDef ?? {
			initArgs = this.args.collect({ arg a,i;
						var spec;
						if(a.rate == \stream,{
							// use an Ir to reserve an input into the synth.
							// a Pattern does not know the spec/default of its output
							// but we do.
							spec = this.instr.specs.at(i);
							IrNumberEditor(spec.default,spec);
						},{
							a
						})
						//a
					});
			synthDef = InstrSynthDef.build(this.instr,initArgs,Out);
			//synthDef.secretDefArgs(this.args).insp;
			defName = synthDef.name;
			numChannels = synthDef.numChannels;
			rate = synthDef.rate;
			synthDef
		}
	}
	makeTask {
		deltaStream.reset; delta = 0.0;
		streams.do({ arg stream; stream.reset });
		spawnTask = Routine({
			var server;
			server = this.server;
			while({
				this.spawnNext;
			},{
				server.sendBundle(latency,sendArray);
				delta.wait // we are running slightly ahead of arrival time
			});					
		});
	}
	spawnNext {
		if((delta = deltaStream.next(this)).isNil,{ ^false });
		// only sending secret def args the first time !
		// if sample changes, need to put into sendArray
		streams.do({ arg s,i;
			var val;
			if((val = s.next(this)).isNil ,{ ^false });
			sendArray.put(i * 2 + 6,val);
		});
		^true
	}
	update { arg changed,changer;
		// one of my scalar inputs changed
		if(this.isPlaying,{
			synthDef.secretDefArgs(this.args).do({ arg newarg,i;
				sendArray.put(i + firstSecretDefArgIndex, newarg);
			})
		},{
			defName = synthDef = nil;
		})
	}

	makeResourcesToBundle { arg bundle;
		spawnGroup = Group.basicNew;
		NodeWatcher.register(spawnGroup);
		this.annotate(spawnGroup,"spawnGroup");
		bundle.add( spawnGroup.addToTailMsg(group) );

		streams = Array(argsForSynth.size);
		sendArray = Array(argsForSynth.size * 2);
		synthArgsIndices.do({ arg ind,i;
			if(ind.notNil,{
				// a pattern or player
				// eg. a ModalFreq would use asStream rather than its synth version
				streams.add(args.at(i).asStream);

				sendArray.add(this.instr.argNameAt(i));
				sendArray.add(nil); // where the value will go
			})
		});
		deltaStream = deltaPattern.asStream;

		CmdPeriod.add(this);		
		sendArray = ["/s_new",defName,-1,1,nil] ++ sendArray;
		firstSecretDefArgIndex = sendArray.size;
		sendArray = sendArray	++ synthDef.secretDefArgs(args) 
					++ [\out, this.patchOut.synthArg];
	}
	freeResourcesToBundle { arg bundle;
		spawnGroup.freeToBundle(bundle);
		bundle.addFunction({ spawnGroup  = nil; });
		bundle.addFunction({ CmdPeriod.remove(this) });
	}

	makePatchOut { arg agroup,private=false,bus,bundle;
		//group = group.asGroup;
		//server = group.server;
		this.topMakePatchOut(agroup,private,bus);

		this.childrenMakePatchOut(spawnGroup,true,bundle);

		streams.do({ arg s,i;
			// replace players with their outputs
			if(s.isKindOf(AbstractPlayer),{
				streams.put(i,s.synthArg);
			})
		});
		sendArray.put(4,spawnGroup.nodeID);
		sendArray.put(sendArray.size - 1, this.patchOut.synthArg);
	}

	spawnToBundle { arg bundle;
		if(patchOut.isNil,{ 
			(thisMethod.asString 
				+ "PatchOut is nil. Has this been prepared for play ?").die(this);
		});
		this.makeTask;

		if((delta = deltaStream.next).notNil and: {this.spawnNext},{
			this.asSynthDef;// make sure it exists
			
			this.children.do({ arg child;
				//child.group = spawnGroup;
				child.spawnToBundle(bundle);
			});

			bundle.add(sendArray);
			bundle.addMessage(this,\didSpawn);
		});
	}

	didSpawn {
		super.didSpawn;
		this.startTask;
	}
	startTask {
		clock = SystemClock;
		clock.sched((delta - latency).max(0.0),spawnTask);
	}

	isPlaying {
		^(spawnTask.notNil and: {spawnTask.state == 7})
	}
	didStop {
		super.didStop;
		// can't use Task because its start is quantized
		spawnTask.stop;
		spawnTask = nil;
	}
	didFree {
		CmdPeriod.remove(this);
	}
	cmdPeriod {
		this.didStop;
	}
	freeToBundle { arg bundle;
		bundle.addMessage(this,\didFree);
	}
	
	guiClass { ^InstrSpawnerGui }	
}


InstrGateSpawner : InstrSpawner {
	
	var <beat=0;
	
	makeTask {
		deltaStream.reset; delta = 0.0; beat = 0.0;
		streams.do({ arg stream; stream.reset });
		spawnTask = Routine({
			var server;
			server = this.server;
			while({
				beat = beat + delta;
				this.spawnNext;
			},{
				server.sendBundle(latency,sendArray);
				delta.wait // we are running slightly ahead of arrival time
			});					
		});
	}

	startTask {
		clock = TempoClock.default;
		// play without the quantize
		//first event has been sent, sched to the second
		clock.schedAbs(clock.elapsedBeats + (delta - Tempo.secs2beats(latency)), spawnTask);
	}
}




/**

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



**/



