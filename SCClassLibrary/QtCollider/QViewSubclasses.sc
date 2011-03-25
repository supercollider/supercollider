
/////////////////////// ABSTRACT CLASSES ////////////////////////////

QTextViewBase : QView {
  var <align;

  align_ { arg aSymbol;
    align = aSymbol;
    this.setProperty( \alignment, QAlignment(aSymbol) );
  }
}

QItemViewBase : QView
{
  var <items;

  items_ { arg stringArray;
    items = stringArray;
    this.setProperty( \items, stringArray);
  }

  item {
    ^items.at( this.value );
  }

  valueAction_ { arg val;
    this.value_(val);
    action.value(this);
  }
}

QAbstractScroll : QView {
  var <hasHorizontalScroller = true, <hasVerticalScroller = true;
  var <autohidesScrollers = true;

  autohidesScrollers_ { arg aBool;
    if( aBool ) {
      if( hasHorizontalScroller ) {
        this.setProperty( \horizontalScrollBarPolicy, 0 );
      };
      if( hasVerticalScroller ) {
        this.setProperty( \verticalScrollBarPolicy, 0 );
      };
    } {
      if( hasHorizontalScroller ) {
        this.setProperty( \horizontalScrollBarPolicy, 2 )
      };
      if( hasVerticalScroller ) {
        this.setProperty( \verticalScrollBarPolicy, 2 );
      };
    };
    autohidesScrollers = aBool;
  }

  hasHorizontalScroller_ { arg aBool;
    var policy;
    if( aBool ) {
      if( autohidesScrollers ) { policy = 0 } { policy = 2 };
      this.setProperty( \horizontalScrollBarPolicy, policy );
    } {
      this.setProperty( \horizontalScrollBarPolicy, 1 );
    };
    hasHorizontalScroller = aBool;
  }

  hasVerticalScroller_ { arg aBool;
    var policy;
    if( aBool ) {
      if( autohidesScrollers ) { policy = 0 } { policy = 2 };
      this.setProperty( \verticalScrollBarPolicy, policy );
    } {
      this.setProperty( \verticalScrollBarPolicy, 1 );
    };
    hasVerticalScroller = aBool;
  }
}

QAbstractStepValue : QView {
  var <step, <shift_scale, <ctrl_scale, <alt_scale;

  step_ { arg aFloat;
    step = aFloat;
    this.setProperty( \step, aFloat );
  }

  value {
    ^this.getProperty( \value );
  }

  value_ { arg argVal;
    this.setProperty( \value, argVal );
  }

  valueAction_ { arg val;
    this.value_(val);
    action.value(this);
  }

  shift_scale_ { arg aFloat;
    shift_scale = aFloat;
    this.setProperty( \shiftScale, aFloat );
  }

  ctrl_scale_ { arg aFloat;
    ctrl_scale = aFloat;
    this.setProperty( \ctrlScale, aFloat );
  }

  alt_scale_ { arg aFloat;
    alt_scale = aFloat;
    this.setProperty( \altScale, aFloat );
  }

  increment {
    this.nonimpl( \increment );
  }

  decrement {
    this.nonimpl( \decrement );
  }
}

/////////////////////// CONTAINERS ////////////////////////////////

QHLayoutView : QView {
  *qtClass { ^"QcHLayoutWidget" }
}

QVLayoutView : QView {
  *qtClass { ^"QcVLayoutWidget" }
}

QScrollView : QAbstractScroll {
  var <background, <hasBorder=true;

  *qtClass { ^"QcScrollArea" }

  background_ { arg aColor;
    background = aColor;
    this.setProperty( \background, aColor, true );
  }

  hasBorder_ { arg aBool;
    hasBorder = aBool;
    this.setProperty( \hasBorder, aBool );
  }

  innerBounds {
    ^this.getProperty( \innerBounds, Rect.new );
  }

  visibleOrigin {
    ^this.getProperty( \visibleOrigin, Point.new );
  }

  visibleOrigin_ { arg point;
    this.setProperty( \visibleOrigin, point );
  }
}

/////////////////////////// WIDGETS ///////////////////////////////

QStaticText : QTextViewBase {
  var <string;

  *qtClass { ^"QLabel" }

  *new { arg aParent, aBounds;
    var obj = super.new( aParent, aBounds );
    obj.setProperty(\wordWrap, true);
    ^obj;
  }

  background_ { arg aColor;
    if( this.background.isNil ) {
      this.setProperty( \autoFillBackground, true);
    };
    super.background_( aColor );
  }

  string_ { arg text;
    string = text;
    this.setProperty( \text, text );
  }

  stringColor {
    ^this.palette.windowTextColor;
  }

  stringColor_ { arg color;
    this.setProperty( \palette, this.palette.windowTextColor_(color) );
  }
}

QTextField : QTextViewBase {
  *qtClass { ^"QcTextField" }

  string {
    ^this.getProperty( \text );
  }

  string_ { arg text;
    this.setProperty( \text, text );
  }

  stringColor {
    ^this.palette.baseTextColor;
  }

  stringColor_ { arg color;
    this.setProperty( \palette, this.palette.baseTextColor_(color) );
  }

  background {
    ^this.palette.baseColor;
  }

  background_ { arg color;
    this.setProperty( \palette, this.palette.baseColor_(color) )
  }

  value {
    ^this.string;
  }

  value_ { arg val;
    this.string_( val.asString );
  }

  valueAction_ { arg val;
    this.string_( val.asString );
    this.doAction;
  }
}

QButton : QView {
  var <states;

  *qtClass { ^"QcButton" }

  *properties {
    ^[\string, \states];
  }

  value {
    ^this.getProperty( \value );
  }

  value_ { arg argVal;
    this.setProperty( \value, argVal );
  }

  valueAction_ { arg anInt;
    this.value_( anInt );
    action.value(this);
  }

  states_ { arg stateArray;
    states = stateArray;
    super.setProperty( \states, stateArray );
  }
}

QCheckBox : QView {

  *qtClass { ^"QcCheckBox" }

	*new{ |parent,bounds,text|
		^super.new(parent,bounds).init(text)
	}

	init{ |text|
		this.string_(text)
	}

	value{
		^this.getProperty(\value)
	}

	value_{ |val|
		this.setProperty(\value,val)
	}

	string_{ |string|
		this.setProperty(\text,string)
	}

	string{
		^this.getProperty(\text)
	}
}



QSlider : QAbstractStepValue {
  //compatibility stuff:
  var <orientation;
  var <> thumbSize;

  *qtClass { ^"QcSlider" }

  *new { arg parent, bounds;
    ^super.new( parent, bounds ).initQSlider( bounds );
  }

  knobColor {
    ^this.palette.buttonColor;
  }

  knobColor_ { arg color;
    this.setProperty( \palette, this.palette.buttonColor_(color) );
  }

  initQSlider { arg bounds;
    var r;
    if( bounds.notNil ) {
      r = bounds.asRect;
      if( r.width > r.height ) {
        this.orientation_( \horizontal );
      } {
        this.orientation_( \vertical );
      }
    }
  }

  pixelStep {
    ^0;
  }

  orientation_ { arg aSymbol;
    orientation = aSymbol;
    this.setProperty( \orientation, QOrientation(aSymbol) );
  }
}

QPopUpMenu : QItemViewBase {

  *qtClass { ^"QcPopUpMenu" }

  value {
    var v = this.getProperty( \currentIndex );
    if( v < 0 ) { ^nil } { ^v };
  }

  value_ { arg val;
    this.setProperty( \currentIndex, val ? -1 );
  }

  stringColor {
    ^this.palette.buttonTextColor;
  }

  stringColor_ { arg color;
    this.setProperty( \palette, this.palette.buttonTextColor_(color) );
  }
}
