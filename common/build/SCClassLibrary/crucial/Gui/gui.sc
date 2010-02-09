+ Object {

	guiClass { ^ObjectGui }
	//guiClass { ^ModelImplementsGuiBody }
	//guiBody {}

	gui { arg  ... args;
		^this.guiClass.new(this).performList(\gui,args);
	}

	topGui { arg ... args;
		^this.guiClass.new(this).performList(\topGui,args);
	}

	smallGui { arg  ... args;
		var class;
		class = this.guiClass;
		if(class.findMethod(\smallGui).notNil,{
			^this.guiClass.new(this).performList(\smallGui,args);
		});
		while ({ class = class.superclass; class !== Object },{
			if(class.findMethod(\smallGui).notNil,{
				^this.guiClass.new(this).performList(\smallGui,args);
			});
		});
		^Tile(this,args.first.asPageLayout)
	}
	// return an array of symbols specifying which vars to put up on the gui
	*instVarsForGui { ^[] }
	*publicInstVars {
		^this.instVarNames.select({ |ivar| this.findRespondingMethodFor(ivar).notNil })
	}

	// Insp
	insp { arg  ... args;
		Insp(this,args);
	}
	// gui into the Insp tabbed browser
	ginsp { arg  ... args;
		Insp(this,args,true);
	}

	debug { arg caller;
		// by using this rather than just postln
		// you can command-shift-y on debug and find every one you left
		if(caller.notNil,{
			Post << caller << ": " << this << Char.nl;
		},{
			Post << this << Char.nl;
		});
	}
	die { arg ... culprits;
		"\n\nFATAL ERROR:\n".postln;
		culprits.do({ arg c; if(c.isString,{c.postln},{c.dump}) });
		this.dump;
		Error("FATAL ERROR").throw;
	}
	checkKind { arg shouldBeKindOf;
		if(this.isKindOf(shouldBeKindOf).not,{
			Error("Type check failed:" + this + "should be kind of:" + shouldBeKindOf).throw;
		})
	}

}

+ Collection {
	debug { arg caller;
		// by using this rather than just postln
		// you can command-shift-y on debug and find every one you left
		if(caller.notNil,{
			if(this.size < 10,{
				Post << caller << ": " << this << Char.nl;
			},{
				Post << caller << ": " <<* this << Char.nl;
			});
		},{
			Post <<* this << Char.nl;
		});
	}
}

// these all get simple guis : just a label with the name
+ String {
	guiClass { ^StringGui }
}
+ Symbol {
	guiClass { ^StringGui }
}
// though it would be nice to have it indicated what class it is
+ SimpleNumber {
	guiClass { ^StringGui }
}
+ Boolean {
	guiClass { ^StringGui }
}
+ Nil {
	guiClass { ^StringGui }
}


+ Pattern {
	// by default gui all public inst vars
	*instVarsForGui {
		^this.publicInstVars
	}
}
+ Pbind {
	guiBody { |f|
		var endval = patternpairs.size-1;
		forBy (0, endval, 2) { arg i;
			f.startRow;
			ArgNameLabel(patternpairs[i],f);
			patternpairs[i+1].gui(f);
		};
	}
}
+ Pswitch  {
//	guiClass { ^PswitchGui }
	guiBody { arg layout;
		this.list.do({ arg l;
			l.gui(layout.startRow);
		});
		this.which.gui(layout);
	}
}

+ Pstutter {
	guiBody { arg layout;
		pattern.gui(layout);
		n.gui(layout.startRow);
	}
}



+ RawArray { // string, signal
	debug { arg caller;
		// by using this rather than just postln
		// you can command-shift-y on debug and find every one you left
		if(caller.notNil,{
			Post << caller << ": " << this << Char.nl;
		},{
			Post << this << Char.nl;
		});
	}
}


+ Class {
	guiClass { ^ClassGui }
}

+ Method {
	guiClass { ^MethodGui }
}

+ Dictionary {
	//guiClass { ^DictionaryGui }
	guiBody { arg layout;
		this.keysValuesDo({ arg k,v,i;
			CXLabel(layout.startRow,k,minWidth: 100);
			Tile(v,layout,200);
		})
	}
}

+ Server {
	guiClass { ^ServerGui }
}
+ Node {
//	guiClass { ^NodeGui }
	guiBody { arg layout;
		Tile(this.server,layout);
		Tile(this.group,layout);
		ActionButton(layout,"trace",{
			this.trace;
		});
		ActionButton(layout,"query",{
			this.query;
		});
	}
}
+ Synth {
//	guiClass { ^SynthGui }
	guiBody { arg layout;
		CXLabel(layout,this.defName);
		super.guiBody(layout);
	}
}



/* layout support */
+ Nil {

	asPageLayout { arg name,bounds,metal=true;
		^MultiPageLayout(name.asString,bounds, metal: metal ).front
	}
	asFlowView { arg bounds;
		^FlowView(nil,bounds)
	}
	remove {}
}

+ Point {
	asPageLayout {
		^MultiPageLayout("",this.asRect ).front
	}
}
+ Rect {
	asPageLayout {
		^MultiPageLayout("",this ).front
	}
}


+ FlowView {
	asFlowView {}
	asPageLayout {} // should be compatible
}

+ MultiPageLayout {
	asFlowView { arg bounds;
		^if(bounds.notNil,{
			FlowView(this,bounds)
		},{
			this.view
		})

		//bounds = bounds ?? {this.view.bounds};
		//^FlowView(this.view,this.layRight(bounds.width - 10,bounds.height - 10))
	}
}

