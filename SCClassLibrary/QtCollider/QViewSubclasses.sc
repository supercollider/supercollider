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

/////////////////////// CONTAINERS ////////////////////////////////

QHLayoutView : QView {
  *qtClass { ^"QcHLayoutWidget" }
}

QVLayoutView : QView {
  *qtClass { ^"QcVLayoutWidget" }
}

QScrollArea : QAbstractScroll {
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

QTextViewBase : QView {
  var <align;

  align_ { arg aSymbol;
    align = aSymbol;
    this.setProperty( \alignment, QAlignment(aSymbol) );
  }
}

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

QSlider2D : QAbstractStepValue {
  *qtClass { ^"QcSlider2D" }

  *new { arg parent, bounds;
    var me = super.new( parent, bounds );
    me.connectMethod( "randomize()", \randomize );
    ^me;
  }

  x {
    ^this.getProperty( \xValue );
  }

  x_ { arg aFloat;
    this.setProperty( \xValue, aFloat );
  }

  activex_ { arg aFloat;
    this.x_(aFloat);
    this.doAction;
  }

  y {
    ^this.getProperty( \yValue );
  }

  y_ { arg aFloat;
    this.setProperty( \yValue, aFloat );
  }

  activey_ { arg aFloat;
    this.y_(aFloat);
    this.doAction;
  }

  setXY { arg x, y;
    this.x_(x);
    this.y_(y);
  }

  setXYActive { arg x, y;
    this.setXY(x,y);
    this.doAction;
  }

  randomize {
    this.setXYActive( 1.0.rand, 1.0.rand );
  }

  knobColor {
    ^this.palette.baseTextColor;
  }

  knobColor_ { arg color;
    this.setProperty( \palette, this.palette.baseTextColor_(color) );
  }

  background {
    ^this.palette.baseColor;
  }

  background_ { arg color;
    this.setProperty( \palette, this.palette.baseColor_(color) );
  }
}

QRangeSlider : QAbstractStepValue {
  *qtClass { ^"QcRangeSlider" }

  *new { arg parent, bounds;
    ^super.new( parent, bounds ).initQRangeSlider( bounds );
  }

  initQRangeSlider { arg bounds;
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

  orientation_ { arg aSymbol;
    this.setProperty( \orientation, QOrientation(aSymbol) );
  }

  lo {
    ^this.getProperty( \loValue );
  }

  lo_ { arg aFloat;
    this.setProperty( \loValue, aFloat );
  }

  activeLo_ { arg aFloat;
    this.lo_(aFloat);
    this.doAction;
  }

  hi {
    ^this.getProperty( \hiValue );
  }

  hi_ { arg aFloat;
    this.setProperty( \hiValue, aFloat );
  }

  activeHi_ { arg aFloat;
    this.hi_(aFloat);
    this.doAction;
  }

  range {
    ^(this.hi - this.lo);
  }

  range_ { arg aFloat;
    this.hi_( this.lo + aFloat; )
  }

  activeRange_ { arg aFloat;
    this.range_(aFloat);
    this.doAction;
  }

  setSpan { arg lo, hi;
    this.lo_(lo);
    this.hi_(hi);
  }

  setSpanActive { arg lo, hi;
    this.setSpan(lo,hi);
    this.doAction;
  }

  setDeviation { arg deviation, average;
    this.lo_( average - deviation );
    this.hi_( average + deviation );
  }

  knobColor {
    ^this.palette.baseTextColor;
  }

  knobColor_ { arg color;
    this.setProperty( \palette, this.palette.baseTextColor_(color) );
  }

  background {
    ^this.palette.baseColor;
  }

  background_ { arg color;
    this.setProperty( \palette, this.palette.baseColor_(color) )
  }
}

QNumberBox : QAbstractStepValue {
  var <clipLo, <clipHi, <scroll, <scroll_step, <decimals;
  var <align, <buttonsVisible = false;
  var <normalColor, <typingColor;

  *qtClass { ^"QcNumberBox" }

  *new { arg aParent, aBounds;
    var obj = super.new( aParent, aBounds );
    obj.initQNumberBox;
    ^obj;
  }

  initQNumberBox {
    clipLo = inf;
    clipHi = inf;
    scroll = true;
    scroll_step = 1;
    normalColor = Color.black;
    typingColor = Color.red;
  }

  clipLo_ { arg aFloat;
    clipLo = aFloat;
    this.setProperty( \minimum, aFloat; );
  }

  clipHi_ { arg aFloat;
    clipHi = aFloat;
    this.setProperty( \maximum, aFloat; );
  }

  scroll_ { arg aBool;
    scroll = aBool;
    this.setProperty( \scroll, aBool );
  }

  scroll_step_ { arg aFloat;
    scroll_step = aFloat;
    this.setProperty( \scrollStep, aFloat );
  }

  decimals_ {  arg anInt;
    decimals = anInt;
    this.setProperty( \decimals, anInt );
  }

  align_ { arg alignment;
    align = alignment;
    this.setProperty( \alignment, QAlignment(alignment));
  }

  stringColor {
    ^this.palette.baseTextColor;
  }

  stringColor_ { arg color;
    this.setProperty( \palette, this.palette.baseTextColor_(color) );
  }

  normalColor_ { arg aColor;
    normalColor = aColor;
    this.setProperty( \normalColor, aColor );
  }

  typingColor_ { arg aColor;
    typingColor = aColor;
    this.setProperty( \editingColor, aColor );
  }

  background {
    ^this.palette.baseColor;
  }

  background_ { arg color;
    this.setProperty( \palette, this.palette.baseColor_(color) )
  }

  buttonsVisible_ { arg aBool;
    buttonsVisible = aBool;
    this.setProperty( \buttonsVisible, aBool );
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

QListView : QItemViewBase {
  var <colors;
  var <enterKeyAction;

  *qtClass { ^"QcListWidget" }

  selectionMode_ { arg mode;
    var m;
    m = mode.switch(
      \none, {0},
      \single, {1},
      \multi, {2},
      \extended, {3},
      \contiguous, {4}
    );
    if( m == 0 ) {
      this.invokeMethod( \clearSelection );
      this.setProperty( \currentRow, -1 );
      this.setProperty( \focusPolicy, 0 );
    };
    this.setProperty( \selectionMode, m );
  }

  selectionMode {
    var modes = [\none, \single, \multi, \extended, \contiguous];
    var m = this.getProperty( \selectionMode );
    ^modes[m];
  }

  value {
    var v = this.getProperty( \currentRow );
    if( v < 0 ) { ^nil } { ^v };
  }

  value_ { arg val;
    this.setProperty( \currentRow, val ? -1 );
  }

  stringColor {
    ^this.palette.baseTextColor;
  }

  stringColor_ { arg color;
    this.setProperty( \palette, this.palette.baseTextColor_(color) );
  }

  selectedStringColor {
    ^this.palette.highlightedTextColor;
  }

  selectedStringColor_ { arg color;
    this.setProperty( \palette, this.palette.highlightedTextColor_(color) );
  }

  hiliteColor {
    ^this.palette.highlightColor;
  }

  hiliteColor_ { arg color;
    this.setProperty( \palette, this.palette.highlightColor_(color) );
  }

  enterKeyAction_ { arg func;
    enterKeyAction = func;
    this.connectMethod( "returnPressed()", \enterKey );
  }

  enterKey {
    enterKeyAction.value( this );
  }

  colors_ { arg colorArray;
    colors = colorArray;
    this.setProperty( \colors, colorArray );
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

QMultiSliderView : QView {
  var <size=0, <editable=true, <step=0;
  var <reference;
  var <indexIsHorizontal=true, <elasticMode=false;
  var <indexThumbSize=12, <valueThumbSize=12, <gap=1;
  var <drawLines=false, <drawRects=true;
  var <metaAction;


  *qtClass { ^"QcMultiSlider" }

  background {
    ^this.palette.baseColor;
  }

  background_ { arg color;
    this.setProperty( \palette, this.palette.baseColor_(color) );
  }

  size_ { arg int;
    size = int;
    this.setProperty( \size, int );
  }

  indexIsHorizontal_ { arg bool;
    indexIsHorizontal = bool;
    if( bool ) {
      this.setProperty( \orientation, QOrientation(\horizontal) );
    } {
      this.setProperty( \orientation, QOrientation(\vertical) );
    };
  }

  editable_ { arg aBool;
    editable = aBool;
    this.setProperty( \editable, aBool );
  }

  readOnly {
    ^editable.not;
  }

  readOnly_ { arg bool;
    this.editable_( bool.not );
  }

  step_ { arg aFloat;
    step = aFloat;
    this.setProperty( \stepSize, aFloat );
  }

  value {
    ^this.getProperty( \values );
  }

  value_ { arg floatArray;
    this.setProperty( \values, floatArray );
  }

  valueAction_ { arg val;
    this.value_(val);
    action.value(this);
  }

  currentvalue {
    ^this.getProperty( \value );
  }

  currentvalue_ { arg aFloat;
    this.setProperty( \value, aFloat );
  }

  index {
    ^this.getProperty( \index );
  }

  index_ { arg anInt;
    this.setProperty( \index, anInt );
  }

  selectionSize {
    ^this.getProperty( \selectionSize );
  }

  selectionSize_ { arg anInt;
    this.setProperty( \selectionSize, anInt );
  }

  reference_ { arg aFloatArray;
    reference = aFloatArray;
    this.setProperty( \reference, aFloatArray );
  }

  startIndex_ { arg anInt;
    this.setProperty( \startIndex, anInt );
  }

  elasticMode_ { arg int;
    elasticMode = int.booleanValue;
    this.setProperty( \elastic, elasticMode);
  }

  thumbSize_ { arg float;
    this.indexThumbSize_(float);
    this.valueThumbSize_(float);
  }

  indexThumbSize_ { arg float;
    indexThumbSize = float;
    this.setProperty( \indexThumbSize, float );
  }

  valueThumbSize_ { arg float;
    valueThumbSize = float;
    this.setProperty( \valueThumbSize, float );
  }

  gap_ { arg anInt;
    gap = anInt;
    this.setProperty( \gap, anInt );
  }

  drawLines_ { arg bool;
    drawLines = bool;
    this.setProperty( \drawLines, bool );
  }

  drawRects_ { arg bool;
    drawRects = bool;
    this.setProperty( \drawRects, bool );
  }

  showIndex_ { arg aBool;
    this.setProperty( \highlight, aBool );
  }

  isFilled_ { arg aBool;
    this.setProperty( \isFilled, aBool );
  }

  fillColor_ { arg aColor;
    this.setProperty( \fillColor, aColor );
  }

  strokeColor_ { arg aColor;
    this.setProperty( \strokeColor, aColor );
  }

  colors_ { arg colorStroke, colorFill;
    this.strokeColor_( colorStroke );
    this.fillColor_ ( colorFill );
  }

  metaAction_ { arg func;
    if( metaAction.isNil ) { this.connectMethod( "metaAction()", \doMetaAction ) };
    metaAction = func;
  }

  doMetaAction {
    metaAction.value(this);
  }
}

QTextView : QAbstractScroll {
  var <stringColor, <font, <editable=true;

  /*TODO*/ var <enterInterpretsSelection;

  *qtClass { ^"QcTextEdit" }

  enterInterpretsSelection_ {
    this.nonimpl( "enterInterpretsSelection" );
  }

  editable_ { arg aBool;
    editable = aBool;
    this.setProperty( \readOnly, aBool.not );
  }

  usesTabToFocusNextView_ { arg aBool;
    this.setProperty( \tabChangesFocus, aBool );
  }

  open { arg aString;
    this.setProperty( \document, aString );
  }

  //FIXME where do line breaks go??
  string {
    ^this.getProperty( \plainText );
  }

  string_ { arg aString;
    this.setProperty( \plainText, aString );
  }

  font_ { arg aFont;
    font = aFont;
    this.setProperty( \textFont, aFont );
  }

  stringColor_ { arg aColor;
    stringColor = aColor;
    this.setProperty( \textColor, aColor );
  }

  selectedString {
    ^this.getProperty( \selectedString );
  }

  selectionStart {
    ^this.getProperty( \selectionStart );
  }

  selectionSize {
    ^this.getProperty( \selectionSize );
  }

  setStringColor { arg aColor, intStart, intSize;
    this.setProperty( \rangeColor, [aColor,intStart,intSize] );
  }

  setFont { arg aFont, intStart, intSize;
    this.setProperty( \rangeFont, [aFont,intStart,intSize] );
  }

  setString { arg aString, intStart, intSize;
    this.setProperty( \rangeText, [aString,intStart,intSize] );
  }

  syntaxColorize {
    this.nonimpl( "syntaxColorize" );
  }
}

QScopeView : QView {
  var <bufnum, <style=0, <xZoom=1.0, <yZoom=1.0, <x=0.0, <y=0.0;
  var <waveColors;

  *qtClass { ^"QcScope" }

  bufnum_ { arg anInt;
    bufnum = anInt;
    this.setProperty( \bufferNumber, anInt );
  }

  style_ { arg anInt;
    style = anInt;
    this.setProperty( \style, anInt );
  }

  xZoom_ { arg aFloat;
    xZoom = aFloat;
    this.setProperty( \xZoom, aFloat );
  }

  yZoom_ { arg aFloat;
    yZoom = aFloat;
    this.setProperty( \yZoom, aFloat );
  }

  x_ { arg aFloat;
    x = aFloat;
    this.setProperty( \xOffset, aFloat );
  }

  y_ { arg aFloat;
    y = aFloat;
    this.setProperty( \yOffset, aFloat );
  }

  waveColors_ { arg aColorArray;
    waveColors = aColorArray;
    this.setProperty( \waveColors, aColorArray );
  }

  background { ^this.getProperty( \background, Color.new ); }

  background_ { arg color; this.setProperty( \background, color, true ); }
}
