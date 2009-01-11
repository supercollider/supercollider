/*
(
w = SCWindow.new.front;
a = SCTextField2(w, Rect(10, 10, 100, 20));
b = SCTextField2(w, Rect(10, 100, 250, 40));
c = SCTextField(w, Rect(10, 200, 250, 20)).string_("old-style text box");
a.string = "hi there";
a.action = {arg field; field.value.postln; };
)

a.value = "yo";
a.string = "oy";
a.valueAction = "yoooo";

a.boxColor_(Color.yellow)

a.align_(\left)
a.align_(\center)
a.align_(\right)

b.align_(\left)
b.align_(\center)
b.align_(\right)

a.editable_(false);
a.editable_(true);

// Drag-and-drop NOWORKY:
a.defaultGetDrag
a.defaultCanReceiveDrag
a.defaultReceiveDrag

a.font
a.font_(Font(Font.defaultSerifFace))
a.font_(Font(Font.defaultSansFace), 8)
a.font_(nil)

a.stringColor
a.stringColor_(Color.red)
a.stringColor
*/
SCTextField2 : SCStaticTextBase {
	
	*paletteExample { arg parent, bounds;
		^this.new(parent, bounds).initBackGround.value_("edit me");
	}
	
	initBackGround {
		super.init;
		background = Color.white;
		
	}
	
	value {
		^this.string
	}
	string{
		^this.getProperty(\string);
	}
	
	value_{|str|
		^this.string_(str);
	}
	valueAction_{|str|
		^this.string_(str).doAction;
	}
	boxColor {
		this.deprecated(thisMethod, SCView.findMethod(\background));
		^this.background;
	}
	boxColor_ { arg color;
		this.deprecated(thisMethod, SCView.findMethod(\background_));
		this.background_(color)
	}
	properties {
		^super.properties ++ #[\boxColor]
	}
	
	// Dragging doesn't seem to be working here
	defaultGetDrag { 
		//"defaultGetDrag was called".postln;
		^this.string
	}
	defaultCanReceiveDrag { 
		//"defaultCanReceiveDrag was called".postln;
		^currentDrag.respondsTo(\asString)
	}
	defaultReceiveDrag {
		//"defaultReceiveDrag was called".postln;
		this.valueAction = currentDrag;
	}

	// private:
	// prClose removes action here, to prevent action acting on textview after it's gone
	prClose { dataptr = nil; action = nil; onClose.value(this); }

}
