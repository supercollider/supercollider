
Instr  { 

	classvar <>instrDirectory = "Instr/";

	var  <>name, <>func,<>specs,<outSpec;
	
	*new { arg name, func,specs,outSpec;
		^super.newCopyArgs(name.asArray,func).init(specs,outSpec).write;
	}

	init { arg specs,outsp;
		this.makeSpecs(specs ?? {[]});
		
		if(outsp.isNil,{
			outSpec = nil; //ArOutputSpec.new;
			// it should be possible to divine the out spec from 
			// valuing the function.
		},{
			outSpec = outsp.asSpec;
		});

		this.class.put(this);
	}
	
	makeSpecs { arg argspecs;
		specs =  
			Array.fill(this.argsSize,{ arg i;
				var sp,name;
				name = this.argNameAt(i);
				sp = argspecs.at(i);
				if(sp.isSequenceableCollection,{ 
					// backwards compatibility with old spec style
					// [\envperc]
					// [[0,1]]
					// [StaticSpec()]
					// [0,1]
					if(sp.first.isNumber,{
						sp = sp.asSpec;
					},{
						sp = (sp.first ? name).asSpec
					});
				},{
					sp = (sp ? name).asSpec ?? {ControlSpec.new};
				});
				sp.copy; 
			});
	}

	rate { ^outSpec.rate }
	numChannels { ^outSpec.numChannels }

	
	*put { arg instr;
		^Library.putList([this.name,instr.name,instr].flatten )
	}
	*at { arg  name;
		^(Library.atList([this.name] ++ name.asArray)
			??
		{ 	
			// if not previously loaded, try loading the file
			loadPath(instrDirectory ++ [name].flat.first.asString ++ ".rtf");
		}
		)
	}

	*ar { arg  name, args;
		var instr;
		instr=this.at(name);
		if(instr.isNil,{
			die("(Meta_Instr-ar) Instr not found !!" 
					+ name.asCompileString, thisMethod);
		},{
			^instr.valueArray(args)
		})
	}
	
	ar { arg ... inputs;
		^func.valueArray(inputs);
	}
	value { arg inputs;
		^func.valueArray(inputs)
	}
	valueEnvir { arg inputs;
		^func.valueArrayEnvir(inputs);
	}
	valueArray { arg inputs;
		^func.valueArray(inputs)
	}
	kr{ arg ... inputs;
		^func.valueArrayEnvir(inputs);
	}

	maxArgs { ^this.argsSize }
	argsSize { ^func.def.argNames.size }
	argNames { ^(func.def.argNames ?? {[]}).asList }
	defArgs { ^(func.def.prototypeFrame ?? {[]}).asList }

	argNameAt { arg i; 
		var nn;
		nn=func.def.argNames;
		^if(nn.notNil,{nn.at(i)},nil);
	}
	defArgAt { arg i; 
		var nn;
		nn=func.def.prototypeFrame;
		^nn.at(i) ?? {specs.at(i).default}
	}
	initAt { arg i;  ^(this.defArgAt(i) ?? {this.specs.at(i).default}) }
	
	/*  server support */
	defName {
		// String-join would be better
		var n;
		n = "";
		name.do({ arg part;
			n = n ++ part.asString ++ ".";
		});
		^n.copyRange(0,n.size - 2)
	}
	
	asSynthDef { arg fixedArgs,outClass = \Out,xfader,defName;
		//xfader is only if using XOut
		var isScalarOut,nonScalarIndices,controlIndices,ci=0;
		var fixedNames="",inputSpecs;
		
		// must supply an output spec if you are doing scalar
		// otherwise we can determine the spec from the result.
		// eg. SendTrig which returns 0.0
		// other scalars can't be returned in a SynthDef
		isScalarOut = outSpec.notNil and: {outSpec.rate == \scalar};
		if(isScalarOut,{ ci = -1; }); // first arg is NOT \out
		
		outClass = outClass.asClass;
		if(fixedArgs.isNil, { fixedArgs = [] });
		controlIndices = 
				this.specs.collect({ arg spec,i;
					if(spec.rate == \scalar or: {fixedArgs.at(i).notNil},{
						nil
					},{
						nonScalarIndices = nonScalarIndices.add(i);
						ci = ci + 1;
						ci
					})
				});
			
		if(defName.isNil,{ // better to let the Patch specify it, 
						// its faster, but no reuseability of synthDef files
			defName = "";
			name.do({ arg part;
				defName = defName ++ part.asString.asFileSafeString;
			});
			defName = defName ++ outClass.name.asString.first.toLower;
			fixedArgs.do({ arg fa,i;
				if(fa.notNil,{
					fixedNames = fixedNames ++ i ++ fa.asCompileString;
				})
			});
			defName = defName ++ fixedNames.asFileSafeString;
		});
		
		if(isScalarOut.not,{
			inputSpecs = 	[[\out,\control,0]]; // first arg is always \out buss index
		},{
			inputSpecs = [];
		});
		inputSpecs = inputSpecs ++
				nonScalarIndices.collect({ arg agi;
					[this.argNameAt(agi),
					specs.at(agi).rate,
					this.defArgAt(agi)];
				});

		^SynthDef.newFromSpecs(defName,{ arg inputs;
			var outIndex,funcArgs,out,anOutChannel;
			// only gets inputs matching controls
			funcArgs = this.specs.collect({ arg spec,i;
				if(spec.rate == \audio,{
					fixedArgs.at(i) ?? 
						{
						In.ar(inputs.at(controlIndices.at(i)), 
						spec.numChannels)} 
					// not possible to fix an audio anyway
				},{
					if(spec.rate == \scalar,{
						("fixed arg:" + fixedArgs.at(i)).postln;
						fixedArgs.at(i) ?? {this.defArgAt(i)}
					},{// control
						fixedArgs.at(i) ?? {									if(spec.isKindOf(TrigSpec),{ // create a trig,
												// responds to /c_set
								InTrig.kr(inputs.at(controlIndices.at(i)))
							},{
								// input already is a control signal.
								// patching involves using /map, not In.kr
								inputs.at(controlIndices.at(i)) 
								// no support for multi channel kr
							})
						} 
					})
				});
			});
			out = this.func.valueArray(funcArgs);
			// lets guess, look at what the ugenFunc returned
			if(outSpec.isNil,{
				anOutChannel = if(out.isSequenceableCollection,
									{out.first},{out});
				if(anOutChannel.rate == \audio,{
					outSpec = AudioSpec(out.size);
					// MultiTrackAudioSpec must be explicitly stated
					// in the Instr def
				},{
					if(anOutChannel.rate == \control,{
						// \bipolar, \unipolar etc.
						outSpec = anOutChannel.signalRange.asSpec;
						// TrigSpec must be explictly stated
						// in the Instr def
					},{ // or scalar
						if(anOutChannel.isFloat,{
							outSpec =  ScalarSpec.new;//or StaticSpec
							// a SendTrig etc. can end with a 0.0
							// so don't add an Out, but still needs an spec.
						} , { 				
							die("can't handle this scalar output from SynthDef:"
							+ out + "@0: " + anOutChannel);
						});
					})
				})
			});
			if(isScalarOut.not,{ // wrap it in an Out
				if(outClass !== XOut,{
					out = outClass.perform(if(this.rate == \audio,\ar,\kr),
								inputs.at(0),out)
				},{
					out = outClass.perform(if(this.rate == \audio,\ar,\kr),
								inputs.at(0),xfader.value,out)
				});
			});
			out
		},inputSpecs);
	}
	
	writeDefFile {
		// TODO: check if needed
		this.asSynthDef.writeDefFile;
	}
	write {
		var synthDef;
		synthDef = this.asSynthDef;
		//synthDef.dumpUGens;
		synthDef.writeDefFile;
		this.writePropertyList(synthDef);
	}
	writePropertyList { arg synthDef;
		/*
			current model supports non-modulatable inputs
			that don't make it into the synthDef.
		*/
		Dictionary[
			"name" -> this.name.collect({ arg item; item.asString }),
			"defName" -> synthDef.name,
			"argNames" -> this.argNames.asArray,
			"inputSpecs" -> specs.collect({ arg sp,i;
								sp.asPropertyList
							}),
			"outputSpec" -> outSpec.asPropertyList
		].writeAsPlist("synthdefs/" ++ synthDef.name ++ ".plist");
	}
	
	
	
	test {
		var p;
		p = Patch(this.name);
		p.topGui;
		^p
	}
/*
	*choose { arg startAt;
		var page;
		if(startAt.isNil,{
//			startAt = Library3.at(Instr);
			page = Roster(":rosters:EfxOrcs").all.choose;
			page.load;
			startAt = Instr.at(PathName(page).fileName.asSymbol);
		},{
			startAt = this.at(startAt);
		});
		^Library3.global.chooseFromNode(startAt);
	}
	
	*loadAll
		or at least register all the names of files
		
*/	
	guiClass { ^InstrGui }
	asString { ^"Instr " ++ this.name.asString }
//	*keys {
//		^Library.global.values.select({ arg w; w.isKindOf(Instr) }).collect({ arg instr; instr.name })
//	}
}



