
Instr  { 

	classvar <>instrDirectory = "Instr/";

	var  <>name, <>func,<>specs,<outSpec;
	
	*new { arg name, func,specs,outSpec;
		^super.newCopyArgs(name.asArray,func).init(specs,outSpec).write;
	}

	init { arg specs,outsp;
		this.makeSpecs(specs ?? {[]});
		
		if(outsp.isNil,{
			outSpec = ArOutputSpec.new;
		},{
			outSpec = outsp.asSpec;
		});

		// it should be possible to divine the out spec from 
		// valuing the function.
		// as long as all args are supplied (no nils)
		// use spec.default
				
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
						sp = sp.asSpec ?? {ControlSpec.new}; // [0,1,\linear] 
					},{
						sp = (sp.first ? name).asSpec
					});
				},{
					sp = (sp ? name).asSpec ?? {ControlSpec.new};
				});
				sp.copy; //.name_(name) // not using flyweight objects.  instr own them and name them
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
			die("(Meta_Instr-ar) Instr not found !!" ++ name.asCompileString, thisMethod);
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
		var nonScalarIndices,controlIndices,ci=0;
		var fixedNames="";
		
		outClass = outClass.asClass;
		if(fixedArgs.isNil, { fixedArgs = [] });
		controlIndices = this.specs.collect({ arg spec,i;
					if(spec.rate == \scalar or: {fixedArgs.at(i).notNil},{
						nil
					},{
						nonScalarIndices = nonScalarIndices.add(i);
						ci = ci + 1;
						ci
					})
				});
			
		if(defName.isNil,{ // better to let the Patch specify it, its faster, but no sharing
			defName = "";
			name.do({ arg part;
				defName = defName ++ part.asString;
			});
			defName = defName ++ outClass.name.asString.first.toLower;
			fixedArgs.do({ arg fa,i;
				if(fa.notNil,{
					fixedNames = fixedNames ++ i ++ fa.asCompileString;
				})
			});
			defName = defName ++ fixedNames.asFileSafeString;
		});
		
		
		// should only happen if you are ar or kr out
		^SynthDef.newFromSpecs(defName,{ arg inputs;
			var outIndex,funcArgs;
			//outIndex = inputs.removeAt(0);
			
			// only gets inputs matching controls
			funcArgs = this.specs.collect({ arg spec,i;
			
				if(spec.rate == \audio,{
					fixedArgs.at(i) ?? {In.ar(inputs.at(controlIndices.at(i) ),spec.numChannels)} 
					// not really possible to fix an audio anyway
				},{
					if(spec.rate == \scalar,{
						["fixed", fixedArgs.at(i)].postln;
						fixedArgs.at(i) ?? {this.defArgAt(i)}//or nil:  get def arg
					},{// control
						fixedArgs.at(i) ?? {									if(spec.isKindOf(TrigSpec),{ // create a trig, responds to /c_set
								InTrig.kr(inputs.at(controlIndices.at(i)))
							},{
								// in  already is a control signal.
								// patching involves using /map, not In.kr
								inputs.at(controlIndices.at(i)) // no support for variable channel kr
							})
						} 
					})
				});
			});

			if(outClass !== XOut,{
				outClass.perform(if(this.rate == \audio,\ar,\kr),
							inputs.at(0),this.func.valueArray(funcArgs))
			},{
				outClass.perform(if(this.rate == \audio,\ar,\kr),
							inputs.at(0),xfader.value,this.func.valueArray(funcArgs))
			});
		},
			[[\outIndex,\control,0]] ++ 
				nonScalarIndices.collect({ arg agi;
					[this.argNameAt(agi),specs.at(agi).rate,this.defArgAt(agi)];
				})
		);
	}
	
	writeDefFile {
		// check if needed ?
		this.asSynthDef.writeDefFile;
	}
	write {
		var synthDef;
		synthDef = this.asSynthDef;
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



