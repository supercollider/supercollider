
SCView {  // abstract class
	classvar <>currentDrag;
	classvar <>globalKeyDownAction, <>globalKeyUpAction;

	var dataptr, <parent, <>action, <background, <>keyDownAction, <>keyUpAction, <>keyTyped;
	
	*new { arg parent, bounds;
		^super.new.init(parent, bounds);
	}
	
	*paletteExample { arg parent, bounds;
		^this.new(parent, bounds);
	}
	
	init { arg argParent, argBounds;
		parent = argParent.asView;
		this.prInit(parent, argBounds);
		if (parent.notNil, { parent.add(this); });
		// init default keydown & keyup
		this.keyDownAction_({ arg view, key, modifiers, unicode;
			this.defaultKeyDownAction(key, modifiers, unicode);
			nil
		});
		this.keyUpAction_({ arg view, key, modifiers, unicode;
			this.defaultKeyUpAction(key, modifiers, unicode);
			nil
		});

	}
	
	asView { ^this }
	
	bounds {
		^this.getProperty(\bounds, Rect.new)
	}
	bounds_ { arg rect;
		this.setProperty(\bounds, rect)
	}
	
	visible {
		^this.getProperty(\visible)
	}
	visible_ { arg bool;
		this.setProperty(\visible, bool)
	}
	
	enabled {
		^this.getProperty(\enabled)
	}
	enabled_ { arg bool;
		this.setProperty(\enabled, bool)
	}
	
	canFocus {
		^this.getProperty(\canFocus)
	}
	canFocus_ { arg bool;
		this.setProperty(\canFocus, bool)
	}
	// sc.solar addition
	focus {
		this.prFocus
	}

	/*
	resize behaviour in an SCCompositeView:
		1  2  3
		4  5  6
		7  8  9
		
		1 - fixed to left, fixed to top
		2 - horizontally elastic, fixed to top
		3 - fixed to right, fixed to top
		
		4 - fixed to left, vertically elastic
		5 - horizontally elastic, vertically elastic
		6 - fixed to right, vertically elastic
		
		7 - fixed to left, fixed to bottom
		8 - horizontally elastic, fixed to bottom
		9 - fixed to right, fixed to bottom
	*/
	resize {
		^this.getProperty(\resize)
	}
	resize_ { arg resize;
		this.setProperty(\resize, resize)
	}
	
	background_ { arg color;
		background = color;
		this.setProperty(\background, color)
	}

	keyDown { arg key, modifiers, unicode;
		globalKeyDownAction.value(this, key, modifiers, unicode); 
		this.handleKeyDownBubbling(this, key, modifiers, unicode);
	}
	
	defaultKeyDownAction { arg key, modifiers, unicode;}
		
	handleKeyDownBubbling { arg view, key, modifiers, unicode;
		// nil from keyDownAction --> pass it on
		if (this.keyDownAction.value(view, key, modifiers, unicode).isNil, {
			// call local keydown action of parent view
			this.parent.handleKeyDownBubbling(view, key, modifiers, unicode);
		});
	}
	
	// sc.solar addition
	keyUp { arg key, modifiers, unicode; 
		this.keyTyped = key;
		// allways call global keydown action first
		globalKeyUpAction.value(this, key, modifiers, unicode);
		this.handleKeyUpBubbling(this, key, modifiers, unicode);
	}
	defaultKeyUpAction { arg key, modifiers, unicode; }
		
	handleKeyUpBubbling { arg view, key, modifiers;
		// nil from keyUpAction --> pass it on
		if (this.keyUpAction.value(view, key, modifiers).isNil, {
			// call local keyUp action of parent view
			this.parent.handleKeyUpBubbling(view, key, modifiers);
		});
	}

	canReceiveDrag {
		^false;
	}

	// get the view parent tree up to the SCTopView
	getParents {
		var parents, view;
		view = this;
		parents = List.newClear;
		while({view.parent.notNil},{view = view.parent; parents.add(view)});
		^parents
	}
	
	doAction {
		action.value(this);
	}
	
	properties {
		^#[\bounds, \visible, \enabled, \canFocus, \resize, \background]
	}
	getPropertyList {
	// has to return !
		^this.properties.collect({ arg name;
			[name, this.perform(name)]
		});
	}
	setPropertyList { arg list;
		list.do({ arg item;
			var name, value;
			#name, value = item;
			this.perform(name.asSetter, value);
		});
	}
	
	// private
	prInit { arg argParent, argBounds;
		_SCView_New
		^this.primitiveFailed
	}
	prClose { dataptr = nil; }
	setProperty { arg key, value;
		_SCView_SetProperty
		^this.primitiveFailed
	}
	getProperty { arg key, value;
		_SCView_GetProperty
		^this.primitiveFailed
	}
	prFocus {
		_SCView_Focus
		^this.primitiveFailed
	}
	
	setPropertyWithAction { arg symbol, obj;
		// setting some properties may need to have the action called.
		if (this.setProperty(symbol, obj), {
			// setProperty returns true if action needs to be called.
			this.doAction;
		});
	}
	
	*importDrag { 
		// this is called when an NSString is the drag object.
		// we compile it to an SCObject.
		currentDrag = currentDrag.interpret;
	}
}

SCContainerView : SCView { // abstract class
	var <children, <>decorator;
			
	add { arg child;
		children = children.add(child);
		if (decorator.notNil, { decorator.place(child); });
	}
	// private
	prClose {
		super.prClose;
		children.do({ arg item; item.prClose });
	}
}

SCCompositeView : SCContainerView {
}

SCTopView : SCCompositeView {
	// created by SCWindow
	handleKeyDownBubbling { arg view, key, modifiers, unicode;
		this.keyDownAction.value(view, key, modifiers, unicode);
	}
	handleKeyUpBubbling { arg view, key, modifiers, unicode;
		this.keyUpAction.value(view, key, modifiers, unicode);
	}
}

SCControlView : SCView { // abstract class
}

SCSliderBase : SCControlView {
	
	knobColor {
		^this.getProperty(\knobColor, Color.new)
	}
	knobColor_ { arg color;
		this.setProperty(\knobColor, color)
	}
	
	step_ { arg stepSize;
		this.setPropertyWithAction(\step, stepSize);
	}
	step {
		^this.getProperty(\step)
	}
	
	properties {
		^super.properties ++ [\knobColor, \step]
	}

}

SCSlider : SCSliderBase
{
	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setPropertyWithAction(\value, val);
	}	
	
	increment { ^this.value = this.value + this.bounds.width.reciprocal }
	decrement { ^this.value = this.value - this.bounds.width.reciprocal }
	
	defaultKeyDownAction { arg key, modifiers, unicode;
		// standard keydown
		if (key == $r, { this.value = 1.0.rand; });
		if (key == $n, { this.value = 0.0; });
		if (key == $x, { this.value = 1.0; });
		if (key == $c, { this.value = 0.5; });
		if (key == $], { this.increment; ^this });
		if (key == $[, { this.decrement; ^this });
		if (unicode == 16rF700, { this.increment; ^this });
		if (unicode == 16rF703, { this.increment; ^this });
		if (unicode == 16rF701, { this.decrement; ^this });
		if (unicode == 16rF702, { this.decrement; ^this });
	}
	
	beginDrag { 
		currentDrag = this.value; 
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.value = currentDrag;
		currentDrag = nil;
	}
}

SCKnob : SCSlider
{
}

SCRangeSlider : SCSliderBase {

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.lo = 0.2;
		v.hi = 0.7;
	}
	
	lo {
		^this.getProperty(\lo)
	}
	lo_ { arg val;
		this.setPropertyWithAction(\lo, val);
	}	
	hi {
		^this.getProperty(\hi)
	}
	hi_ { arg val;
		this.setPropertyWithAction(\hi, val);
	}	
	range {
		^this.getProperty(\range)
	}
	range_ { arg val;
		this.setPropertyWithAction(\range, val);
	}	

	properties {
		^super.properties ++ [\lo, \hi]
	}
	
	increment { 
		var inc, val; 
		inc = this.bounds.width.reciprocal;
		val = this.hi + inc;
		if (val > 1, {
			inc = 1 - this.hi;
			val = 1;
		});
		this.lo = this.lo + inc;
		this.hi = val;
	}
	decrement { 
		var inc, val; 
		inc = this.bounds.width.reciprocal;
		val = this.lo - inc;
		if (val < 0, {
			inc = this.lo;
			val = 0;
		});
		this.lo = val;
		this.hi = this.hi - inc;
	}

	defaultKeyDownAction { arg key, modifiers, unicode;
		var a, b;
		// standard keydown
		if (key == $r, { 
			a = 1.0.rand; 
			b = 1.0.rand; 
			this.lo = min(a, b);
			this.hi = max(a, b);
		});
		if (key == $n, { this.lo = 0.0; this.hi = 0.0; });
		if (key == $x, { this.lo = 1.0; this.hi = 1.0; });
		if (key == $c, { this.lo = 0.5; this.hi = 0.5; });
		if (key == $a, { this.lo = 0.0; this.hi = 1.0; });
		if (unicode == 16rF700, { this.increment; ^this });
		if (unicode == 16rF703, { this.increment; ^this });
		if (unicode == 16rF701, { this.decrement; ^this });
		if (unicode == 16rF702, { this.decrement; ^this });
	}
	beginDrag { 
		currentDrag = Point(this.lo, this.hi); 
	}
	canReceiveDrag {
		^currentDrag.isKindOf(Point);
	}
	receiveDrag {
	// changed to x,y instead of lo, hi
		this.lo = currentDrag.x;
		this.hi = currentDrag.y;
		currentDrag = nil;
	}
}

SC2DSlider : SCSliderBase {
	x {
		^this.getProperty(\x)
	}
	x_ { arg val;
		this.setPropertyWithAction(\x, val);
	}	
	y {
		^this.getProperty(\y)
	}
	y_ { arg val;
		this.setPropertyWithAction(\y, val);
	}	
	properties {
		^super.properties ++ [\x, \y]
	}

	incrementY { ^this.y = this.y + this.bounds.height.reciprocal }
	decrementY { ^this.y = this.y - this.bounds.height.reciprocal }
	incrementX { ^this.x = this.x + this.bounds.width.reciprocal }
	decrementX { ^this.x = this.x - this.bounds.width.reciprocal }

	defaultKeyDownAction { arg key, modifiers, unicode;
		// standard keydown
		if (key == $r, { this.x = 1.0.rand; this.y = 1.0.rand; });
		if (key == $n, { this.x = 0.0; this.y = 0.0; });
		if (key == $x, { this.x = 1.0; this.y = 1.0; });
		if (key == $c, { this.x = 0.5; this.y = 0.5; });
		if (unicode == 16rF700, { this.incrementY; ^this });
		if (unicode == 16rF703, { this.incrementX; ^this });
		if (unicode == 16rF701, { this.decrementY; ^this });
		if (unicode == 16rF702, { this.decrementX; ^this });
	}
	beginDrag { 
		currentDrag = Point(this.x, this.y); 
	}
	canReceiveDrag {
		^currentDrag.isKindOf(Point);
	}
	receiveDrag {
		this.x = currentDrag.x;
		this.y = currentDrag.y;
		currentDrag = nil;
	}
}


SCButton : SCControlView {
	var <font, <states;

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.states = [
			["Push", Color.black, Color.red],
			["Pop", Color.white, Color.blue]];
		^v

	}
	
	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setPropertyWithAction(\value, val);
	}	

	defaultKeyDownAction { arg key, modifiers, unicode;
		if (key == $ , { this.value = this.value + 1; ^this });
		if (key == $\r, { this.value = this.value + 1; ^this });
	}

	font_ { arg argFont;
		font = argFont;
		this.setProperty(\font, font)
	}

	states_ { arg array;
		states = array;
		this.setProperty(\states, states);
	}
	
	properties {
		^super.properties ++ [\value, \font, \states]
	}

	beginDrag { 
		currentDrag = this.value; 
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.value = currentDrag;
		currentDrag = nil;
	}
}


SCPopUpMenu : SCControlView {
	var <font, <items;
	
	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.items = ["linear","exponential","sine","welch","squared","cubed"];
		^v
	}
		
	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setPropertyWithAction(\value, val);
	}	

	defaultKeyDownAction { arg key, modifiers, unicode;
		if (key == $ , { this.value = this.value + 1; ^this });
		if (key == $\r, { this.value = this.value + 1; ^this });
		if (unicode == 16rF700, { this.value = this.value + 1; ^this });
		if (unicode == 16rF703, { this.value = this.value + 1; ^this });
		if (unicode == 16rF701, { this.value = this.value - 1; ^this });
		if (unicode == 16rF702, { this.value = this.value - 1; ^this });
	}
	font_ { arg argFont;
		font = argFont;
		this.setProperty(\font, font)
	}
	items_ { arg array;
		items = array;
		this.setProperty(\items, items);
	}
	stringColor {
		^this.getProperty(\stringColor, Color.new)
	}
	stringColor_ { arg color;
		this.setProperty(\stringColor, color)
	}
	
	properties {
		^super.properties ++ [\value, \font, \items, \stringColor]
	}

	beginDrag { 
		currentDrag = this.value; 
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.value = currentDrag;
		currentDrag = nil;
	}
}



SCStaticTextBase : SCView {
	var <string, <font, <object;
	
	font_ { arg argFont;
		font = argFont;
		this.setProperty(\font, font)
	}
	
	string_ { arg argString;
		string = argString.asString;
		this.setProperty(\string, string)
	}
	align_ { arg align;
		this.setProperty(\align, align)
	}
	
	stringColor {
		^this.getProperty(\stringColor, Color.new)
	}
	stringColor_ { arg color;
		this.setProperty(\stringColor, color)
	}

	object_ { arg obj;
		object = obj;
		this.string = object.asString(80);
	}
	
	properties {
		^super.properties ++ [\string, \font, \stringColor]
	}
}

SCStaticText : SCStaticTextBase {
	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.string = "The lazy brown fox";
		^v
	}
}

SCNumberBox : SCStaticTextBase {
	var <> keyString;
	
	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.value = 123.456;
		^v
	}

	defaultKeyDownAction { arg key, modifiers, unicode;
		// standard keydown
		if ((key == 3.asAscii) || (key == $\r) || (key == $\n), { // enter key
			if (keyString.notNil,{ // no error on repeated enter
				this.value = keyString.asFloat;
			});
			^this
		});
		if (key == 127.asAscii, { // delete key
			keyString = nil;
			this.string = object.asString;
			this.stringColor = Color.black;
			^this
		});
		if (key.isDecDigit || "+-.eE".includes(key), {
			if (keyString.isNil, { 
				keyString = String.new;
				this.stringColor = Color.red;
			});
			keyString = keyString.add(key);
			this.string = keyString;
		});
	}
	value { ^object }
	value_ { arg val;
		keyString = nil;
		this.stringColor = Color.black;
		object = val;
		this.string = val.asString;
		this.doAction;
	}
	properties {
		^super.properties ++ [\boxColor]
	}
	beginDrag { 
		currentDrag = object.asFloat; 
		
	}
	canReceiveDrag {
		^currentDrag.isNumber;
	}
	receiveDrag {
		this.value = currentDrag;
		
		currentDrag = nil;
	}
}

SCDragView : SCStaticTextBase {
	
	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds);
		v.object = \something;
		^v
	}
}

SCDragSource : SCDragView {
	var <>beginDragAction;
	
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			object, 
		{
			beginDragAction.value(this);
		});
	}
}

SCDragSink : SCDragView
{	
	var <>acceptDrag = true;
	
	canReceiveDrag {
		^acceptDrag.value(this);
	}
	receiveDrag {
		this.object = currentDrag;
		this.doAction;
		currentDrag = nil;
	}
}

SCDragBoth : SCDragSink {
	var <>beginDragAction;
	
	beginDrag { 
		currentDrag = if (beginDragAction.isNil, 
			object, 
		{
			beginDragAction.value(this);
		});
	}
}


SCUserView : SCView { // abstract class
	draw {}
	mouseBeginTrack { arg x, y, modifiers; }
	mouseTrack { arg x, y, modifiers;x.postln }
	mouseEndTrack { arg x, y, modifiers; }
}

SCFuncUserView : SCUserView {
	var <>keyDownFunc, <>drawFunc;
	var <>mouseBeginTrackFunc, <>mouseTrackFunc, <>mouseEndTrackFunc;
	
	draw { drawFunc.value(this) }
	mouseBeginTrack { arg x, y, modifiers; 
		mouseBeginTrackFunc.value(this, x, y, modifiers); 
	}
	mouseTrack { arg x, y, modifiers; 
		mouseTrackFunc.value(this, x, y, modifiers); 
	}
	mouseEndTrack { arg x, y, modifiers; 
		mouseEndTrackFunc.value(this, x, y, modifiers); 
	}
	keyDown { arg key, modifiers, unicode; 
		keyDownFunc.value(this, key, modifiers, unicode) 
	}
}

//by jt v.0.1
SCMultiSliderView : SCView { 

	var <>acceptDrag = true;
	var <> metaAction;
	var <> size ;
	var < gap;

	draw {}
	mouseBeginTrack { arg x, y, modifiers;
		}
	mouseTrack { arg x, y, modifiers; 	
 	}
	mouseEndTrack { arg x, y, modifiers;
	
	}
	properties {
		^super.properties ++ [\value, \thumbSize, \fillColor, \strokeColor, \xOffset, \x, \y, \showIndex, \drawLines, \drawRects, \selectionSize, \startIndex, \referenceValues, \thumbWidth, \absoluteX, \isFilled]
	}	
	value {//returns array
		^this.getProperty(\value, Array.newClear(this.size))
	}
	value_ {arg val;
		this.size = val.size;
		^this.setProperty(\value, val)
	}
	reference {//returns array
		^this.getProperty(\referenceValues, Array.newClear(this.size))
	}
	reference_ {arg val;
		//this.size = val.size;
		^this.setProperty(\referenceValues, val)
	}
	index { //returns selected index
		^this.getProperty(\x)
	}
	index_ {arg inx;
		this.setProperty(\x, inx)
	}

	gap_ {arg inx;
		gap = inx;
		this.setProperty(\xOffset, inx)
	}
	
	selectionSize { 
		^this.getProperty(\selectionSize)
	}
	currentvalue { //returns value of selected index
		^this.getProperty(\y)
	}

	currentvalue_ {arg iny;
		this.setProperty(\y, iny)
	}
	showIndex{arg abool;
		this.setProperty(\showIndex, abool)
		}
	drawLines{arg abool;
		this.setProperty(\drawLines, abool)
		}
	readOnly_{arg val;
		this.setProperty(\readOnly, val);
	}
	thumbSize_{arg val;
		this.setProperty(\thumbSize, val)
	}
	isHorizontal_{arg val;
		this.setProperty(\isHorizontal,val);	
	}
	canReceiveDrag {
		^acceptDrag.value(this);
	}
	receiveDrag {
		//this.object = currentDrag;
		if(currentDrag.at(0).isKindOf(Array), { 
		this.value_(currentDrag.at(0));
		this.reference_(currentDrag.at(1));
		},{
		this.value_(currentDrag);
		});
		this.doAction;
		currentDrag = nil;
	}
	beginDrag {
		var setsize, vals, rvals;
		rvals = this.reference;
		vals = this.value;
		if(this.selectionSize > 1, {
			vals = vals.copyRange(this.index, this.selectionSize + this.index);});
		if(rvals.isNil, { 
			currentDrag = vals 
		},{
		if(this.selectionSize > 1, {
			rvals = rvals.copyRange(this.index, this.selectionSize + this.index);});
			currentDrag = currentDrag.add(vals);
			currentDrag = currentDrag.add(rvals);
		});

		^currentDrag
	}
	defaultKeyDownAction { arg key, modifiers, unicode;
		//modifiers.postln; 16rF702
		if (unicode == 16rF703, { this.index = this.index + 1; ^this });
		if (unicode == 16rF702, { this.index = this.index - 1; ^this });
		if (unicode == 16rF700, { this.gap = this.gap + 1; ^this });
		if (unicode == 16rF701, { this.gap = this.gap - 1; ^this });

		}
	doMetaAction{ 
		var ax;
		if(this.metaAction.isNil,{
			ax = this.getProperty(\absoluteX);
			this.bounds = this.bounds.moveTo(ax,this.bounds.top );
		},{	this.metaAction.value(this);} );
	} //when ctrl click

	

}
SCEnvelopeView : SCMultiSliderView {
	xvalues {//returns array
		^this.getProperty(\xvalues, Array.newClear(this.size))
	}
	yvalues {//returns array
		^this.getProperty(\value, Array.newClear(this.size))
	}
	value_ {arg val;
		this.size = val.at(0).size;
		^this.setProperty(\value, val)
	}

}


Gradient {
	var color1, color2, direction, steps;
	*new { arg color1, color2, direction=\h, steps=64;
		^super.newCopyArgs(color1, color2, direction, steps)
	}
}

HiliteGradient {
	var color1, color2, direction, steps, frac;
	*new { arg color1, color2, direction=\v, steps=64, frac = 0.33;
		^super.newCopyArgs(color1, color2, direction, steps, frac)
	}
}
//including keydown by Heiko:
// bug fixed in SCRangeSlider-receiveDrag
// bug fixed in SCView-getPropertyList
// allowing children in SCContainerView and parent in SCView to be accessed
// globalKeyDownAction as classvar for all Views
// KeyDownAction for all Views
// bug fixed in SCNumberBox-defaultKeyDownAction
// passing keydown up the tree
// consuming keydown in function 
// added keyup
// and SCMultiSlider by jan t.
