
// : HasInputsPlayer

Patch : AbstractPlayer  {
		
	var <>args,<instr;
	var <patchIns,synthArgs;
	var defName;
	
	*new { arg name,args;
		^super.new.loadSubject(name).createArgs(loadPath(args) ?? {[]})
	}

	ar { 	arg ... overideArgs;	^this.valueArray(overideArgs) }
	value { arg ... overideArgs;  ^this.valueArray(overideArgs) }
	valueArray { arg  overideArgs;  
		// each arg is valued as it is passed into the instr function
		^instr.valueArray(args.collect({ arg a,i; 
									(overideArgs.at(i) ? a).value; 
							})  
			)
	}
	
	loadSubject { arg name;
		instr = name.asInstr;
		if(instr.isNil,{
			("Instrument not found !!" + name).die;
		});
	}

	createArgs { arg argargs;
		synthArgs = [];
		patchIns = [];
		args=Array.fill(this.instr.argsSize,{arg i; 
			var proto,spec,ag;
			ag = 
				argargs.at(i) // explictly specified
				?? 
				{ //  or auto-create a suitable control...
					spec = instr.specs.at(i);
					
					// should check for argName match too...
					proto = spec.defaultControl(instr.initAt(i));
					
					//ControlPrototypes.forSpec(spec,instr.argNames.at(i));
					//proto.tryPerform('spec_',spec); 
					// make sure it does the spec
					proto
				};
			if(instr.specs.at(i).rate != \scalar,{
				synthArgs = synthArgs.add(ag);
			});
			//TODO: force floats to PatchIn.scalar so you don't waste
			//an extra Control. but then you can't live patch it
			patchIns = patchIns.add(PatchIn.newByRate(instr.specs.at(i).rate));

			ag		
		});
	}
	
	numChannels { ^instr.numChannels }
	rate { ^instr.rate }
	
	/* scserver support */
	asSynthDef { 
		var synthDef,fixedArgs;
		fixedArgs = args.collect({ arg ag,i;
			if(instr.specs.at(i).rate === \scalar 
				//or if the ag itself is scalar
				//or: { ag.rate === \scalar }
				// this allows a float to be fixed, but a number editor also gets fixed
			,{
				//TODO: if its just a float or int, use an .ir, not a fixed
				ag.initDefArg
			},{
				nil // not fixed
			})
		});
		
		synthDef = this.instr.asSynthDef( 
			fixedArgs//.insp(thisMethod,"patch's fixedArgs supplied to instr.asSynthDef") 
		);		
		defName = synthDef.name;
		^synthDef
	} // the default Out version
	
	synthDefArgs {
		^[patchOut.bus.index] ++ 
			// not every arg makes it into the synth def
			synthArgs.collect({ arg ag; ag.initDefArg })//.insp("synth def args");
	}
	defName { // needs to be identical to what the instr writes
			// prepare,asSynthDef would have been called first
		^defName
	}
	didSpawn {
		// make the connection
		patchIns.do({ arg pti,i;
			var argOut;
			pti.nodeControl_(NodeControl(synth,i + 1));
			// not finding the node until after playing
			patchOut.patchOutsOfInputs.at(i).connectTo(pti,false); // scalars hooked up
			//[ i, patchOut.patchOutsOfInputs.at(i), pti ].ins;
		});
		//patchIns.i(nil,"patchIns");
	}

//	setInput { arg i,newarg;
//		var old,newargpatchOut;
//		old = args.at(i);
//		args.put(i,newarg);
//		if(this.isPlaying,{
//			// release old  thru some manager ?
//			//old.patchOut.releaseConnection;
//			newargpatchOut = newarg.play(Destination.newByRate(this.instr.specs.at(i).rate,NodeControl(patchOut.synth,i + 1)));
//			newargpatchOut.retainConnection;
//			newargpatchOut.connectTo(patchIns.at(i));
//		});
//	}
	free {
		// TODO only if i am the only owner
		this.children.do({ arg child; child.free });
		super.free;
	}

	// pattern support
/*	asStream {
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
	storeParamsOn { arg stream;
		stream.storeArgs([this.instr.name,enpath(args)]);
	}

	children { ^args }
	guiClass { ^PatchGui }

}

