
// : HasInputsPlayer

Patch : AbstractPlayer  {
		
	var <>args,<instr;
	var <patchIns,synthPatchIns,argsForSynth;
	var synthDef;
	var <numChannels,<rate; // determined after making synthdef
	
	*new { arg name,args;
		^super.new.loadSubject(name).createArgs(loadPath(args) ?? {[]})
	}

	loadSubject { arg name;
		instr = name.asInstr;
		if(instr.isNil,{
			("Instrument not found !!" + name).die;
		});
	}

	createArgs { arg argargs;
		argsForSynth = [];
		patchIns = [];
		synthPatchIns = [];
		args=Array.fill(this.instr.argsSize,{arg i; 
			var proto,spec,ag,patchIn,darg;
			ag = 
				argargs.at(i) // explictly specified
				?? 
				{ //  or auto-create a suitable control...
					spec = instr.specs.at(i);
					proto = ControlPrototypes.forSpec(spec,instr.argNames.at(i));
					proto.tryPerform('spec_',spec); // make sure it does the spec
					if((darg = instr.initAt(i)).isNumber,{
						proto.tryPerform('value_',darg);// set its value
					});
				};
				
			patchIn = PatchIn.newByRate(instr.specs.at(i).rate);
			patchIns = patchIns.add(patchIn);

			// although input is control, arg could overide that
			if(instr.specs.at(i).rate != \scalar
				and: {ag.instrArgRate != \scalar}
			,{
				argsForSynth = argsForSynth.add(ag);
				synthPatchIns = synthPatchIns.add(patchIn);
			});
			
			ag		
		});
	}
	
	/* scserver support */
	asSynthDef {
		// could be cached, must be able to invalidate it
		// if an input changes
		^synthDef ?? {
			synthDef = InstrSynthDef.build(this.instr,this.args,\Out);
			defName = synthDef.name;
			numChannels = synthDef.numChannels;
			rate = synthDef.rate;
			synthDef
		}
	}
	// has inputs
	spawnToBundle { arg bundle;
		var synthArgs;
		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		synthArgs = Array(argsForSynth.size + 1 * 2);
		this.synthDefArgs.do({ arg a,i;
			synthArgs.add(i);
			synthArgs.add(a);
		});
		synth = Synth.newMsg(bundle, // newToBundle
				this.defName,
				synthArgs ++ synthDef.secretDefArgs(args),//.insp("secretArgs"),
				patchOut.group,
				\addToTail
				);
	}

	synthDefArgs {
		// not every arg makes it into the synth def
		^(argsForSynth.collect({ arg ag; ag.synthArg })
			++ [patchOut.bus.index]) // always goes last
	}
	didSpawn {
		//node control if you want it
		synthPatchIns.do({ arg patchIn,synthArgi;
			patchIn.nodeControl_(NodeControl(synth,synthArgi));
		});
		patchIns.do({ arg patchIn,pti;
			// objects hook up 
			patchOutsOfChildren.at(pti).connectTo(patchIn,false); 
		});
	}
	
	/*
	setInput { arg i,newarg;
		var old,newargpatchOut;
		old = args.at(i);
		args.put(i,newarg);
		if(this.isPlaying,{
			// release old  thru some manager ?
			//old.patchOut.releaseConnection;
			newargpatchOut = newarg.play(Destination.newByRate(this.instr.specs.at(i).rate,
								NodeControl(patchOut.synth,i + 1)));
			newargpatchOut.retainConnection;
			newargpatchOut.connectTo(patchIns.at(i));
		});
	}
	*/
	free {
		// TODO only if i am the only exclusive owner
		this.children.do({ arg child; child.free });
		super.free;
	}

	// pattern support
	/*	
	asStream {
		// return a stream of ugenFuncs.
		// each argument is also streamed
		var argstreams,func;
		func= this.instr.func;
		argstreams = args.collect({ arg a; a.asStream });
		
		^FuncStream({
			arg inval;
			// return the ugenFunc
			{	func.valueArray( argstreams.collect({ arg a,i; a.next.value })  )  }
			// no envelope added
		})
	}
	*/
	
	
	
	//act like a simple ugen function
	ar { 	arg ... overideArgs;	^this.valueArray(overideArgs) }
	value { arg ... overideArgs;  ^this.valueArray(overideArgs) }
	valueArray { arg  overideArgs;  
		// each arg is valued as it is passed into the instr function
		^instr.valueArray(
				args.collect({ arg a,i; (overideArgs.at(i) ? a).value; })  
			)
	}


	storeParamsOn { arg stream;
		stream.storeArgs([this.instr.name,enpath(args)]);
	}

	children { ^args }
	guiClass { ^PatchGui }

}

