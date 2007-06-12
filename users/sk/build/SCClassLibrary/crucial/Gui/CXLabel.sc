
CXAbstractLabel : SCStaticText {

	*new { arg layout,string,x,y=17,minWidth=15;
		string = string.asString;
		^super.new(layout,	Rect(0,0,x ?? {(string.size * 7.5).max(minWidth)} ,y))
			.string_(string)
	}
	*viewClass { ^SCStaticText }
	label_ { arg string;
		this.string_(string);
	}
	bold { 
		this.font = Font("Helvetica-Bold",12.0);
	}
}

CXLabel : CXAbstractLabel {
	classvar <>bgcolor;

	*new { arg layout,string,x,y=17,minWidth=15;
		var new;
		new = super.new(layout,string,x,y,minWidth);
		new.font_(Font("Helvetica",12.0))
			.background_(Color.new255(250,250,240))
			.align_(\left);
		^new
	}
}

VariableNameLabel : CXAbstractLabel {
	*new { arg name,layout,minWidth=120;
		^super.new(layout,name,minWidth: minWidth)
			.background_(Color( 1, 0.86666666666667, 0.38039215686275 ))
			.font_(Font("Helvetica",10))
			.align_(\right)
	}
}

ArgNameLabel : CXAbstractLabel {
	*new { arg name,layout,minWidth=130;
		^super.new(layout," " ++ name,minWidth: minWidth)
			.background_(Color( 0.47843137254902, 0.72941176470588, 0.50196078431373 ))
			.font_(Font("Helvetica",10))
			.align_(\left)
	}
}
