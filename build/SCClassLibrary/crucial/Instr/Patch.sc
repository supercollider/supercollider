
// : HasInputsPlayer

Patch : AbstractPlayer  {
		
	var <>args,<instr;
	var <patchIns,synthPatchIns,argsForSynth;
	
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
			var proto,spec,ag,patchIn;
			ag = 
				argargs.at(i) // explictly specified
				?? 
				{ //  or auto-create a suitable control...
					spec = instr.specs.at(i);
					// should check for argName match too...
					//proto = spec.defaultControl(instr.defArgAt(i));
					spec.defaultControl(0.2);
					proto
				};
				
//			patchIn = PatchIn.newByRate(instr.specs.at(i).rate);
//			patchIns = patchIns.add(patchIn);
//
//			// although input is control, arg could overide that
//			if(instr.specs.at(i).rate != \scalar
//				and: {ag.instrArgRate != \scalar}
//			,{
//				argsForSynth = argsForSynth.add(ag);
//				synthPatchIns = synthPatchIns.add(patchIn);
//			});
			
			ag		
		});
		this.dump;
		this.halt;
	}
	
	numChannels { ^instr.numChannels }
	rate { ^instr.rate }
	
	/* scserver support */
	asSynthDef {
		var is;
		is = InstrSynthDef.new;
		is.build(this.instr,this.args,\Out);
		defName = is.name;
		^is
	}

	synthDefArgs {
		// not every arg makes it into the synth def
		^argsForSynth.collect({ arg ag; ag.synthArg })
			++ [patchOut.bus.index] // always goes last
	}
	defName {
		// set only after asSynthDef !
		^defName
	}
	didSpawn {
		//node control if you want it
		synthPatchIns.do({ arg patchIn,synthArgi;
			patchIn.nodeControl_(NodeControl(synth,synthArgi));
		});
		patchIns.do({ arg patchIn,pti;
			// objects hook up 
			patchOut.patchOutsOfInputs.at(pti).connectTo(patchIn,false); 
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

