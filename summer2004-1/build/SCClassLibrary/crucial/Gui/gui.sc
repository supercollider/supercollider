
+ Object {

	guiClass { ^ModelImplementsGuiBody }
	guiBody {}
	
	gui { arg  ... args; 
		^this.guiClass.new(this).performList(\gui,args);
	}
	
	topGui { arg ... args; 
		^this.guiClass.new(this).performList(\topGui,args);
	}
	
	smallGui { arg  ... args;
		if(this.guiClass.findMethod(\smallGui).notNil,{
			^this.guiClass.new(this).performList(\smallGui,args);
		},{
			^Tile(this,args.first.asPageLayout)
		});
	}
	
	insp { arg  ... args;
		Insp(this,args);
	}
	debug { arg caller;
		// by using this rather than just postln
		// you can command-shift-y on debug and find every one you left
		if(caller.notNil,{ 
			(caller.asString + ":" + this).postln;
		},{
			this.postln;	
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
			CXLabel(layout.startRow,k,maxx: 100);
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

+ Nil {

	asPageLayout { arg name,bounds,metal=true;
		^MultiPageLayout(name.asString,bounds, metal: metal ).front
	}
	asFlowView { arg bounds;
		^FlowView(nil,bounds)
	}
	remove {}
}

+ SCWindow {
	asPageLayout { arg title,bounds;
		^MultiPageLayout.on(this.asView,bounds)
	}
}	
+ SCContainerView {
	asPageLayout { arg title,bounds;
		// though it won't go multi page
		// FlowView better ?
		^MultiPageLayout.on(this,bounds)
	}
}
+ SCViewAdapter {
	asPageLayout { arg title,bounds;
		^MultiPageLayout.on(this,bounds)
	}
}	
+ SCCompositeView {
	asFlowView { arg bounds;
		^FlowView(this,bounds ?? {this.bounds})
	}
}
+ SCLayoutView {
	asFlowView {}
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

