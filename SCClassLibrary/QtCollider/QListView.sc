QListView : QItemViewBase {
  var <colors;
  var <enterKeyAction, <selectionAction;

  *qtClass { ^'QcListWidget' }

  *new { arg parent, bounds;
     ^super.new(parent, bounds)
       .setEventHandler( QObject.mouseMoveEvent, \mouseMoveEvent, true );
  }

  mouseDownEvent { arg x, y, modifiers, buttonNumber, clickCount;
    // Override QView:mouseDownEvent: postpone drag start to move event
    modifiers = QKeyModifiers.toCocoa(modifiers);
    ^this.mouseDown( x, y, modifiers, buttonNumber, clickCount );
  }

  mouseMoveEvent { arg x, y, modifiers, buttons;
    // Override QView:mouseMoveEvent: start drag
    if( buttons != 0 and: ((modifiers & QKeyModifiers.control) > 0) ) {
      if( this.beginDrag( x, y ) ) { ^true };
    };

    ^super.mouseMoveEvent(x, y, modifiers, buttons);
  }

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

  selection { ^ this.getProperty(\selection) }

  selection_  { arg indexes;
        if (indexes.isNumber) { indexes = [indexes] };
        this.setProperty(\selection, indexes)
    }

  background { ^this.palette.base; }
  background_ { arg color; this.palette = this.palette.base_(color); }

  stringColor {
    ^this.palette.baseText;
  }

  stringColor_ { arg color;
    this.palette = this.palette.baseText_(color);
  }

  selectedStringColor {
    ^this.palette.highlightText;
  }

  selectedStringColor_ { arg color;
    this.palette = this.palette.highlightText_(color);
  }

  hiliteColor {
    ^this.palette.highlight;
  }

  hiliteColor_ { arg color;
    this.palette = this.palette.highlight_(color);
  }

  enterKeyAction_ { arg func;
    this.manageMethodConnection( enterKeyAction, func, 'returnPressed()', 'enterKey' );
    enterKeyAction = func;
  }

  enterKey {
    enterKeyAction.value( this );
  }

  selectionAction_ { arg func;
    this.manageFunctionConnection( selectionAction, func, 'itemSelectionChanged()' );
    selectionAction = func;
  }

  colors_ { arg colorArray;
    colors = colorArray;
    this.setProperty( \colors, colorArray );
  }

  defaultGetDrag { ^this.value; }
  defaultCanReceiveDrag { ^QView.currentDrag.isNumber; }
  defaultReceiveDrag {
    this.valueAction = QView.currentDrag;
  }
}
