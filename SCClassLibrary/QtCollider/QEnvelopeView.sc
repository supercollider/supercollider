QEnvelopeView : QView
{
  var <editable, <step, <grid, <gridOn = false;
  var <fillColor;
  var <drawLines = true, <drawRects = true;
  var <metaAction;

  *qtClass {^'QcGraph'}

  editable_ { arg aBool;
    editable = aBool;
    this.setProperty( \editable, aBool );
  }

  setEditable { arg index, flag;
    this.invokeMethod( \setEditableAt, [index, flag] );
  }

  step_ { arg aFloat;
    step = aFloat;
    this.setProperty( \step, aFloat );
  }

  keepHorizontalOrder {
    ^this.getProperty( \horizontalOrder ) != 0;
  }

  keepHorizontalOrder_ { arg bool;
    this.setProperty( \horizontalOrder, if(bool){1}{0} );
  }

  elasticSelection {
    ^this.getProperty( \selectionForm ) == 0;
  }

  elasticSelection_ { arg bool;
    this.setProperty( \selectionForm, if(bool){0}{1} );
  }

  value {
    ^this.getProperty( \value );
  }

  value_ { arg anArray;
    this.setProperty( \value, anArray );
  }

  valueAction_ { arg anArray;
    this.setProperty( \value, anArray );
    this.doAction;
  }

  index {
    ^this.getProperty( \index );
  }

  index_ { arg index;
    ^this.setProperty( \index, index );
  }

  lastIndex { ^this.getProperty( \lastIndex ); }

  selection { ^this.getProperty( \selectionIndexes );}

  selectIndex { arg index;
    if( index < 0 ){
      this.invokeMethod( \deselectAll );
    }{
      this.invokeMethod( \select, [index,false] );
    };
    this.index = index;
  }

  deselectIndex { arg index;
    this.invokeMethod( \deselect, index );
  }

  x {
    ^this.getProperty( \x );
  }

  x_ { arg aFloat;
    this.setProperty( \x, aFloat );
  }

  y {
    ^this.getProperty( \y );
  }

  y_ { arg aFloat;
    this.setProperty( \y, aFloat );
  }

  currentvalue { ^this.y }

  currentvalue_ { arg aFloat;
    this.y_( aFloat );
  }

  setString { arg index, string;
    this.invokeMethod( \setStringAt, [index, string] );
  }

  strings_ { arg anArray;
    this.setProperty( \strings, anArray );
  }

  curves { this.nonimpl( "curves" ); }

  curves_ { arg curves;
    this.invokeMethod( \setCurves,
      if(curves.size > 0) { [curves.collect{|c| QCurve(c)}] } { QCurve(curves) }
    );
  }

  setEnv { arg env;
    var times = [0] ++ env.times.integrate;
    if( times.last > 0 ) {times = times / times.last};
    this.value = [times, env.levels];
    this.curves = env.curves;
  }

  grid_ { arg aPoint;
    grid = aPoint;
    this.setProperty( \grid, aPoint );
  }

  gridOn_ { arg aBool;
    gridOn = aBool;
    this.setProperty( \gridOn, aBool );
  }

  connect { arg source, targets;
    this.invokeMethod( \connectElements, [source, targets] );
  }

  gridColor { ^this.getProperty(\gridColor) }
  gridColor_ { arg color; this.setProperty( \gridColor, color ) }

  selectionColor { ^this.getProperty(\selectionColor) }
  selectionColor_ { arg color; this.setProperty(\selectionColor, color) }

  strokeColor { ^this.getProperty(\strokeColor) }
  strokeColor_ { arg color; this.setProperty( \strokeColor, color ) }

  fillColor_ { arg aColor;
    fillColor = aColor;
    this.setProperty( \fillColor, aColor );
  }

  setFillColor { arg index, color;
    this.invokeMethod( \setFillColorAt, [index, color] );
  }

  colors_ { arg strokeColor, fillColor;
    this.strokeColor_( strokeColor );
    this.fillColor_( fillColor );
  }

  drawLines_ { arg aBool;
    drawLines = aBool;
    this.setProperty( \drawLines, aBool );
  }

  drawRects_ { arg aBool;
    drawRects = aBool;
    this.setProperty( \drawRects, aBool );
  }

  style { ^this.getProperty(\style) }

  style_ { arg style;
    if (style.isNumber.not) {
      style = style.switch (
        \dots, 0,
        \rects, 1,
        0
      );
    };
    style = style.clip(0,1).asInteger;
    this.setProperty(\style, style)
  }

  thumbWidth_ { arg width;
    this.setProperty( \thumbWidth, width.asInteger; );
  }

  thumbHeight_ { arg height;
    this.setProperty( \thumbHeight, height.asInteger; );
  }

  thumbSize_ { arg size;
    this.setProperty( \thumbSize, size.asInteger; );
  }

  setThumbWidth { arg index, width;
    this.invokeMethod(\setThumbWidthAt, [index, width.asInteger])
  }

  setThumbHeight { arg index, height;
    this.invokeMethod(\setThumbHeightAt, [index, height.asInteger])
  }

  setThumbSize { arg index, size;
    this.invokeMethod(\setThumbSizeAt, [index, size.asInteger])
  }

  metaAction_ { arg function;
    this.manageMethodConnection( metaAction, function, 'metaAction()', \doMetaAction );
    metaAction = function;
  }

  doMetaAction {
    metaAction.value(this);
  }

  defaultGetDrag {
    ^this.value;
  }
  defaultCanReceiveDrag { ^true; }
  defaultReceiveDrag {
    this.value = QView.currentDrag;
  }
}
