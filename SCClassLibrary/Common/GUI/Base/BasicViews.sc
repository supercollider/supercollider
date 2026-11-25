/////////////////////// ABSTRACT CLASSES ////////////////////////////

TextViewBase : View {
	var <object, <>setBoth = true;
	var <align;

	object_  { |obj|
		if(setBoth) { this.string = obj.asString };
		object = obj
	}

	align_ { |aSymbol|
		align = aSymbol;
		this.setProperty(\alignment, QAlignment(aSymbol))
	}
}

ItemViewBase : View {
	var <items;

	items_ { |stringArray|
		items = stringArray;
		this.setProperty(\items, stringArray)
	}

	item {
		^items.at(this.value)
	}

	valueAction_ { |val|
		this.value_(val);
		action.value(this)
	}

	clear {
		this.invokeMethod(\clear)
	}
}

AbstractScroll : View {
	var <hasHorizontalScroller = true, <hasVerticalScroller = true;
	var <autohidesScrollers = true;

	autohidesScrollers_ { |aBool|
		if(aBool) {
			if(hasHorizontalScroller) {
				this.setProperty(\horizontalScrollBarPolicy, 0)
			};
			if(hasVerticalScroller) {
				this.setProperty(\verticalScrollBarPolicy, 0)
			}
		} {
			if(hasHorizontalScroller) {
				this.setProperty(\horizontalScrollBarPolicy, 2)
			};
			if(hasVerticalScroller) {
				this.setProperty(\verticalScrollBarPolicy, 2)
			}
		};
		autohidesScrollers = aBool
	}

	hasHorizontalScroller_ { |aBool|
		var policy;
		if(aBool) {
			if(autohidesScrollers) { policy = 0 } { policy = 2 };
			this.setProperty(\horizontalScrollBarPolicy, policy)
		} {
			this.setProperty(\horizontalScrollBarPolicy, 1)
		};
		hasHorizontalScroller = aBool
	}

	hasVerticalScroller_ { |aBool|
		var policy;
		if(aBool) {
			if(autohidesScrollers) { policy = 0 } { policy = 2 };
			this.setProperty(\verticalScrollBarPolicy, policy)
		} {
			this.setProperty(\verticalScrollBarPolicy, 1)
		};
		hasVerticalScroller = aBool
	}
}

AbstractStepValue : View {
	var <step = 0.1, <shift_scale = 100.0, <ctrl_scale = 10.0, <alt_scale = 0.1;

	step_ { |aFloat|
		step = aFloat;
		this.setProperty(\step, aFloat)
	}

	shift_scale_ { |aFloat|
		shift_scale = aFloat;
		this.setProperty(\shiftScale, aFloat)
	}

	ctrl_scale_ { |aFloat|
		ctrl_scale = aFloat;
		this.setProperty(\ctrlScale, aFloat)
	}

	alt_scale_ { |aFloat|
		alt_scale = aFloat;
		this.setProperty(\altScale, aFloat)
	}

	getScale { |modifiers|
		^case
			{ modifiers.isShift } { shift_scale }
			{ modifiers.isCtrl } { ctrl_scale }
			{ modifiers.isAlt } { alt_scale }
			{ 1 }
	}

	increment { |factor = 1.0| this.invokeMethod(\increment, factor.asFloat) }
	decrement { |factor = 1.0| this.invokeMethod(\decrement, factor.asFloat) }
}

/////////////////////// CONTAINERS ////////////////////////////////

HLayoutView : View {
	*qtClass { ^'QcHLayoutWidget' }
}

VLayoutView : View {
	*qtClass { ^'QcVLayoutWidget' }
}

ScrollCanvas : QObject {
	*qtClass { ^'QcScrollWidget' }

	background { ^this.getProperty(\background) }
	background_ { |color| this.setProperty(\background, color) }
}

ScrollView : AbstractScroll {
	var <canvas;
	var <hasBorder = true;
	var actionConnected = false;

	*new { |parent, bounds|
		^super.new(parent, bounds).initScrollView
	}

	*qtClass { ^'QcScrollArea' }

	children { |class = (View)|
		^canvas.children(class)
	}

	background { ^canvas.background }
	background_ { |color| canvas.background = color }

	hasBorder_ { |aBool|
		hasBorder = aBool;
		this.setProperty(\hasBorder, aBool)
	}

	innerBounds {
		^this.getProperty(\innerBounds)
	}

	visibleOrigin {
		^this.getProperty(\visibleOrigin)
	}

	visibleOrigin_ { |point|
		this.setProperty(\visibleOrigin, point);
		this.doAction
	}

	canvas_ { |view|
		if(view.notNil and: { view.parent == this }) {
			Error("Can't set canvas to a view, whose parent is this ScrollView.").throw // when that's the case, the interpreter crashes
		} {
			canvas = view;
			this.invokeMethod(\setWidget, view, true)
		}
	}

	action_ { |handler|
		action = handler;
		if(actionConnected.not) { this.connectMethod('scrolled()', \doAction) };
		actionConnected = true
	}

	initScrollView {
		// NOTE: The canvas widget may not be a View, so that asking its
		// children for parent will skip it and hit this view instead.
		this.canvas = ScrollCanvas()
	}
}

/////////////////////////// WIDGETS ///////////////////////////////

StaticText : TextViewBase {
	*qtClass { ^'QLabel' }

	*new { |parent, bounds|
		var obj = super.new(parent, bounds);
		obj.setProperty(\wordWrap, true);
		^obj
	}

	background {
		var p = this.palette;
		^if(p.hasColor(\window)) { p.window } { nil }
	}

	background_ { |color|
		var p = this.palette;
		if(p.hasColor(\window).not)
			{ this.setProperty(\autoFillBackground, true) };
		this.palette = p.window_(color)
	}

	string { ^this.getProperty(\text) }
	string_ { |text| this.setProperty(\text, text.asString) }

	stringColor {
		^this.palette.windowText
	}

	stringColor_ { |color|
		this.palette = this.palette.windowText_(color)
	}
}

TextField : TextViewBase {
	*qtClass { ^'QcTextField' }

	string {
		^this.getProperty(\text)
	}

	string_ { |text|
		this.setProperty(\text, text)
	}

	stringColor {
		^this.palette.baseText
	}

	stringColor_ { |color|
		this.palette = this.palette.baseText_(color)
	}

	background {
		^this.palette.base
	}

	background_ { |color|
		this.palette = this.palette.base_(color)
	}

	value {
		^this.string
	}

	value_ { |val|
		this.string_(val.asString)
	}

	valueAction_ { |val|
		this.string_(val.asString);
		this.doAction
	}

	defaultGetDrag { ^this.string }
	defaultCanReceiveDrag { ^true }
	defaultReceiveDrag {
		this.valueAction = View.currentDrag
	}
}

Button : View {
	var <states;

	*qtClass { ^'QcButton' }

	value {
		^this.getProperty(\value)
	}

	value_ { |argVal|
		this.setProperty(\value, argVal)
	}

	valueAction_ { |anInt|
		this.value_(anInt);
		action.value(this)
	}

	states_ { |stateArray|
		states = stateArray;
		super.setProperty(\states, stateArray)
	}

	string_ { |string|
		if(states.isNil) {
			this.states = [[string]]
		} {
			states[this.value][0] = string;
			this.states = states
		}
	}

	string {
		^states !? { states[this.value][0] }
	}

	action_ { |func|
		this.manageMethodConnection(action, func, 'action(int)', \prDoAction);
		action = func
	}

	doAction { |modifiers|
		action.value(this, modifiers)
	}

	defaultGetDrag { ^this.value }
	defaultCanReceiveDrag { ^true }
	defaultReceiveDrag {
		if(View.currentDrag.isNumber)
			{ this.valueAction = View.currentDrag }
			{ this.action = View.currentDrag }
	}

	menu { ^this.getProperty(\menu) }
	menu_{ |menu| ^this.setProperty(\menu, menu) }

	icon_{ |image| ^this.setProperty(\icon, image) }

	iconSize_{ |size| ^this.setProperty(\iconSize, size.asSize) }
	iconSize { ^this.getProperty(\iconSize) }

	prDoAction { |mods|
		this.doAction(QKeyModifiers.toCocoa(mods))
	}
}

CheckBox : View {

	*qtClass { ^'QcCheckBox' }

	*new{ |parent, bounds, text|
		^super.new(parent, bounds).init(text)
	}

	init{ |text|
		this.string_(text)
	}

	value{
		^this.getProperty(\value)
	}

	value_{ |val|
		this.setProperty(\value, val)
	}

	valueAction_ { |val|
		this.value_(val);
		this.doAction
	}

	string_{ |string|
		this.setProperty(\text, string)
	}

	string{
		^this.getProperty(\text)
	}

	defaultGetDrag { ^this.value }
	defaultCanReceiveDrag { ^((View.currentDrag == true) || (View.currentDrag == false)) }
	defaultReceiveDrag {
		this.valueAction = View.currentDrag
	}
}

PopUpMenu : ItemViewBase {

	*qtClass { ^'QcPopUpMenu' }

	allowsReselection { ^this.getProperty(\reactivationEnabled) }

	allowsReselection_ { |flag| ^this.setProperty(\reactivationEnabled, flag) }

	value {
		var v = this.getProperty(\currentIndex);
		if(v.isNil or: { v < 0 }) { ^nil } { ^v }
	}

	value_ { |val|
		this.setProperty(\currentIndex, val ? -1)
	}

	background { ^this.palette.button }

	background_ { |color| this.palette = this.palette.button_(color) }

	stringColor {
		^this.palette.buttonText
	}

	stringColor_ { |color|
		this.palette = this.palette.buttonText_(color)
	}

	defaultGetDrag { ^this.value }
	defaultCanReceiveDrag { ^View.currentDrag.isNumber }
	defaultReceiveDrag {
		this.valueAction = View.currentDrag
	}
}
