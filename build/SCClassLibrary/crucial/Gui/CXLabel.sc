
// temp translations

// FLabel

CXLabel {
	classvar <>bgcolor;
	var <>view;
	*new { arg layout,string,x,y=17,maxx=15;
		var new;
		new = this.prNew(layout,string,x,y,maxx);
		new.view
			.font_(Font("Helvetica",12.0))
			.background_(bgcolor)
			.align_(\center);
		^new
	}
	*prNew { arg layout,string,x,y=17,maxx=15;
		layout = layout.asPageLayout(string);
		^super.new.view_(
			SCStaticText(layout.window,
					layout.layRight(x ?? { (string.size * 8.3).max(maxx) },y)
			).string_(string)
		)
	}
	backColor_ { arg c; view.background_(c) }
	label_ { arg string;
		view.string_(string);
	}
	align_ { arg a=\left;
		view.align = a;
	}
	font_ { arg f;
		view.font = f;
	}
	bold { 
		view.font = Font("Helvetica-Bold",12.0);
	}
}


VariableNameLabel : CXLabel { // also used for arg name
	*new { arg name,layout,maxx=120;
		^super.prNew(layout,name,maxx: maxx)
			.backColor_(Color( 1, 0.86666666666667, 0.38039215686275 ))
			.font_(Font("Monaco",10))
			.align_(\right)
	}
}

ArgNameLabel : CXLabel {
	*new { arg name,layout,maxx=100;
		^super.prNew(layout,name,maxx: maxx)
			.backColor_(Color( 0.47843137254902, 0.72941176470588, 0.50196078431373 ))
			.font_(Font("Monaco",10))
			.align_(\right)
	}
}

