/////////////////////// CONTAINERS ////////////////////////////////

QLayout : QView {

}

QHLayout : QLayout {
}

QVLayout : QLayout {
}

QScrollArea : QView {
  var <background, <hasBorder=true;

  background_ { arg aColor;
    background = aColor;
    this.setProperty( \background, aColor, true );
  }

  hasBorder_ { arg aBool;
    hasBorder = aBool;
    this.setProperty( \hasBorder, aBool );
  }
}

/////////////////////////// WIDGETS ///////////////////////////////

QTextViewBase : QView {
  var <align;

  align_ { arg aSymbol;
    align = aSymbol;
    this.setProperty( \alignment, alignmentDict[ aSymbol ] );
  }
}

QStaticText : QTextViewBase {
  var <string;

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
    ^palette.windowTextColor;
  }

  stringColor_ { arg color;
    palette.windowTextColor = color;
    this.setProperty( \palette, palette );
  }
}

QTextField : QTextViewBase {
  string {
    ^this.getProperty( \text );
  }

  string_ { arg text;
    this.setProperty( \text, text );
  }

  stringColor {
    ^palette.baseTextColor;
  }

  stringColor_ { arg color;
    palette.baseTextColor = color;
    this.setProperty( \palette, palette );
  }

  background {
    ^palette.baseColor;
  }

  background_ { arg color;
    palette.baseColor = color;
    this.setProperty( \palette, palette )
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

  *new { arg parent, bounds;
    ^super.new( parent, bounds ).prInitQSlider( bounds.asRect );
  }

  knobColor {
    ^palette.buttonColor;
  }

  knobColor_ { arg aColor;
    palette.buttonColor = aColor;
    this.setProperty( \palette, palette );
  }

  prInitQSlider { arg bounds;
    if( bounds.notNil ) {
      if( bounds.width > bounds.height ) {
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
    this.setProperty( \orientation, orientationDict[ aSymbol ] );
  }
}

QSlider2D : QAbstractStepValue {
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
    ^palette.baseTextColor;
  }

  knobColor_ { arg aColor;
    palette.baseTextColor = aColor;
    this.setProperty( \palette, palette );
  }

  background {
    ^palette.baseColor;
  }

  background_ { arg color;
    palette.baseColor = color;
    this.setProperty( \palette, palette )
  }
}

QRangeSlider : QAbstractStepValue {

  *new { arg parent, bounds;
    ^super.new( parent, bounds ).prInitQRangeSlider( bounds.asRect );
  }

  prInitQRangeSlider { arg bounds;
    if( bounds.notNil ) {
      if( bounds.width > bounds.height ) {
        this.orientation_( \horizontal );
      } {
        this.orientation_( \vertical );
      }
    }
  }

  orientation_ { arg aSymbol;
    this.setProperty( \orientation, orientationDict[ aSymbol ] );
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
    ^palette.baseTextColor;
  }

  knobColor_ { arg aColor;
    palette.baseTextColor = aColor;
    this.setProperty( \palette, palette );
  }

  background {
    ^palette.baseColor;
  }

  background_ { arg color;
    palette.baseColor = color;
    this.setProperty( \palette, palette )
  }
}

QNumberBox : QAbstractStepValue {
  var <clipLo, <clipHi, <scroll, <scroll_step, <decimals;
  var <align, <buttonsVisible = false;
  var <normalColor, <typingColor;

  *new { arg aParent, aBounds;
    var obj = super.new( aParent, aBounds );
    obj.prInitQNumberBox;
    ^obj;
  }

  prInitQNumberBox {
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
    this.setProperty( \alignment, alignmentDict[ alignment ]);
  }

  stringColor {
    ^palette.baseTextColor;
  }

  stringColor_ { arg color;
    palette.baseTextColor = color;
    this.setProperty( \palette, palette );
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
    ^palette.baseColor;
  }

  background_ { arg color;
    palette.baseColor = color;
    this.setProperty( \palette, palette )
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
  var <>enterKeyAction;

  value {
    ^this.getProperty( \currentRow );
  }

  value_ { arg val;
    this.setProperty( \currentRow, val );
  }

  stringColor {
    ^palette.baseTextColor;
  }

  stringColor_ { arg aColor;
    palette.baseTextColor = aColor;
    this.setProperty( \palette, palette );
  }

  selectedStringColor {
    ^palette.higlightedTextColor;
  }

  selectedStringColor_ { arg aColor;
    palette.higlightedTextColor = aColor;
    this.setProperty( \palette, palette );
  }

  hiliteColor {
    ^palette.highlightColor;
  }

  hiliteColor_ { arg aColor;
    palette.highlightColor = aColor;
    this.setProperty( \palette, palette );
  }

  enterKey {
    enterKeyAction.value( this );
  }

  colors_ { arg aColor;
    colors = aColor;
    this.nonimpl( "colors" );
  }
}

QPopUpMenu : QItemViewBase {
  value {
    ^this.getProperty( \currentIndex );
  }

  value_ { arg val;
    this.setProperty( \currentIndex, val );
  }

  stringColor {
    ^palette.buttonTextColor;
  }

  stringColor_ { arg aColor;
    palette.buttonTextColor = aColor;
    this.setProperty( \palette, palette );
  }
}

QMultiSliderView : QView {
  var <size=0, <editable=true, <step=0;
  var <reference;
  var <indexIsHorizontal=true, <elasticMode=false;
  var <indexThumbSize=12, <valueThumbSize=12, <gap=1;
  var <drawLines=false, <drawRects=true;
  var <>metaAction;

  size_ { arg int;
    size = int;
    this.setProperty( \size, int );
  }

  indexIsHorizontal_ { arg bool;
    indexIsHorizontal = bool;
    if( bool ) {
      this.setProperty( \orientation, orientationDict[\horizontal] );
    } {
      this.setProperty( \orientation, orientationDict[\vertical] );
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
    elasticMode = int;
    this.setProperty( \elastic, int > 0 );
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

  doMetaAction {
    metaAction.value(this);
  }
}

QTextView : QView {
  var <stringColor, <font, <editable=true;

  /*TODO*/ var <enterInterpretsSelection;

  enterInterpretsSelection_ {
    this.nonimpl( "enterInterpretsSelection" );
  }

  editable_ { arg aBool;
    editable = aBool;
    this.setProperty( \readOnly, aBool.not );
  }

  autohidesScrollers_ { arg aBool;
    if( aBool ) {
      this.setProperty( \horizontalScrollBarPolicy, 0 );
      this.setProperty( \verticalScrollBarPolicy, 0 );
    } {
      this.setProperty( \horizontalScrollBarPolicy, 2 );
      this.setProperty( \verticalScrollBarPolicy, 2 );
    }
  }

  hasHorizontalScroller_ { arg aBool;
    if( aBool ) {
      this.setProperty( \horizontalScrollBarPolicy, 2 );
    } {
      this.setProperty( \horizontalScrollBarPolicy, 1 );
    }
  }

  hasVerticalScroller_ { arg aBool;
    if( aBool ) {
      this.setProperty( \verticalScrollBarPolicy, 2 );
    } {
      this.setProperty( \verticalScrollBarPolicy, 1 );
    }
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
}
