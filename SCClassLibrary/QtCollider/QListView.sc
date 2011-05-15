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
