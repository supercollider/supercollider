/////////////////////// ABSTRACT CLASSES ////////////////////////////

QTextViewBase : QView {
  var <object, <>setBoth = true;
  var <align;

  object_  { arg obj;
    if( setBoth ) { this.string = obj.asString };
    object = obj
  }

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

  clear {
    this.invokeMethod(\clear);
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
  var <step = 0.1, <shift_scale = 100.0, <ctrl_scale = 10.0, <alt_scale = 0.1;

  step_ { arg aFloat;
    step = aFloat;
    this.setProperty( \step, aFloat );
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

  getScale { |modifiers|
    ^case
      { modifiers.isShift } { shift_scale }
      { modifiers.isCtrl } { ctrl_scale }
      { modifiers.isAlt } { alt_scale }
      { 1 };
  }

  increment { arg factor = 1.0; this.invokeMethod( \increment, factor.asFloat ); }
  decrement { arg factor = 1.0; this.invokeMethod( \decrement, factor.asFloat ); }
}

/////////////////////// CONTAINERS ////////////////////////////////

QHLayoutView : QView {
  *qtClass { ^'QcHLayoutWidget' }
}

QVLayoutView : QView {
  *qtClass { ^'QcVLayoutWidget' }
}

QScrollCanvas : QObject {
  *qtClass { ^'QcScrollWidget' }

  background { ^this.getProperty(\background); }
  background_ { arg color; this.setProperty(\background, color); }
}

QScrollView : QAbstractScroll {
  var <canvas;
  var <hasBorder=true;
  var actionConnected=false;

  *new { arg parent, bounds;
    ^super.new( parent, bounds ).initQScrollView;
  }

  *qtClass { ^'QcScrollArea' }

  children { arg class = QView;
    ^canvas.children( class );
  }

  background { ^canvas.background }
  background_ { arg color; canvas.background = color }

  hasBorder_ { arg aBool;
    hasBorder = aBool;
    this.setProperty( \hasBorder, aBool );
  }

  innerBounds {
    ^this.getProperty( \innerBounds );
  }

  visibleOrigin {
    ^this.getProperty( \visibleOrigin );
  }

  visibleOrigin_ { arg point;
    this.setProperty( \visibleOrigin, point );
    this.doAction;
  }

  canvas_ { arg view;
    canvas = view;
    this.invokeMethod( \setWidget, view, true );
  }

  action_ { arg handler;
    action = handler;
    if(actionConnected.not) { this.connectMethod( 'scrolled()', \doAction ) };
    actionConnected = true;
  }

  initQScrollView {
    // NOTE: The canvas widget must not be a QView, so that asking its
    // children for parent will skip it and hit this view instead.
    this.canvas = QScrollCanvas();
  }
}

/////////////////////////// WIDGETS ///////////////////////////////

QStaticText : QTextViewBase {
  *qtClass { ^'QLabel' }

  *new { arg aParent, aBounds;
    var obj = super.new( aParent, aBounds );
    obj.setProperty(\wordWrap, true);
    ^obj;
  }

  background {
    var p = this.palette;
    ^if(p.hasColor(\window)) {p.window} {nil}
  }

  background_ { arg color;
    var p = this.palette;
    if(p.hasColor(\window).not)
      { this.setProperty( \autoFillBackground, true) };
    this.palette = p.window_(color);
  }

  string { ^this.getProperty(\text) }
  string_ { arg text; this.setProperty( \text, text.asString ) }

  stringColor {
    ^this.palette.windowText;
  }

  stringColor_ { arg color;
    this.palette = this.palette.windowText_(color);
  }
}

QTextField : QTextViewBase {
  *qtClass { ^'QcTextField' }

  string {
    ^this.getProperty( \text );
  }

  string_ { arg text;
    this.setProperty( \text, text );
  }

  stringColor {
    ^this.palette.baseText;
  }

  stringColor_ { arg color;
    this.palette = this.palette.baseText_(color);
  }

  background {
    ^this.palette.base;
  }

  background_ { arg color;
    this.palette = this.palette.base_(color);
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

  defaultGetDrag { ^this.string; }
  defaultCanReceiveDrag { ^true; }
  defaultReceiveDrag {
    this.valueAction = QView.currentDrag;
  }
}

QButton : QView {
  var <states;

  *qtClass { ^'QcButton' }

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

  action_ { arg func;
    this.manageMethodConnection( action, func, 'action(int)', \prDoAction );
    action = func;
  }

  doAction { arg modifiers;
    action.value(this, modifiers);
  }

  defaultGetDrag { ^this.value; }
  defaultCanReceiveDrag { ^true; }
  defaultReceiveDrag {
    if( QView.currentDrag.isNumber )
      { this.valueAction = QView.currentDrag; }
      { this.action = QView.currentDrag; };
  }

  prDoAction { arg mods;
    this.doAction(QKeyModifiers.toCocoa(mods));
  }
}

QCheckBox : QView {

  *qtClass { ^'QcCheckBox' }

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

  valueAction_ { |val|
    this.value_(val);
    this.doAction;
  }

  string_{ |string|
    this.setProperty(\text,string)
  }

  string{
    ^this.getProperty(\text)
  }

  defaultGetDrag { ^this.value; }
  defaultCanReceiveDrag { ^((QView.currentDrag == true) || (QView.currentDrag == false)); }
  defaultReceiveDrag {
    this.valueAction = QView.currentDrag;
  }
}

QPopUpMenu : QItemViewBase {

  *qtClass { ^'QcPopUpMenu' }

  allowsReselection { ^this.getProperty( \reactivationEnabled ) }

  allowsReselection_ { arg flag; ^this.setProperty( \reactivationEnabled, flag ) }

  value {
    var v = this.getProperty( \currentIndex );
    if( v < 0 ) { ^nil } { ^v };
  }

  value_ { arg val;
    this.setProperty( \currentIndex, val ? -1 );
  }

  background { ^this.palette.button; }

  background_ { arg color; this.palette = this.palette.button_(color); }

  stringColor {
    ^this.palette.buttonText;
  }

  stringColor_ { arg color;
    this.palette = this.palette.buttonText_(color);
  }

  defaultGetDrag { ^this.value; }
  defaultCanReceiveDrag { ^QView.currentDrag.isNumber; }
  defaultReceiveDrag {
    this.valueAction = QView.currentDrag;
  }
}
