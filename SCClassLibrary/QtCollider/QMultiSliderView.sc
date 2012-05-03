QMultiSliderView : QView {
  var <editable=true;
  var <elasticMode=false;
  var <indexThumbSize=12, <valueThumbSize=12, <gap=1;
  var <drawLines=false, <drawRects=true;
  var <metaAction;


  *qtClass { ^'QcMultiSlider' }

  size { ^this.getProperty(\sliderCount) }
  size_ { arg int; this.setProperty( \sliderCount, int ) }

  indexIsHorizontal { ^this.getProperty(\orientation) == QOrientation(\vertical) }

  indexIsHorizontal_ { arg bool;
    this.setProperty( \orientation, QOrientation(if(bool){\vertical}{\horizontal}) );
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

  step { ^this.getProperty(\step) }
  step_ { arg val; this.setProperty( \step, val ) }

  value {
    ^this.getProperty( \values );
  }

  value_ { arg array;
    if( array.isKindOf(DoubleArray).not and: {array.isKindOf(FloatArray).not} )
      { array = array.as(DoubleArray) };
    this.setProperty( \values, array );
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

  reference { ^this.getProperty(\reference) }

  reference_ { arg array;
    if( array.isKindOf(DoubleArray).not and: {array.isKindOf(FloatArray).not} )
      { array = array.as(DoubleArray) };
    this.setProperty( \reference, array );
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

  // alias for 'gap'
  xOffset_ { arg int; this.gap_(int); }
  xOffset { arg int; ^this.gap; }

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

  fillColor { ^this.getProperty(\fillColor) }
  fillColor_ { arg color; this.setProperty( \fillColor, color ) }

  strokeColor { ^this.getProperty(\strokeColor) }
  strokeColor_ { arg color; this.setProperty( \strokeColor, color ) }

  colors_ { arg colorStroke, colorFill;
    this.strokeColor_( colorStroke );
    this.fillColor_ ( colorFill );
  }

  metaAction_ { arg func;
    this.manageMethodConnection( metaAction, func, 'metaAction()', \doMetaAction );
    metaAction = func;
  }

  doMetaAction {
    metaAction.value(this);
  }

  defaultKeyDownAction { arg char, mod, uni, keycode, key;
    key.switch (
      QKey.left, { this.index = this.index - 1 },
      QKey.right, { this.index = this.index + 1 },
      QKey.up, { this.currentvalue = this.currentvalue + this.step },
      QKey.down, { this.currentvalue = this.currentvalue - this.step }
    );
  }

  defaultGetDrag {
    var val = this.value;
    var c, i;
    if( val.size < 1 ) {^nil};
    c = this.selectionSize;
    if( c > 1 ) {
      i = this.index;
      ^val[i..(i+c-1)];
    }
    ^this.value;
  }
  defaultCanReceiveDrag { ^true; }
  defaultReceiveDrag {
    arg data = QView.currentDrag;
    if( data.size > 0 ) {
      if( data[0].size > 0 ) {
        this.value = data[0];
        this.reference = data[1];
      }{
        this.value = data;
      }
    };
  }
}
