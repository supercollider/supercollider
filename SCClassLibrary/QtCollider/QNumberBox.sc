QNumberBox : QAbstractStepValue {
  var <scroll, <scroll_step;
  var <align, <buttonsVisible = false;
  var <normalColor, <typingColor;
  var <object, <>setBoth = true;

  *qtClass { ^'QcNumberBox' }

  *new { arg aParent, aBounds;
    var obj = super.new( aParent, aBounds );
    obj.initQNumberBox;
    ^obj;
  }

  initQNumberBox {
    scroll = true;
    scroll_step = 1;
    normalColor = Color.black;
    typingColor = Color.red;
  }

  object_  { arg obj;
    if( setBoth ) {
      if( obj.isNumber ) { this.value = obj } { this.string = obj.asString }
    };
    object = obj
  }

  value {
    var type = this.getProperty( \valueType );
    var val;
    switch( type,
      0 /* Number */, { val = this.getProperty( \value ) },
      1 /* Inf */, { val = inf },
      2 /* -Inf */, { val = -inf },
      3 /* NaN */, { val = 0 },
      4 /* Text */, { val = 0 }
    );
    ^val;
  }

  value_ { arg value;
    case
      // isNaN has to be on the first plase, because a NaN is also equal to inf and -inf
      { value.isNaN } { this.invokeMethod( \setNaN ); }
      { value == inf } { this.invokeMethod( \setInfinite, true ); }
      { value == -inf } { this.invokeMethod( \setInfinite, false ); }
      { this.setProperty( \value, value.asFloat ); }
    ;
  }

  valueAction_ { arg val;
    this.value_(val);
    action.value(this);
  }

  string { ^this.getProperty( \text ); }

  string_ { arg string; this.setProperty( \text, string ); }

  clipLo { ^this.getProperty(\minimum) }
  clipLo_ { arg aFloat; this.setProperty( \minimum, aFloat ) }

  clipHi { ^this.getProperty(\maximum) }
  clipHi_ { arg aFloat; this.setProperty( \maximum, aFloat ) }

  scroll_ { arg aBool;
    scroll = aBool;
    this.setProperty( \scroll, aBool );
  }

  scroll_step_ { arg aFloat;
    scroll_step = aFloat;
    this.setProperty( \scrollStep, aFloat );
  }

  decimals { ^this.getProperty(\decimals); }
  minDecimals { ^this.getProperty(\minDecimals); }
  maxDecimals { ^this.getProperty(\maxDecimals); }

  decimals_ {  arg decimals; this.setProperty( \decimals, decimals ); }
  minDecimals_ { arg decimals; this.setProperty( \minDecimals, decimals ); }
  maxDecimals_ { arg decimals; this.setProperty( \maxDecimals, decimals ); }

  align_ { arg alignment;
    align = alignment;
    this.setProperty( \alignment, QAlignment(alignment));
  }

  stringColor {
    ^this.palette.baseText;
  }

  stringColor_ { arg color;
    this.palette = this.palette.baseText_(color);
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
    ^this.palette.base;
  }

  background_ { arg color;
    this.palette = this.palette.base_(color);
  }

  buttonsVisible_ { arg aBool;
    buttonsVisible = aBool;
    this.setProperty( \buttonsVisible, aBool );
  }

  defaultGetDrag { ^this.value; }
  defaultCanReceiveDrag { ^QView.currentDrag.isNumber; }
  defaultReceiveDrag {
    this.valueAction = QView.currentDrag;
  }
}
