
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
		if(name.isString or: {name.isKindOf(Symbol)},{
			name = [name.asSymbol];
		});

		^(Library.atList([this.name,name].flatten)
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
		^nn.at(i)
	}
	initAt { arg i;  ^(this.defArgAt(i) ?? {this.specs.at(i).defaultControl}) }
	
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
	asSynthDef { arg args,outClass=\Out;
		var synthDef;
		synthDef = InstrSynthDef.new;
		synthDef.build(this,args,outClass);
		^synthDef
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

	test { arg ... args;
		var p;
		p = Patch(this.name,args);
		p.topGui;
		^p
	}
	play { arg ... args;
		^Patch(this.name,args).play
	}
/*
	*choose { arg startAt;
		var page;
		if(startAt.isNil,{
//			startAt = Library3.at(Instr);
			page = Roster(":rosters:EfxOrcs").all.choose;
			page.loadPath;
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



