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
SCTextField : SCStaticTextBase {

	*paletteExample { arg parent, bounds;
//		^this.new(parent, bounds).initBackGround.value_("edit me");
		^this.new(parent, bounds).value_("edit me");

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

SCNumberBox : SCTextField {

	//var <> keyString,
	var <>step=1, <>scroll_step=1;
	var <>typingColor, <>normalColor;
	var <>clipLo = -inf, <>clipHi = inf, hit, inc=1.0, <>scroll=true;
	var <>shift_scale = 100.0, <>ctrl_scale = 10.0, <>alt_scale = 0.1;
	//var mouseIncOrDeced = true;

	getScale { |modifiers|
		^case
			{ modifiers & 131072 == 131072 } { shift_scale }
			{ modifiers & 262144 == 262144 } { ctrl_scale }
			{ modifiers & 524288 == 524288 } { alt_scale }
			{ 1 };
	}

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.value = 123.456;
		^v
	}
//
//	init { arg argParent, argBounds;
//		typingColor = Color.red;
//		normalColor = Color.black;
//		background = Color.white;
//		parent = argParent.asView; // actual view
//		this.prInit(parent.asView, argBounds.asRect,this.class.viewClass);
//		argParent.add(this);//maybe window or viewadapter
//	}

//	clipLo_ {|val = -inf|
//		clipLo = val;
//		this.setProperty(\clipLo, val);
//	}
//
//	clipHi_ {|val = inf|
//		clipHi = val;
//		this.setProperty(\clipHi, val);
//	}

	increment {arg mul=1; this.valueAction = this.value + (step*mul); }
	decrement {arg mul=1; this.valueAction = this.value - (step*mul); }

	defaultKeyDownAction { arg char, modifiers, unicode;
		var zoom = this.getScale(modifiers);

		// standard chardown
		if (unicode == 16rF700, { this.increment(zoom); ^this });
		if (unicode == 16rF703, { this.increment(zoom); ^this });
		if (unicode == 16rF701, { this.decrement(zoom); ^this });
		if (unicode == 16rF702, { this.decrement(zoom); ^this });

//		if ((char == 3.asAscii) || (char == $\r) || (char == $\n), { // enter key
//			if (keyString.notNil,{ // no error on repeated enter
//				this.valueAction_(keyString.asFloat);
//			});
//			^this
//		});
//		if (char == 127.asAscii, { // delete key
//			keyString = nil;
//			this.string = object.asString;
//			this.stringColor = normalColor;
//			^this
//		});
//		if (char.isDecDigit || "+-.eE".includes(char), {
//			if (keyString.isNil, {
//				keyString = String.new;
//				this.stringColor = typingColor;
//			});
//			keyString = keyString.add(char);
//			this.string = keyString;
//			^this
//		});



		^nil		// bubble if it's an invalid key
	}

	//value { ^object }
//	value_ { arg val;
//		keyString = nil;
//		this.stringColor = normalColor;
//		object = val !? { val.clip(clipLo, clipHi) };
//		this.string = object.asString;
//	}
//	valueAction_ { arg val;
//		var prev;
//		prev = object;
//		this.value = val !? { val.clip(clipLo, clipHi) };
//		if (object != prev, { this.doAction });
//	}

//	boxColor {
//		this.deprecated(thisMethod, SCView.findMethod(\background));
//		^this.background;
//	}
//	boxColor_ { arg color;
//		this.deprecated(thisMethod, SCView.findMethod(\background_));
//		this.background_(color)
//	}

	properties {
		^super.properties ++ #[\boxColor]
	}
	defaultGetDrag {
		^this.value;
	}
	defaultCanReceiveDrag {
		^currentDrag.isNumber;
	}
	defaultReceiveDrag {
		this.valueAction = currentDrag;
	}

	mouseDown { arg x, y, modifiers, buttonNumber, clickCount;
		hit = Point(x,y);
		if (scroll == true, { inc = this.getScale(modifiers) });
		mouseDownAction.value(this, x, y, modifiers, buttonNumber, clickCount)
	}

	mouseMove { arg x, y, modifiers;
		var direction;
		if (scroll == true, {
			//mouseIncOrDeced = true;
			direction = 1.0;
				// horizontal or vertical scrolling:
			if ( (x - hit.x) < 0 or: { (y - hit.y) > 0 }) { direction = -1.0; };

			this.valueAction = (this.value + (inc * this.scroll_step * direction));
			hit = Point(x, y);
		});
		mouseMoveAction.value(this, x, y, modifiers);
	}
	mouseUp{

		//if(mouseIncOrDeced, {this.value = this.value;mouseIncOrDeced = false});
		inc=1
	}


	value {
		^this.getProperty(\value);
	}

	value_ { arg val;
		val = val !? { val.clip(clipLo, clipHi) };
		this.setProperty(\value, val);
	}

	valueAction_ { arg val;
		var prev;
		prev = this.value;
		val = val !? { val.clip(clipLo, clipHi) };
		if (val != prev, { this.value_(val); this.doAction });
	}

	// validate range here
	doAction {
		var current;
		current = this.value;
		if(current.inclusivelyBetween(clipLo, clipHi).not, {
			this.value = current; // clips here
		});
		action.value(this);
	}
}
