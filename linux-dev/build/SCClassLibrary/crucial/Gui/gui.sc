
+ Object {

	guiClass { ^ObjectGui }

	gui { arg lay ... args; 
		^this.guiClass.new(this).performList(\gui,[lay] ++ args);
	}
	
	topGui { arg lay; 
		^this.guiClass.new(this).topGui(lay);
	}
	
	smallGui { arg lay;
		^this.guiClass.new(this).smallGui(lay);
	}
	
	insp { arg  ... args;
		Insp(this,args);
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

+ Nil {

	asPageLayout { arg name,width,height,x,y,metal=false;
		^PageLayout(name.asString,width,height,x, y, metal: metal )
	}
	asFlowView { arg bounds;
		^FlowView(nil,bounds)
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
	asPageLayout {
	
	} // should be compatible ?
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
