

Interface : AbstractPlayerProxy {

	var <interfaceDef,<args,environment;
	
	var <>onPlay,
		<>onStop, // also on command-.
		<>onFree, // not on command-.
		
		<>onNoteOn,
		<>onNoteOff,
		//<>onPitchBend,
		<>onCC,
		
		<guiFunction,
		<guiBodyFunction,
		<>keyDownAction,
		<>keyUpAction;

	var nor,nfr,ccr;
	
	*new { arg interfaceDef,args;
		^super.new.init(interfaceDef,args);
	}
	init { arg i,a;
		// string,symbol,array of names, function or interface def
		interfaceDef = i.asInterfaceDef;
		args = this.createArgs( a.asArray );
		environment = Environment.new;

		// if you change an arg, must rebuild
		interfaceDef.argNames.do({ arg argName,i;
			environment.put(argName.asSymbol,args.at(i));
		});
		environment.use({
			source = interfaceDef.valueArray(args);
			// valueArray causes the last arg to expand
			//.valueArray(args.collect({ arg a; a.value }));
		})
	}
	createArgs { arg argargs;
		^Array.fill(interfaceDef.argsSize,{arg i; 
			var spec,proto,darg,ag;
			ag = argargs.at(i) // explictly specified
			?? 
			{ //  or auto-create a suitable control...
				spec = interfaceDef.specs.at(i);
				darg = interfaceDef.initAt(i);
				if(darg.isNumber,{
					proto = spec.defaultControl(darg);
				},{
					proto = spec.defaultControl;
				});
				proto
			};
			ag.addDependant(this);
			ag
		});
	}
	
		
	// on play
	didSpawn {
		super.didSpawn;
		environment.use(onPlay ? interfaceDef.onPlay);
		// if midi, install
		if(onNoteOn.notNil,{
			nor = NoteOnResponder({ arg num,veloc; environment.use({onNoteOn.value(num,veloc)}) });
		});
		if(onNoteOff.notNil,{
			nfr = NoteOffResponder({ arg num,veloc; environment.use({onNoteOff.value(num,veloc)}) });
		});
		if(onCC.notNil,{
			ccr = CCResponder({ arg num,value; environment.use({onCC.value(num,value)}) });
		});
	}
	didStop {
		super.didStop;
		environment.use(onStop ? interfaceDef.onStop);
		if(nor.notNil,{ nor.remove; nor = nil; });
		if(nfr.notNil,{ nfr.remove; nfr = nil; });
		if(ccr.notNil,{ ccr.remove; ccr = nil; });
	}
	freeToBundle { arg b;
		super.freeToBundle(b);
		b.addFunction({ environment.use(onFree ? interfaceDef.onFree) });
	}
	gui_ { arg f; guiBodyFunction = f; }
	guiClass { ^InterfaceGui }
	guiBody { arg ... args;
		var g,layout,a;
		layout = args.first;
		
		a = keyDownAction ? interfaceDef.keyDownAction;
		if(a.notNil,{
			layout.keyDownAction = { environment.use(a) };
		});
		a = keyUpAction ? interfaceDef.keyUpAction;
		if(a.notNil,{
			layout.keyUpAction = { environment.use(a) };
		});
		
		g = guiBodyFunction ? interfaceDef.guiBodyFunction;
		if(g.notNil,{
			environment.use({ g.valueArray(([layout,this] ++ args.copyRange(1,args.size-1))) })
		},{
			args.do({ arg a,i;
				if(i > 0,{
					layout.startRow;
					a.gui(layout);
				})
			})
		})
	}
	
	use { arg function;
		environment.use(function);
	}
	
	storeArgs { ^[interfaceDef.name,args] }
	storeModifiersOn { arg stream;
		// gui
		// midi
		// etc.
			// stored as instr name references
	}
}
