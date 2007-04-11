

Instr  { 

	classvar <dir;

	var  <>name, <>func, <>specs, <>outSpec, >path;
	
	// specs are optional, can be guessed from the argnames
	// outSpec is optional, can be determined by evaluating the func
	*new { arg name, func, specs, outSpec = \audio;
		var previous;
		if(func.isNil,{ ^this.at(name) });
		name = this.symbolizeName(name);
		previous = Library.atList(name.copy.addFirst(Instr));
		if(previous.notNil,{
			previous.func = func;
			previous.init(specs,outSpec);
			previous.changed(this);
			^previous
		});
		^super.newCopyArgs(name,func).init(specs,outSpec)
	}
	*put { arg instr;
		^Library.putList([Instr,this.symbolizeName(instr.name),instr].flatten )
	}
	*remove { arg instr;
		^Library.global.removeAt([this,this.symbolizeName(instr.name)].flatten )
	}
	// bulk insert an orchestra of instr
	*orc { arg name, pairs, outSpec = \audio;
		forBy(0,pairs.size-1,2,	{ arg i;
			Instr( [name,pairs@i ],pairs@(i+1),nil,outSpec)
		})
	}
	
	*at { arg  name;
		var search;
		search = this.objectAt(name);
		//if(search.notNil,{ search = search.asInstr; });
		^search
	}

	*ar { arg name, args;
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
		^func.valueArray(inputs ? [])
	}
	valueEnvir { arg inputs;
		^func.valueArrayEnvir(inputs ? []);
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
	
	// set the directory where your library of Instr is to be found
	*dir_ { arg p;
		dir = p.standardizePath ++ "/";
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
	// the default value supplied in the function
	initAt { arg i;  
		^(this.defArgAt(i) ?? {this.specs.at(i).tryPerform(\default)})
	}
	
	defName { ^Instr.symbolizeName(name).collect(_.asString).join($.) }
	asSynthDef { arg args,outClass=\Out;
		var synthDef;
		synthDef = InstrSynthDef.new;
		synthDef.build(this,args,outClass);
		^synthDef
	}

	prepareToBundle { arg group,bundle;
		this.asSynthDef.prepareToBundle(group,bundle);
	}
			
	writeDefFile { arg dir;
		this.asSynthDef.writeDefFile(dir);
	}
	write { arg dir;
		var synthDef;
		synthDef = this.asSynthDef;
		synthDef.writeDefFile(dir);
	}
	// for use in patterns
	store { 
		var args;
		args = this.specs.collect({ arg spec,i;
					if(spec.rate == \control or: spec.rate == \stream,{
						IrNumberEditor(this.initAt(i),spec);
					},{
						spec.defaultControl(this.initAt(i))
					})
				}); 
			^this.asSynthDef(args).store
	}
	asDefName {
		^this.store.name
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
	*choose { arg start;
		// this is only choosing from Instr in memory,
		// it is not loading all possible Instr from file
		^if(start.isNil,{ 
			Library.global.choose(this)
		},{
			Library.global.performList(\choose,([this] ++ this.symbolizeName(start)))
		})
	}
	*leaves { arg startAt; // address array
		if(startAt.isNil,{
			startAt = Library.global.at(this);
			if(startAt.isNil,{ ^[] });
		},{
			startAt = this.at(startAt.first);
		});
		^Library.global.prNestedValuesFromDict(startAt).flat
	}
	*selectBySpec { arg outSpec;
		outSpec = outSpec.asSpec;
		^this.leaves.select({ |ins| ins.outSpec == outSpec })
	}
	*chooseBySpec { arg outSpec;
		^this.selectBySpec(outSpec).choose
	}
	
	//private
	*symbolizeName { arg name;
		if(name.isString,{
			^name.split($.).collect(_.asSymbol);
		 });
	 	if(name.isKindOf(Symbol),{
			^[name];
		});	
		if(name.isSequenceableCollection,{
			^name.collect(_.asSymbol);
		});
		error("Invalid name for Instr : "++name);
	}	

	*objectAt { arg name;
		var symbolized,search,path,pathParts,rootPath;
		symbolized = Instr.symbolizeName(name);
		search = Library.atList(([this] ++ symbolized));
		if(search.notNil,{ ^search });

		// look for a file
		if((path = this.findPath(symbolized)).isNil,{
			^nil
		});
		path.loadPath(false);
		
		// now see if its there
		^Library.atList([this] ++ symbolized);
		// else it returns nil : not found
	}
	*findPath { arg symbolized;
		var quarkInstr,found;
		found = this.findPathIn(symbolized,Instr.dir);
		if(found.notNil,{ ^found });
		
		quarkInstr = (Platform.userExtensionDir ++ "/quarks/*/Instr").pathMatch;
		quarkInstr.do({ |path|
			found = this.findPathIn(symbolized,path);
			if(found.notNil,{ ^found });
		});
		^nil
	}
	*findPathIn { arg symbolized, rootPath;
		var pathParts,path;

		pathParts = symbolized.collect(_.asString);
		// if its a multi-part name then the last item is the instr name
		// and the next to last is the file name.
		// so remove last item :
		if (pathParts.size > 1, { pathParts.pop });

		// .rtf .txt .sc .scd or plain
		rootPath = (rootPath ++ pathParts.join("/"));
		
		path = rootPath ++ ".rtf";
		
		if(File.exists(path),{ ^path });
		path = rootPath ++ ".txt";
		if(File.exists(path),{ ^path });
		path = rootPath ++ ".sc";
		if(File.exists(path),{ ^path });
		path = rootPath ++ ".scd";
		if(File.exists(path),{ ^path });
		path = rootPath;
		if(File.exists(path),{ ^path });
		^nil
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
		var quarkInstr;
		(this.dir ++ "*").pathMatch.do({ arg path; path.loadPath(false) });
		quarkInstr = (Platform.userExtensionDir ++ "/quarks/*/Instr/*").pathMatch;
		quarkInstr.do({ |path|
			path.debug("loading");
			path.loadPath(true);
		});
	}
	*clearAll {
		Library.global.removeAt(this.name)
	}

	asString { ^"Instr(" ++ this.defName.asCompileString ++ ")" }
		
	*initClass {
		Class.initClassTree(Document);
		// default is relative to your doc directory
		if(dir.isNil,{ dir = Document.dir ++ "Instr/"; });
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

	//guiClass { ^InstrGui }
	guiBody { arg layout;
		var defs,tf,source,lines,h,w;
		source = this.func.def.sourceCode;
		lines = source.split($\n);
		w = lines.maxValue({ |l| l.size }) * 10;
		h = lines.size * 20;
		
		tf = GUI.textField.new(layout,Rect(0,0,w,h));
		tf.string = source;
		
		if(path.notNil,{
			ActionButton(layout.startRow,"open file...",{ path.openTextFile });
		});
		ArgNameLabel("outSpec:",layout.startRow,150);
		this.outSpec.gui(layout);
		this.argNames.do({ arg a,i;
			layout.startRow;		
			ArgNameLabel(  a ,layout,150);
			CXLabel(layout, " = " ++ this.defArgAt(i).asString,100);
			this.specs.at(i).gui(layout);
		});

		layout.startRow;
		if(File.exists(this.path),{
			ActionButton(layout,"edit File",{ this.path.openTextFile });
		});
		ActionButton(layout,"make Patch",{ Patch(this.name).topGui });
	}

}


// an object that points to an Instr and is saveable/loadable as a compile string
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


// make a virtual Instr by reading the *ar and *kr method def
// eg Patch(SinOsc,[ 440 ])

UGenInstr { 

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
	asString { ^"UGenInstr " ++ ugenClass.name.asString }
	asInstr { ^this }
	name { ^ugenClass }

}

// see Interface
InterfaceDef : Instr {

	var <>onLoad,
		<>onPlay,
		<>onStop,
		<>onFree,
		
		<>onNoteOn,
		<>onNoteOff,
		<>onPitchBend,
		<>onCC,

		<guiBodyFunction,
		<>keyDownAction,
		<>keyUpAction;
		
		// do your own views to handle these
		//<>beginDragAction,
		//<>mouseDownAction,
		//<>mouseUpAction,
	gui_ { arg function; guiBodyFunction = function; }

}

