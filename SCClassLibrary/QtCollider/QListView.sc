QListView : QItemViewBase {
  var <colors;
  var <enterKeyAction;

  *qtClass { ^"QcListWidget" }

  mouseDownEvent { arg x, y, modifiers, buttonNumber, clickCount;
    // Override QView:mouseDownEvent:
    // If Ctrl / Cmd is pressed, try to start the drag after this event
    // is processed, so that current item can be changed before.
    if( (modifiers & QKeyModifiers.control) > 0 ) {
      AppClock.sched( 0, {this.beginDrag(x,y)} );
    };

    modifiers = QKeyModifiers.toCocoa(modifiers);
    ^this.mouseDown( x, y, modifiers, buttonNumber, clickCount );
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
