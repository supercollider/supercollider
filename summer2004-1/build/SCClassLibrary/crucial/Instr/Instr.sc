
Instr  { 

	classvar <dir; // set this in Main-startUp

	var  <>name, <>func,<>specs,<>outSpec,>path;
	
	*new { arg name, func,specs,outSpec=\audio;
		var previous;
		if(name.isString or: {name.isKindOf(Symbol)},{
			name = [name.asSymbol];
		},{
			name = name.collect({ arg n; n.asSymbol });
		});
		previous = Library.atList([this.name,name].flatten);
		if(previous.notNil,{
			previous.func = func;
			previous.init(specs,outSpec);
			previous.changed(this);
			^previous
		});
		^super.newCopyArgs(name,func).init(specs,outSpec)//.write;
	}
	*initClass {
		Class.initClassTree(Document);
		// default is relative to your doc directory
		if(dir.isNil,{ dir = Document.dir ++ "Instr/"; });
	}
	*dir_ { arg p;
		dir = p.standardizePath ++ "/";
	}
	
	init { arg specs,outsp;
		this.makeSpecs(specs ? #[]);
		if(outsp.isNil,{
			outSpec = nil;
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
				//sp.copy; 
				sp
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
	path { ^path ?? {dir ++ name.first.asString ++ ".rtf"} }
	
	*put { arg instr;
		^Library.putList([this.name,instr.name,instr].flatten )
	}
	*at { arg  name;
		var fullname,instr,path;
		if(name.isString or: {name.isKindOf(Symbol)},{
			name = [name.asSymbol];
		},{
			name = name.collect({ arg n; n.asSymbol });
		});

		^(Library.atList(fullname = [this.name,name].flatten)
			??
		{ 	
			// if not previously loaded, try loading the file
			path = (dir ++ [name].flat.first.asString ++ ".rtf");
			path.loadPath;
			instr = Library.atList(fullname);
			if(instr.isKindOf(Instr),{ 
				instr.path = path; 
			});
			instr // or nil, not found
		}
		)
	}

	*ar { arg  name, args;
		var instr;
		instr=this.at(name);
		if(instr.isNil,{
			die("(Meta_Instr-ar) Instr not found !!" 
					+ name.asCompileString +  thisMethod);
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
	kr { arg ... inputs;
		^func.valueArrayEnvir(inputs);
	}
	next { arg ... inputs;
		^func.valueArray(inputs)
	}

	maxArgs { ^this.argsSize }
	argsSize { ^func.def.argNames.size }
	argNames { ^(func.def.argNames ? []).asList }
	defArgs { ^(func.def.prototypeFrame ? []).asList }

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

	prepareToBundle { arg group,bundle;
		this.asSynthDef.prepareToBundle(group,bundle);
	}
			
	writeDefFile {
		this.asSynthDef.writeDefFile;
	}
	write {
		var synthDef;
		synthDef = this.asSynthDef;
		synthDef.writeDefFile;
		this.writePropertyList(synthDef);
	}
	writePropertyList { arg synthDef;
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

	asSingleName {
		^String.streamContents({ arg s;
			name.do({ arg n,i;
				if(i > 0,{ s << $~ });
				s << n;
			})
		})
	}
	*singleNameAsNames { arg singleName;
		^singleName.split($~).collect({ arg n; n.asSymbol })
	}
	
	*loadAll {
		(this.dir ++ "*.rtf").pathMatch.do({ arg path; path.loadPath })
	}
	*clearAll {
		Library.global.removeAt(this.name)
	}

	*choose { arg start;
		// todo: scan directory first
		^if(start.isNil,{ 
			Library.global.choose(this.name)
		},{
			Library.global.performList(\choose,[this.name] ++ start)
		})
	}
	*leaves { arg startAt; // address array
		if(startAt.isNil,{
			startAt = Library.global.at(this.name);
			if(startAt.isNil,{ ^[] });
		},{
			startAt = this.at(startAt.first);
		});
		^Library.global.prNestedValuesFromDict(startAt).flat
	}
	asString { ^"Instr " ++ this.name.asString }

	//guiClass { ^InstrGui }
	guiBody { arg layout;
		var defs;
		this.outSpec.gui(layout);
		this.argNames.do({ arg a,i;
			layout.startRow;		
			ArgNameLabel(  a ,layout,150);
			CXLabel(layout, " = " ++ this.defArgAt(i).asString,100);
			this.specs.at(i).gui(layout);
		});

		layout.startRow;
		ActionButton(layout,"edit File",{ this.path.openTextFile });
		ActionButton(layout,"make Patch",{ Patch(this.name).topGui });
	}
}


// a saveable, reloadable reference by name
InstrAt {
	var <>name,<>path,instr;
	*new { arg name;
		^super.new.name_(name).init
	}
	init {
		instr = Instr.at(name);
	}
	value { arg ... args; instr.valueArray(args) }
	valueEnvir { arg ... args; ^instr.valueEnvir(args) }
	prepareToBundle { arg group,bundle;
		instr.prepareToBundle(group,bundle);
	}
	asDefName { ^instr.asDefName }
	storeArgs { ^[name] }
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



