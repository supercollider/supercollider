

ClassNameLabel : ActionButton {

	*new { arg  class,layout,maxx=200;
		^super.new(layout,class.name.asString,{ClassGui(class)},maxx,13,2)
			.backColor_(Color( 0.52156862745098, 0.75686274509804, 0.90196078431373 ));
	}

}


MethodLabel : ActionButton {

	*new { arg  method,layout,maxx=100;
		^super.new(layout,method.ownerClass.name.asString ++ "::" ++ method.name.asString,
			{MethodGui(method)},maxx,13,2)
				.backColor_(Color( 0.62745098039216, 0.8078431372549, 0.57647058823529 ));
	}
	*withoutClass { arg  method,layout;
		^super.new(layout, "::" ++ method.name.asString,{MethodGui(method)},100,13,2)
			.backColor_(Color( 0.62745098039216, 0.8078431372549, 0.57647058823529 ));
	}
	
//	// search by selector link
//	*browseSelectorLink { arg method,layout;
//		^super.new(layout, "::" ++ method.name.asString,{SelectorBrowser(method.name).gui},100,13,2).backColor_(Color(160,206,147));
//	}

}


SelectorLabel : ActionButton {
	
	*new { arg  selector,layout,maxx=100; // can send it to a MethodBrowser
		^super.new(layout,selector.asString,{},maxx,13,2)
			.backColor_(Color( 0.74509803921569, 0.8078431372549, 0.57647058823529 ));
	}
}

//aka GuiLink
Tile : ActionButton {

	*new { arg  target,layout,maxx=100; 
		^super.new(layout,target.asString,{ 
				Sheet({ arg f; 
					target.gui(f); 
				},target.asString)
			},maxx,13,3)
			.backColor_(Color( 0.90588235294118, 0.98039215686275, 0.69803921568627 ))
			.labelColor_(Color.blue)
	}

}

InspectorLink : ActionButton {
	*new { arg  target,layout,maxx=100; 
		^super.new(layout,target.asString,{target.i},maxx)
			.backColor_(Color( 0.5921568627451, 0.56862745098039, 1 ))
			.labelColor_(Color.blue)
	}
}

VariableNameLabel : ActionButton { // also used for arg name
	*new { arg name,layout,maxx=100;
		^super.new(layout,name,maxx: maxx)
			.backColor_(Color( 1, 0.86666666666667, 0.38039215686275 ));
	}
}

ArgNameLabel : ActionButton {
	*new { arg name,layout,maxx=100;
		^super.new(layout,name,maxx: maxx)
			.backColor_(Color( 0.47843137254902, 0.72941176470588, 0.50196078431373 ));
	}
}
				

