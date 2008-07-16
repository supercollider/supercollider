
// abstract  class
HasPatchIns : AbstractPlayer {

	var <patchIns;

	//  private
	didSpawn {
		super.didSpawn;
		//i know of the synth, i hand out the NodeControls
		patchIns.do({ arg patchIn,argi;
			patchIn.nodeControl_(NodeControl(synth,this.argNameAt(argi)));
			this.inputs.at(argi).connectToPatchIn(patchIn,false);
		});
	}
	// currently Patch just hold onto the PatchIns but they are disconnected
	//freeToBundle { |bundle|
	//	super.freeToBundle(bundle);
	//	bundle.addFunction({ patchIns = nil; });
	//}
	//subclassResponsibility
	synthArgsIndices { ^this.subclassResponsibility(thisMethod) }
	inputs { ^this.subclassResponsibility(thisMethod) }

	// the following methods all have to do with querying and iterating over
	// the inputs to this player.  an example is finding all possibly modulatable
	// inputs in this or any sub-patch and then assigning available controllers to those.
	// these methods are for querying for PlayerInputProxy that are placeholders for
	// a potentially modulateable input
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

	inputProxies { // just this patch
		^this.inputs.select({ arg a; a.isKindOf(PlayerInputProxy) })
	}
	// do a deep search through this Patch and any sub-patches and return all
	// PlayerInputProxies [ input , deepOffset, argName, spec ]
	annotatedInputProxies { arg offset=0,array; // [ input , deepOffset, argName, spec ]
		var inputs;
		inputs = this.inputs;
		if(array.isNil,{ array = [] });
		inputs.do({ arg a,i;
			if(a.isKindOf(PlayerInputProxy),{
				array = array.add([a, offset + i, this.argNameAt(i), this.specAt(i) ]);
			},{
				if(a.isKindOf(HasPatchIns),{
					a.annotatedInputProxies(offset + i, array)
				})
			})
		});
		^array
	}

	setInput { arg ai,ag;
		^this.subclassResponsibility(thisMethod)
	}
	// see deepSpecAt below
	setDeepInput { arg ai,ag,offset=0;
		var inputs;
		inputs = this.inputs;
		if(inputs.size + offset > ai,{
			this.setInput(ai - offset, ag);
			^true
		});
		offset = offset + inputs.size;
		^inputs.any({ arg a,i;
			var set=false;
			a.isKindOf(HasPatchIns) and: {
				set = a.setDeepInput(ai,ag,offset + i);
				offset = offset + a.inputs.size;
				set
			}
		})
	}
	// this finds the spec of an input in a subpatch.
	// argi is an index into an array of all args of this patch followed by depth first traversal of all subpatches
	deepSpecAt { arg argi,offset=0;
		var inputs,deepSpec;
		inputs = this.inputs;
		if(inputs.size + offset > argi,{
			^this.specAt(argi - offset)
		});
		offset = offset + inputs.size;
		inputs.detect({ arg a,i;
			a.isKindOf(HasPatchIns) and: {
				deepSpec = a.deepSpecAt(argi,offset + i);
				offset = offset + a.inputs.size;
				deepSpec.notNil
			}
		});
		^deepSpec
	}
}

Patch : HasPatchIns  {

	var <instr,<args;
	var synthPatchIns,<argsForSynth,<argNamesForSynth,<synthArgsIndices;

	var <synthDef,<>outClass;
	var numChannels,rate; // determined after making synthdef

	var <stepChildren;

	*new { arg name,inputs,outClass;
		^super.new.loadSubject(name).createArgs(loadDocument(inputs) ? []).outClass_(outClass ? Out)
	}
	rand { arg standardDeviation=0.15;
		this.inputs.do({ |in,i|
			// at least NumberEditors will respond
			in.tryPerform(\rand,standardDeviation,this.instr.initAt(i));
		})
	}
	inputs { ^args }
	// insert a new input into this arg position
	setInput { arg index, newArg;
		//ISSUE if it wasn't a synth input before it won't become one now
		// but you can respawn
		var synthArgi;
		args.at(index).removeDependant(this);
		newArg.addDependant(this);
		args.put(index,newArg);
		synthArgi = synthArgsIndices.at(index);
		if(synthArgi.notNil,{
			argsForSynth.put(synthArgi,newArg);
		});
	}
	args_ { arg inputs;
		// if you didn't give enough then set defaults for the others
		if(inputs.size != args.size,{
			Error("Args are not the correct size, should be "+args.size).throw;
		});
		inputs.do({ |in,i|
			this.setInput(i,in)
		});
	}
	set { arg index, value;
		var argg;
		if(index.isKindOf(Symbol),{
			index = this.argNames.detectIndex({ |an| an == index });
			if(index.isNil,{
				Error("Key not found in argNames:"+index).throw
			});
		});
		argg = args[index];
		if(argg.respondsTo('set'),{
			argg.set(value);
		},{
			(argg.asString + "does not respond to set").warn;
		});
	}
	doesNotUnderstand { arg selector ... dnuargs;
		var sel,setter,argName,index;
		sel = selector.asString;
		setter = sel.last == $_;
		if(setter,{
			argName = sel.copyRange(0,sel.size-2).asSymbol;
		},{
			argName = sel.asSymbol;
		});
		index = this.argNames.detectIndex({ |an| an == argName });
		if(index.isNil,{
			^this.superPerformList(\doesNotUnderstand, selector, dnuargs);
		});
		if(setter,{
			this.setInput(index,dnuargs[0])
		},{
			^args[index]
		});
	}
	argNames { ^this.instr.argNames }
	argNameAt { arg i; ^instr.argNameAt(i) }
	specAt { arg i; ^instr.specs.at(i) }
	// out
	spec {
		^this.instr.outSpec ?? {
			if(this.rate.notNil and: this.numChannels.notNil,{
				if(this.rate == \audio,{
					AudioSpec(this.numChannels)
				},{
					// having built, I think we should know better than this
					// check the final result of the instr synth def build,
					// check its signalRange
					ControlSpec(-1,1)
				})
			},{
				//("This patch "+this+"does not yet know its output rate until it is built.").warn;
				nil
			});
		};
	}
	rate {
		^rate ?? {
			if(this.instr.outSpec.notNil,{
				rate = this.instr.outSpec.rate
			},{
				// guess, but don't cache that guess
				'audio'
			});
		};
	}
	numChannels {
		^numChannels ?? {
			if(this.instr.outSpec.notNil,{
				numChannels = this.instr.outSpec.numChannels
			},{
				//( this.asString + ": numChannels is unknown").warn;
				nil
			});
		}
	}

	indexFromName { arg argName;
		^instr.argNames.indexOf(argName)
	}
	argFromName { arg argName;
		var index;
		index = this.indexFromName(argName);
		if(index.notNil,{
			^args.at(index)
		},{
			^nil
		})
	}

	loadSubject { arg name;
		if(instr.notNil,{
			instr.removeDependant(this);
		});
		instr = name.asInstr;
		if(instr.isNil,{
			("Instrument not found !!" + name).die;
		});
		//instr.addDependant(this);
	}

	createArgs { arg argargs;
		var argsSize;
		argsForSynth = [];
		argNamesForSynth = [];
		patchIns = [];
		synthPatchIns = [];
		argsSize = this.instr.argsSize;
		synthArgsIndices = Array.newClear(argsSize);

		args=Array.fill(argsSize,{arg i;
			var proto,spec,ag,patchIn,darg,inSpec;
			spec = instr.specs.at(i);
			if(argargs.at(i).notNil,{
				ag = argargs[i];//.asInputForPatch(spec,argName,initAt);
				/*
					check the spec of the incoming arg if possible.
					compare it to required spec.
				*/
				inSpec = ag.tryPerform(\spec);
				if(inSpec.notNil and: {inSpec.class !== spec.class},{
					// spec validation
					// only if you did supply an explicit spec
					if(spec.canAccept(ag).not and: {instr.explicitSpecs[i] == spec},{
						//Error(
							String.streamContents({ arg stream;
								stream << "For " <<< this.instr << Char.nl;
								stream << " in " << this << Char.nl;
								stream << "The supplied input: " << Char.nl;
									stream << Char.tab << ag << Char.nl;
								stream << "With spec: " << Char.nl;
									stream <<	 Char.tab <<< inSpec << Char.nl;
								stream << "does not match the Instr's spec:" << Char.nl;
									stream << Char.tab <<< spec << Char.nl;
								stream << "for arg: " << instr.argNameAt(i) << Char.nl;
							}).warn
							//).throw;
					});
				});
			},{
				//  or auto-create a suitable control...
				darg = instr.initAt(i);
				if(darg.isNumber,{
					proto = spec.defaultControl(darg);
				},{
					proto = spec.defaultControl;
				});
				ag = proto
			});
			patchIn = PatchIn.newByRate(spec.rate);
			patchIns = patchIns.add(patchIn);

			// although input is control, arg could overide that
			if(spec.rate != \noncontrol
				and: {ag.rate != \noncontrol}
			,{
				// if rate is \stream and spec is not EventStream
				// then fail
				argsForSynth = argsForSynth.add(ag);
				argNamesForSynth = argNamesForSynth.add(this.argNameAt(i));
				synthPatchIns = synthPatchIns.add(patchIn);
				synthArgsIndices.put(i,synthPatchIns.size - 1);
			},{
				// watch noncontrols for changes.
				// if Env or Sample or quantity changed, synth def is invalid
				//if(ag.isNumber.not,{ ag.addDependant(this); });
			});
			ag
		});
	}

	asSynthDef {
		// could be cached, must be able to invalidate it
		// if an input changes
		^synthDef ?? {
			if(this.spec.rate == 'stream',{
				("Output rate is 'stream', not yet supported").warn;
				^nil
			});
			synthDef = InstrSynthDef.build(this.instr,this.args,this.outClass);
			defName = synthDef.name;
			// the synthDef has now evaluated and can know the number of channels
			// but if it returned an Out.ar then it does not know
			// so we will have to trust the Instr outSpec 
			if(synthDef.numChannels.notNil,{
				numChannels = synthDef.numChannels;
			});
			if(synthDef.rate.notNil,{
				rate = synthDef.rate;
			});
			this.watchNoncontrols;
			this.instr.addDependant(this);
			stepChildren = synthDef.secretObjects;

			synthDef
		}
	}
	watchNoncontrols {
		this.args.do({ arg ag,i;
			if(this.specAt(i).rate === \noncontrol
				or: {ag.rate === \noncontrol}
			,{
				// watch scalars for changes.
				// if Env or Sample or quantity changed, synth def is invalid
				if(ag.isNumber.not,{
					ag.addDependant(this);
				});
			});
		})
	}
	update { arg changed,changer;
		var newArgs;
		// one of my scalar inputs changed
		if(this.args.includes(changed),{
			if(this.isPlaying,{
				newArgs = synthDef.secretDefArgs(this.args);
				synth.performList(\set,newArgs);
			},{
				this.invalidateSynthDef;
			})
		});
		if(changed === this.instr,{
			synthDef = nil;
			defName = nil;
			stepChildren = nil;
		});
	}
	invalidateSynthDef {
		synthDef = nil;
		defName = nil;
		stepChildren = nil;
		this.releaseArgs;
		this.instr.removeDependant(this);
	}
	releaseArgs {
		// Sample, Env, NumberEditor are watched
		this.args.do({ arg ag; ag.removeDependant(this) })
	}
	didFree {
		var did;
		did = super.didFree;
		if(did,{ this.invalidateSynthDef; });
		^did
	}

	prepareChildrenToBundle { arg bundle;
		super.prepareChildrenToBundle(bundle);
		stepChildren.do({ arg child;
			child.prepareToBundle(group,bundle,true,nil,false);
		});
	}

	// has inputs
	spawnToBundle { arg bundle;
		var synthArgs;
		if(patchOut.isNil,{
			"PatchOut is nil. Has this been prepared for play ?".die(this,thisMethod.asString);
		});

		// experimental but harmless
		// If I am a builder patch, building a stream
		if(this.rate === 'stream',{
			this.children.do({ arg child;
				child.spawnToBundle(bundle);
			});
			this.stepChildren.do({ arg child;
				child.spawnToBundle(bundle);
			});
			bundle.addMessage(this,\didSpawn);
			^this
		});

		// would be better to track if def is loaded
		// this is a needless extra call
		this.asSynthDef;// make sure it exists

		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		this.stepChildren.do({ arg child;
			child.spawnToBundle(bundle);
		});

		synth = Synth.basicNew(this.defName,this.server);
		this.annotate(synth,"synth");
		NodeWatcher.register(synth);
		bundle.add(
			synth.addToTailMsg(patchOut.group,
				this.synthDefArgs
				++ synthDef.secretDefArgs
			)
		);
		bundle.addMessage(this,\didSpawn);
	}
	didSpawn {
		super.didSpawn;
		this.stepChildren.do({ |child|
			child.didSpawn(this.synth)
		})
	}
	synthDefArgs {
		// not every arg makes it into the synth def
		var args;
		args = Array(argsForSynth.size * 2 + 2);
		argsForSynth.do({ arg ag,i;
			args.add(argNamesForSynth.at(i));
			args.add(ag.synthArg);
		});
		if(patchOut.rate != \scalar,{
			args.add(\out);
			args.add(patchOut.synthArg);
		});
		^args
	}
	defName { ^defName } // super would say 'Patch'

	stopToBundle { arg bundle;
		super.stopToBundle(bundle);
		stepChildren.do({ |sc|
			sc.stopToBundle(bundle)
		})
	}

	// act like a simple ugen function
	// mostly this won't work except with simple UGens
	//ar { arg ... overideArgs;	^this.valueArray(overideArgs) }
	value { arg ... overideArgs;  ^this.valueArray(overideArgs) }
	valueArray { arg  overideArgs;
		var result,usedArgs;
		// each arg is valued as it is passed into the instr function
		usedArgs = args.collect({ arg a,i;
					{
						(overideArgs.at(i) ? a).value;
					}.try({ arg err;
						"Error while argument "+i+"for the function was .valued".postln;
						if(overideArgs.size > 0,{
							"Args passed in to Patch-value:".postln;
							overideArgs.do({ |oa| oa.dump });
						});
						"Patch's args:".postln;
						args.dump;
						err.throw;
					})
				}) ;
		{
			result = instr.valueArray(usedArgs);
		}.try({ arg err;
			("ERROR during Patch-value " + this).postln;
			"ARGS:".postln;
			this.instr.argNames.do({ |an,i|
				("	%:	%".format(an,usedArgs[i])).postln;
			});
			//err.errorString.postln;
			//this.dump;

			err.throw;
		});
		^result
	}

	children { ^args }

	printOn { arg s;
		var n;
		s << this.class.name << "(" <<< instr.dotNotation << ")"; 
		if((n = this.name).notNil,{
			s << "{"++n++"}";
		});
	}
	storeParamsOn { arg stream;
		var last;
		if(this.class === Patch,{ // an indulgence ...
			last = args.size - 1;
			// anything with a path gets stored as abreviated
			stream << "(" <<< this.instr.dotNotation << ",[";
				if(stream.isKindOf(PrettyPrintStream),{ stream.indent(1); });
				args.do({ arg ag,i;
					stream.nl;
					stream <<< enpath(ag);
					if(i != last,{ stream << "," });
				});
				if(stream.isKindOf(PrettyPrintStream),{ stream.indent(-1); });
			stream.nl;
			stream << "])";
		},{
			super.storeParamsOn(stream)
		});
	}
	storeArgs { ^[this.instr.name,args] }
	guiClass { ^PatchGui }
}

