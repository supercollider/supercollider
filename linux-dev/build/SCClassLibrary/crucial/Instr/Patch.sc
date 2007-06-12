
HasPatchIns : AbstractPlayer {

	var <patchIns;

	mapInputToBus { arg i,bus;
		var patchOut;
		bus = bus.asBus;
		patchOut = PatchOut.performList(bus.rate,[nil,bus.server.asGroup,bus]);
		patchOut.connectTo(patchIns.at(i), this.isPlaying );
	}
/*
have to bundle it
	connectInputToPlayer { arg i,player;
		// does it have patchOut
		if(player.patchOut.isNil,{
			// always uncomfortable to not have patchOut decided
			player.makePatchOut(this.group,true);
		});
		player.patchOut.connectTo(patchIns.at(i), this.isPlaying);
	}
*/				
	/*
	setInput { arg i,newarg;
		var old,newargpatchOut;
		old = args.at(i);
		args.put(i,newarg);
		if(this.isPlaying,{
			old.free; // release old  thru some manager ?
			newarg
			
			//old.patchOut.releaseConnection;
			newargpatchOut = newarg.play(Destination.newByRate(this.instr.specs.at(i).rate,
								NodeControl(patchOut.synth,i + 1)));
			newargpatchOut.retainConnection;
			newargpatchOut.connectTo(patchIns.at(i));
		});
	}
	*/
	
	//inputs subclassResponsibility // players, floats etc.
	inputProxies { 
		^this.inputs.select({ arg a; a.isKindOf(PlayerInputProxy) })
	}
	
}

Patch : HasPatchIns  {
		
	var <>args,<instr;
	var synthPatchIns,argsForSynth;
	
	var synthArgsIndices;
	
	var synthDef;
	var <numChannels,<rate; // determined after making synthdef
	
	*new { arg name,args;
		^super.new.loadSubject(name).createArgs(loadDocument(args) ? [])
	}

	loadSubject { arg name;
		instr = name.asInstr;
		if(instr.isNil,{
			("Instrument not found !!" + name).die;
		});
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
					//ControlPrototypes.forSpec(spec,instr.argNames.at(i));
					//if(proto.isNil,{ spec.insp("nil proto:",instr.argNames.at(i),i) });
					proto.tryPerform('spec_',spec); // make sure it does the spec
					if((darg = instr.initAt(i)).isNumber,{
						proto.tryPerform('value_',darg);// set its value
					});
					proto
				};
				
			patchIn = PatchIn.newByRate(instr.specs.at(i).rate);
			patchIns = patchIns.add(patchIn);

			// although input is control, arg could overide that
			if(instr.specs.at(i).rate != \scalar
				and: {ag.instrArgRate != \scalar}
			,{
				argsForSynth = argsForSynth.add(ag);
				synthPatchIns = synthPatchIns.add(patchIn);
				synthArgsIndices.put(i,synthPatchIns.size - 1);
			});
			
			ag		
		});
	}
	
	/* scserver support */
	asSynthDef {
		// could be cached, must be able to invalidate it
		// if an input changes
		^synthDef ?? {
			synthDef = InstrSynthDef.build(this.instr,this.args,Out);
			defName = synthDef.name;
			numChannels = synthDef.numChannels;
			rate = synthDef.rate;
			synthDef
		}
	}
	// has inputs
	spawnToBundle { arg bundle;
		var synthArgs;
		this.asSynthDef;// make sure it exists
		
		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		synth = Synth.basicNew(this.defName,patchOut.server);
		bundle.add(
			synth.addToTailMsg(patchOut.group,
				this.synthDefArgs
				++ synthDef.secretDefArgs(args)
			)
		);
	}
	synthDefArgs {
		// not every arg makes it into the synth def
		var args;
		args = Array(argsForSynth.size * 2 + 2);
		argsForSynth.do({ arg ag,i;
			//args.add(instr.argNameAt(i)); // NO not the right name
			args.add(i);
			args.add(ag.synthArg);
		});
		args.add(\out);
		args.add(patchOut.synthArg //?? {"patchout bus was nil!".error}
							);
		^args//.insp("args")
	}
	defName { ^defName } // NOT 'Patch' ever
	
	// has inputs
	didSpawn { arg patchIn,synthArgi;
		super.didSpawn(patchIn,synthArgi);
		
		//i know of the synth, i hand out the NodeControls
		synthPatchIns.do({ arg synpatchIn,synthArgi;
			synpatchIn.nodeControl_(NodeControl(synth,synthArgi));
		});
		this.children.do({ arg child,childi;
			// some of the kids get a nil synth arg index
			// that's okay, they won't talk to the synth anyway
			child.didSpawn(patchIns.at(childi),
					synthArgsIndices.at(childi));
		});
	}

	free {
		// TODO only if i am the only exclusive owner
		this.children.do({ arg child; child.free });
		super.free;
		// ISSUE: if you change a static, nobody notices to rebuild the synth def
		// so for now, wipe it out
		// the Instr should know if it came from a file, check the moddate
		synthDef = nil;
		readyForPlay = false;
		this.setPatchOut(nil);
	}
	stop {
		super.stop;
		this.children.do({ arg child; child.stop });
	}


	inputs { ^args }

	//act like a simple ugen function
	ar { 	arg ... overideArgs;	^this.valueArray(overideArgs) }
	value { arg ... overideArgs;  ^this.valueArray(overideArgs) }
	valueArray { arg  overideArgs;  
		// each arg is valued as it is passed into the instr function
		^instr.valueArray(
				args.collect({ arg a,i; (overideArgs.at(i) ? a).value; })  
			)
	}


	storeArgs { ^[this.instr.name,args] }

	children { ^args }
	guiClass { ^PatchGui }

}

/*

EfxPatch : Patch


	one meant for efx should be a specific type
	then it can share its bus with the primary input
	
	
*/

