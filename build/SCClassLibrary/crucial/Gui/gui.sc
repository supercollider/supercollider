
+ Object {

	guiClass { ^ObjectGui }

	gui { arg  ... args; 
		^this.guiClass.new(this).performList(\gui,args);
	}
	
	topGui { arg ... args; 
		^this.guiClass.new(this).performList(\topGui,args);
	}
	
	smallGui { arg  ... args;
		^this.guiClass.new(this).performList(\smallGui,args);
	}
	
	insp { arg  ... args;
		Insp(this,args);
	}
	debug { arg caller;
		// by using this rather than just postln
		// you can command-shift-y on debug and find every one you left
		this.postln;
		if(caller.notNil,{ (caller.asString + caller.identityHash).postln; "".postln; });
	}
	
}

+ Class {
	guiClass { ^ClassGui }
}

+ Method {
	guiClass { ^MethodGui }
}

+ Dictionary {
	guiClass { ^DictionaryGui }		
}

+ Server {
	guiClass { ^ServerGui }
}
+ Node {
	guiClass { ^NodeGui }
}
+ Synth {
	guiClass { ^SynthGui }
}

+ Nil {

	asPageLayout { arg name,bounds,metal=false;
		^MultiPageLayout(name.asString,bounds, metal: metal )
	}
	asFlowView { arg bounds;
		^FlowView(nil,bounds)
	}
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
	guiClass { ^PswitchGui }
}
