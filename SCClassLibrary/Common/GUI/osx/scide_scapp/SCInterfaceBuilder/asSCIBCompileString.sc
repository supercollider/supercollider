//todo use streams
+ SCWindow{

	asSCIBCompileString {
		var str = "";
		view.children.do({ |v| if(v.isKindOf(SCConstructionView).not){
			 str = str ++ v.asSCIBCompileString ++";\n" }});
		^format( "(\nvar window = SCWindow.new(\"\",%).front;\n%\n)",this.bounds,str )
	}
}

+ SCIBPanelWindow{
	asSCIBCompileString {
		^window.asSCIBCompileString
	}

}

+ SCView{

	asSCIBCompileString {
		if(this.setActionForConstructionString){
			^format("%.new(window,%)%\n\t.action_{|v| }",this.class,this.bounds, this.modifiersForConstructionString);
		}{
			^format("%.new(window,%)",this.class,this.bounds);
		}
	}

	setActionForConstructionString{
		^true
	}
	modifiersForConstructionString{
		^""
	}

}

+SCButton{

	modifiersForConstructionString{
		^"\n\t.states_(%)".format(this.states.asCompileString)
	}

}

+SCStaticText{

	modifiersForConstructionString{
		^"\n\t.string_(%)".format(this.string.asCompileString)
	}

}
