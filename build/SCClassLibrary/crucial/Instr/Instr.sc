
Instr  { 

	classvar <>instrDirectory = "Instr/";

	var  <>name, <>func,<>specs,<outSpec;
	
	*new { arg name, func,specs,outSpec;
		if(name.isString or: {name.isKindOf(Symbol)},{
			name = [name.asSymbol];
		});
		^super.newCopyArgs(name,func).init(specs,outSpec)//.write;
	}
	//*write go ahead and write it

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

	rate {  
		^if(outSpec.notNil,{ 
			outSpec.rate;
		},{
			// if you aren't audio, you must specify an outSpec
			\audio
		})
	}
	numChannels { 
		^if(outSpec.notNil,{
			outSpec.numChannels 
		},{ // if you are more than one channel, you must specify an outSpec
			1
		});
	}
	path { ^instrDirectory ++ name.first.asString ++ ".rtf" }
	
	*put { arg instr;
		^Library.putList([this.name,instr.name,instr].flatten )
	}
	*at { arg  name;
		^(Library.atList([this.name] ++ name.asArray)
			??
		{ 	
			// if not previously loaded, try loading the file
			(instrDirectory ++ [name].flat.first.asString ++ ".rtf").loadPath;
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
					if(spec.rate == \scalar or: {fixedArgs.at(i).notNil},
						nil
					,{
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
			defName = defName ++ fixedNames.hash.asFileSafeString;
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
					/*  SEEMS RIGHT TO USE THE FIXED ARG FOR THE VALUE */
					fixedArgs.at(agi) ?? {this.defArgAt(agi)}];
				});

		^SynthDef.newFromSpecs(defName,{ arg inputs;
			var outIndex,funcArgs,out,anOutChannel;
			// only gets inputs matching Controls
			
			//fixedArgs.insp("fixedArgs to Instr");
			//inputs.insp("inputs to Instr");
			
			funcArgs = this.specs.collect({ arg spec,i;
				if(spec.rate == \audio,{
					fixedArgs.at(i) ?? 
						{
							In.ar(inputs.at(controlIndices.at(i)), 
								spec.numChannels)
						} 
					// not possible to fix an audio anyway
				},{
					if(spec.rate == \scalar,{
						fixedArgs.at(i) ?? {this.defArgAt(i)}
					},{// control or mixed rate (nil)
						fixedArgs.at(i) ?? {									if(spec.isKindOf(TrigSpec),{ 
								// create a trig, responds to /c_set touching
								InTrig.kr(inputs.at(controlIndices.at(i)))
							},{
								In.kr(inputs.at(controlIndices.at(i)),1 )
								// assumes 1 channel kr for now
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
					outSpec = AudioSpec(if(out.isSequenceableCollection,{out.size},1));
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
							// SendTrig etc.:  no Out
							outSpec =  ScalarSpec.new;//or StaticSpec
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



