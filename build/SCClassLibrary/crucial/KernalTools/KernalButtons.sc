

ClassNameLabel : ActionButton {

	*new { arg  class,layout,maxx=200;
		^super.prNew(layout,class.name.asString,{class.gui},maxx)
			.backColor_(Color( 0.52156862745098, 0.75686274509804, 0.90196078431373 ));
	}
	*newBig { arg  class,layout,maxx=200;
		^super.prNew(layout,class.name.asString,{class.gui},maxx,30)
			.backColor_(Color( 0.52156862745098, 0.75686274509804, 0.90196078431373 ))
			.font_(Font("Helvetica-Bold",18))
	}

}


MethodLabel : ActionButton {

	*new { arg  method,layout,maxx=100;
		^super.prNew(layout,method.ownerClass.name.asString ++ "-" ++ method.name.asString,
			{method.gui},maxx,15)
				.backColor_(Color.new255(245, 222, 179));
	}
	*withoutClass { arg  method,layout,maxx=100;
		^super.prNew(layout, "-" ++ method.name.asString,{method.gui},maxx,15)
			.backColor_(Color.new255(245, 222, 179));
	}
	
//	// search by selector link
//	*browseSelectorLink { arg method,layout;
//		^super.prNew(layout, "::" ++ method.name.asString,{SelectorBrowser(method.name).gui},100,13,2).backColor_(Color(160,206,147));
//	}

}


SelectorLabel : ActionButton {
	
	*new { arg  selector,layout,maxx=100; // can send it to a MethodBrowser
		^super.prNew(layout,selector.asString,{},maxx)
			.backColor_(Color( 0.74509803921569, 0.8078431372549, 0.57647058823529 ));
	}
}

//aka GuiLink
Tile : ActionButton {

	*new { arg  target,layout,maxx=100; 
		^super.prNew(layout,target.asString,{ 
				Sheet({ arg f; 
					target.gui(f); 
				},target.asString)
			},maxx,15)
			.backColor_(Color.new255(0, 205, 187))
			.labelColor_(Color.black)
	}

}

InspectorLink : ActionButton {
	*new { arg  target,layout,maxx=100; 
		^super.prNew(layout,target.asString,{target.insp},maxx)
			.backColor_(Color.white)
			.labelColor_(Color.new255(70, 130, 180))
			.font_(Font("Helvetica",12))
			//.align_(\left)
	}
	*big { arg  target,layout,maxx=100; 
		^super.prNew(layout,target.asString,{target.insp},maxx,30)
			.backColor_(Color.white)
			.labelColor_(Color.black)
			.font_(Font("Helvetica-Bold",18))
			//.align_(\left)
	}
}


				

