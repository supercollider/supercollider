
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
		var fullname;
		if(name.isString or: {name.isKindOf(Symbol)},{
			name = [name.asSymbol];
		});

		^(Library.atList(fullname = [this.name,name].flatten)
			??
		{ 	
			// if not previously loaded, try loading the file
			(instrDirectory ++ [name].flat.first.asString ++ ".rtf").loadPath;
			Library.atList(fullname) // or not found
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
			page = Roster("EfxOrcs").all.choose;
			page.loadDocument;
			startAt = Instr.at(PathName(page).fileName.asSymbol);
		},{
			startAt = this.at(startAt);
		});
		^Library.global.chooseFromNode(startAt);
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



UGenInstr { // make a virtual Instr by reading the *ar and *kr method def

	var <ugenClass,<rate,<specs;
	
	*new { arg ugenClass,rate=\ar;
		^super.new.init(ugenClass,rate)
	}
	storeArgs {
		^[ugenClass , rate ]
	}
	init { arg uc,r;
		ugenClass = uc.asClass;
		rate = r;
	
		//specs
		specs = this.argNames.collect({ arg ag,i;
			ag.asSpec ?? {
				("UGenInstr-init  spec not found for:" + ag).warn;
			}
		});
	}

	value { arg args;
		^ugenClass.performList(rate,args)	
	}
	valueArray { arg args;
		^ugenClass.performList(rate,args)
	}

	ar { arg ... args; ^this.value(args) }	
	kr { arg ... args; ^this.value(args) }

	funcDef { ^ugenClass.class.findMethod(rate) }
	maxArgs { ^this.argsSize }
	argsSize { ^this.funcDef.argNames.size - 1 }
	argNames { 
		var an;
		an = this.funcDef.argNames;
		^if(an.isNil,{
			[]
		},{
			an.copyRange(1,an.size - 1)
		})
	}	

	//defaultArgs
	defArgs { 
		var nn;
		nn=this.funcDef.prototypeFrame;
		^if(nn.notNil,{nn.copyRange(1,nn.size-1)},{[]});
	}

	initAt { arg i;  ^(this.defArgAt(i) ?? {this.specs.at(i).default}) }
	argNameAt { arg i; 
		var nn;
		nn=this.funcDef.argNames;
		^if(nn.notNil,{nn.at(i + 1)},{nil});
	}
	defArgAt {
		 arg i; 
		var nn;
		nn=this.funcDef.prototypeFrame;
		^if(nn.notNil,{nn.at(i + 1)},{nil});
	}
		
//	guiClass { ^UGenInstrGui }
	asString { ^"UGenInstr." ++ ugenClass.name.asString }
	asInstr { ^this }
	name { ^ugenClass }

}





InstrSynthDef : SynthDef {

	var fixedNames,fixedValues,fixedPositions;

	// secret because the function doesn't see them
	// but they are needed to pass into the synth
	var secretIrPairs, secretKrPairs;
	
	var <rate,<numChannels; //after build

	*new { // arg name,rate,numChannels,secretIrPairs,secretKrPairs,bytes;
		^super.prNew
	}
	*build { arg instr,args,outClass = \Out;
		^super.prNew.build(instr,args,outClass)
	}
	build { arg instr,args,outClass= \Out;
		this.initBuild
			.buildUgenGraph(instr,args ? #[],outClass)
			.finishBuild
	}
	buildUgenGraph { arg instr,args,outClass;
		var result,fixedID="";
		var isScalarOut;
		var outputProxies;
		
		// need to know the output type now
		
		// OutputProxy In InTrig Float etc.		
		outputProxies = this.buildControlsWithObjects(instr,args);
							
		/*
			not all of the controls are Controls,
			some are what the object chose to add
			in addToSynthDef
		*/

		result = instr.valueArray(outputProxies);
		rate = result.rate;
		numChannels = max(1,result.size);
		
		if(result != 0.0,{
			outClass = outClass.asClass;
			if(outClass === XOut,{
				"XOut not tested yet.".error;
				//out = outClass.perform(if(this.rate == \audio,\ar,\kr),
				//			inputs.at(0),xfader.value,out)
			});
				
			if(rate == \audio,{
				this.addKr(\out,0);
				result = outClass.ar(Control.names([\out]).kr([0]) , result);
				// can still add Out controls if you always use \out, not index
			},{
				if(rate == \control,{
					this.addKr(\out,0);
					result = outClass.kr(Control.names([\out]).kr([0]) , result);
				},{
					("InstrSynthDef: scalar rate ? result of your function:" + result).error;
				})
			});
		});


		//is based on the instr name, ir, kr pattern, fixedValues
		// outClass,numChannels (in case it expanded)
		name = "";
		instr.name.do({ arg part;
				name = name ++ part.asString.asFileSafeString;
		});
		if(name.size > 10,{
			name = name.copyRange(0,9) ++ name.copyRange(10,name.size - 1).hash.asFileSafeString;
		});
		name = name ++ outClass.name.asString.first.toUpper;
		// TODO: ir, kr pattern
		fixedValues.do({ arg fa,i;
			if(fa.notNil,{
				fixedID = fixedID ++ i ++ fa.asCompileString;
			})
		});
		name = name ++ fixedID.hash.asFileSafeString;
		("InstrSynthDef building:" + name ).postln;
	}
	
	// passed to Instr function but not to synth
	addInstrOnlyArg { arg name,value;
		fixedNames = fixedNames.add(name);
		fixedValues = fixedValues.add(value);
		fixedPositions = fixedPositions.add(controlsSize);
		controlsSize = controlsSize + 1;
	}
	
	// to cache this def, this info needs to be saved
	// argi points to the slot in objects (as supplied to secretDefArgs)
	// selector will be called on that object to produce the synthArg
	// thus sample can indicate itself and be asked for \tempo or \bufnum
	addSecretIr { arg name,value,argi,selector;
		secretIrPairs = secretIrPairs.add([name,value,argi,selector]);
		^Control.names([name]).ir([value])
	}
	addSecretKr { arg name,value,argi,selector;
		secretKrPairs = secretKrPairs.add([name,value,argi,selector]);
		^Control.names([name]).kr([value])
	}
	secretDefArgs { arg objects;
		var synthArgs,size;
		size = secretIrPairs.size * 2 + (secretKrPairs.size * 2);
		if(size == 0, { ^#[] });
		synthArgs = Array(size);
		secretIrPairs.do({ arg n,i;
			synthArgs.add(n.at(0)); // secret arg name
			synthArgs.add(objects.at(n.at(2)).perform(n.at(3)));
		});
		secretKrPairs.do({ arg n,i;
			synthArgs.add(n.at(0));
			synthArgs.add(objects.at(n.at(2)).perform(n.at(3)));
		});
		^synthArgs
	}

	buildControlsWithObjects { arg instr,objects;
		var argNames,defargs,outputProxies;
		objects.do({ arg obj,argi; obj.initForSynthDef(this,argi) });
		defargs = (argNames = instr.argNames).collect({ arg name,defargi;
			var defarg;
			defarg = (objects.at(defargi) ?? {instr.defArgAt(defargi)});
			defarg.addToSynthDef(this,name);
			defarg
		});
		outputProxies = this.buildControls;//.insp("buildControls");
		// wrap them in In.kr etc. if needed
		^outputProxies.collect({ arg outp,i;
			defargs.at(i).instrArgFromControl(outp,i)
		})
	}
	
	
//	tempoKr {
//		// have to include in cacheing, playing
//		tempoKr ?? {
//			this.addSecretKr('__tempo__',Tempo.tempo, ... )
//		}
//	}

	*initClass {
		// TODO make this only start when first put
		Class.initClassTree(Server);
		SimpleController(Server.local)
			.put(\serverRunning,{
				//if(Server.local.serverRunning,{
					"Clearing AbstractPlayer SynthDef cache".inform;
					Library.put(SynthDef,Server.local,nil)
				//})
			});
		SimpleController(Server.internal)
			.put(\serverRunning,{
				//if(Server.local.serverRunning,{
					Library.put(SynthDef,Server.internal,nil)
				//})
			});
		//will fail with other servers
	}
}


