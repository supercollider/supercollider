
CXAbstractLabel : SCStaticText {

	*new { arg layout,string,x,y=17,maxx=15;
		^super.new(layout,	Rect(0,0,x ?? {(string.size * 7.0).max(maxx)} ,y))
			.string_(string)
	}
	*viewClass { ^SCStaticText }
	//deprec me
	backColor_ { arg c; this.background_(c) }

	label_ { arg string;
		this.string_(string);
		//view.string_(string);
	}
//	align_ { arg a=\left;
//		view.align = a;
//	}
//	font_ { arg f;
//		view.font = f;
//	}
	bold { 
		this.font = Font("Helvetica-Bold",12.0);
	}

}

CXLabel : CXAbstractLabel {
	classvar <>bgcolor;

//	*prNew { arg layout,string,x,y=17,maxx=15;
//		//layout = layout.asPageLayout;//View ?? {FlowView.new};
//		//layout.dump;
//		^super.new.view_(
//			SCStaticText(layout,
//					Rect(0,0,x ?? {(string.size * 7.0).max(maxx)} ,y)
//						//layout.layRight(x ?? { (string.size * 7.0).max(maxx) },y)
//			).string_(string)
//		)
//	}

	*new { arg layout,string,x,y=17,maxx=15;
		var new;
		new = super.new(layout,string,x,y,maxx);
		new.font_(Font("Helvetica",12.0))
			.background_(bgcolor)
			.align_(\center);
		^new
	}
}


VariableNameLabel : CXAbstractLabel {
	*new { arg name,layout,maxx=120;
		^super.new(layout,name,maxx: maxx)
			.backColor_(Color( 1, 0.86666666666667, 0.38039215686275 ))
			.font_(Font("Helvetica",10))
			.align_(\right)
	}
}

ArgNameLabel : CXAbstractLabel {
	*new { arg name,layout,maxx=130;
		^super.new(layout," " ++ name,maxx: maxx)
			.backColor_(Color( 0.47843137254902, 0.72941176470588, 0.50196078431373 ))
			.font_(Font("Helvetica",10))
			.align_(\left)
	}
}

