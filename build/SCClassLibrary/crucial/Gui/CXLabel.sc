
CXAbstractLabel : SCViewHolder {

	*new { arg layout,string,width,height,minWidth=15,font;
		string = string.asString;
		if(font.isNil,{ font =  GUI.font.new(*GUI.skin.fontSpecs) });

		^super.new.init(layout, Rect(0,0,
			width ?? {(string.bounds(font).width + 6).max(minWidth)} ,
			height ?? {GUI.skin.buttonHeight}))
			.font_(font)
			.label_(string)
	}
	init { |layout, bounds, string|
		view = this.class.viewClass.new(layout, bounds);
	}
	*viewClass { ^GUI.staticText }
	label_ { arg string;
		view.string_(" " ++ string ++ " ");
	}
	bold {
		this.font = GUI.font.new("Helvetica-Bold",11.0);
	}
}

CXLabel : CXAbstractLabel {

	classvar <>bgcolor;

	*new { arg layout,string,width,height,minWidth=15,font;
		var new;
		new = super.new(layout,string,width,height,minWidth,font);
		new.background_(Color(0.9843137254902, 0.9843137254902, 0.9843137254902, 1.0))
			.align_(\left);
		^new
	}
}


VariableNameLabel : CXAbstractLabel {
	*new { arg name,layout,minWidth=120;
		^super.new(layout,name,minWidth: minWidth)
			.background_(Color( 1, 0.86666666666667, 0.38039215686275 ))
			.align_(\right)
	}
}

ArgNameLabel : CXAbstractLabel {
	*new { arg name,layout,minWidth=130;
		^super.new(layout,name,minWidth: minWidth)
			.background_(Color( 0.47843137254902, 0.72941176470588, 0.50196078431373 ))
			.align_(\left)
	}
}
