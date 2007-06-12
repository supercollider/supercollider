

ClassNameLabel : ActionButton {

	*new { arg  class,layout,maxx=200;
		^super.new(layout,class.name.asString,{class.gui},maxx,nil,
			Color.white,
			Color( 0.52156862745098, 0.75686274509804, 0.90196078431373 )
		);
	}
	*newBig { arg  class,layout,maxx=200;
		^super.new(layout,class.name.asString,{class.gui},maxx,30, Color.white,
			Color( 0.52156862745098, 0.75686274509804, 0.90196078431373 ),
			Font("Helvetica-Bold",18))
	}

}


MethodLabel : ActionButton {

	*new { arg  method,layout,maxx=100;
		^super.new(layout,method.ownerClass.name.asString ++ "-" ++ method.name.asString,
			{method.gui},maxx,15,nil,Color.new255(245, 222, 179));
	}
	*withoutClass { arg  method,layout,maxx=100;
		^super.new(layout, "-" ++ method.name.asString,{method.gui},maxx,15,nil,
			Color.new255(245, 222, 179));
	}
	
//	// search by selector link
//	*browseSelectorLink { arg method,layout;
//		^super.prNew(layout, "::" ++ method.name.asString,{SelectorBrowser(method.name).gui},100,13,2).backColor_(Color(160,206,147));
//	}

}


SelectorLabel : ActionButton {
	
	*new { arg  selector,layout,maxx=100; // can send it to a MethodBrowser
		^super.new(layout,selector.asString,nil,maxx,nil,
			nil,Color( 0.74509803921569, 0.8078431372549, 0.57647058823529 ));
	}
}

//aka GuiLink
Tile : ActionButton {

	*new { arg  target,layout,maxx=100; 
		^super.new(layout,target.asString,{ 
				Sheet({ arg f; 
					target.gui(f); 
				},target.asString)
			},maxx,15, Color.black,Color.new255(152, 251, 152))
	}

}

InspectorLink : ActionButton {
	*new { arg  target,layout,maxx=100; 
		^super.new(layout,target.asString,{target.insp},maxx,nil,
			Color.new255(70, 130, 200),
			Color.white,
			Font("Helvetica",12)
		)
	}
	*big { arg  target,layout,maxx=200;
		^super.new(layout,target.asString,{target.insp},maxx,30,
			Color.black,Color.white,Font("Helvetica-Bold",18))
	}
}


				

