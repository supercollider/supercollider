
// HaspatchIns

Patch : AbstractPlayer  {
		
	var <>args,<instr;
	var <patchIns,synthArgs,argOuts;
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
		if(name.isKindOf(Meta_UGen),{ // you can pass in a class eg. SinOsc
			instr = UGenInstr(name);
		},{
			instr = Instr.at(name);
			if(instr.isNil,{
				("Instrument not found !!" ++ name).die;
			});
		})
	}

	createArgs { arg argargs;
		args=Array.fill(this.instr.argsSize,{arg i; 
			var proto,spec;
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
				}
			});
	}
	
	numChannels { ^instr.numChannels }
	rate { ^instr.rate }
	
	/* scserver support */
	defName { // needs to be identical to what the instr writes
		^defName
	}
	asSynthDef { 
		var synthDef,fixedArgs;
		patchIns = [];
		synthArgs = [];
		fixedArgs = args.collect({ arg ag,i;
			if(instr.specs.at(i).rate == \scalar,{
				ag.value
			},{
				//TODO: force floats to PatchIn.scalar so you don't waste
				//an extra Control
				patchIns = 
					patchIns.add(PatchIn.newByRate(instr.specs.at(i).rate));
				synthArgs = synthArgs.add(ag);
				nil // not fixed
			})
		});
		
		synthDef = this.instr.asSynthDef(	fixedArgs );		defName = synthDef.name;
		^synthDef
	} // the default Out version

	prepareForPlay { arg group,onComplete;
		group = group.asGroup;
		super.prepareForPlay(group,{
			var unpreparedChildren,aChildIsReady;
			readyForPlay = false;
			unpreparedChildren = this.args.size;
			aChildIsReady = {
				unpreparedChildren = unpreparedChildren - 1;
				if(unpreparedChildren == 0,{
					readyForPlay = true;
					onComplete.value;
				})
			};
			this.args.do({ arg child,i;
				// is ready, is ready immediately, must thread
				if(child.readyForPlay,aChildIsReady,{
					child.prepareForPlay(group,aChildIsReady)
				});
			});
		});
		^false
	}
	
	spawn { arg atTime;
		argOuts = synthArgs.collect({ arg argh,i;
			argh.spawn(atTime) // returns PatchOut
		});

		// after the children
		synth = Synth.tail(patchOut.group,this.defName,				[0, patchOut.bus.index] ++ 
					argOuts.collect({	arg o,i;
						[ i + 1 , o.value ] // bus indices, float values
					}).flat
		);
		// make the connection
		patchIns.do({ arg pti,i;
			pti.nodeControl_(NodeControl(synth,i + 1));
			// not finding the node until after playing
			argOuts.at(i).connectTo(pti); // scalars hooked up
		});
		^patchOut
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
		patchIns = nil;
		synthArgs.do({ arg argh;
			argh.free;			
		});
		synthArgs = nil;
		argOuts.do({ arg argh; argh.free });
		argOuts = nil;
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

