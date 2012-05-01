QLayout : QObject {
  spacing_ { arg spacing;
    this.setProperty( \spacing, spacing );
  }

  margins_ { arg margins;
    this.setProperty( \margins, margins + [0,0,0,0] );
  }
}

// LINE LAYOUTS ///////////////////////////////////////////////////

QLineLayout : QLayout {
  *new { arg ...items;
    var serializedItems = items.collect( { |x| this.parse(x) } );
    ^super.new( [serializedItems] );
  }

  *parse { arg in;
    var out = [nil,0,0];
    var key;
    var i;
    if( in.isKindOf(Array) ) {
      out[0] = in[0];
      i = 1;
      while { i + 1 < in.size } {
        key = in[i];
        case (
          { (key === \stretch) || (key === \s) }, { out[1] = in[i+1] },
          { (key === \align) || (key === \a) }, { out[2] = QAlignment(in[i+1]) }
        );
        i = i + 2;
      };
    }{
      out[0] = in;
    };
    ^out;
  }

  add { arg item, stretch = 0, align;
    this.invokeMethod( \addItem, [[item, stretch, QAlignment(align)]], true );
  }

  insert { arg item, index=0, stretch = 0, align;
    this.invokeMethod( \insertItem, [[item, index, stretch, QAlignment(align)]], true );
  }

  setStretch { arg item, stretch;
    this.invokeMethod( \setStretch, [item, stretch], true );
  }

  setAlignment { arg item, align;
    this.invokeMethod( \setAlignment, [item, QAlignment(align)], true );
  }
}

QHLayout : QLineLayout {
  *implementsClass {^'HLayout'}
  *qtClass { ^'QcHBoxLayout' }
}

QVLayout : QLineLayout {
  *implementsClass {^'VLayout'}
  *qtClass { ^'QcVBoxLayout' }
}

// GRID LAYOUT ///////////////////////////////////////////////////

QGridLayout : QLayout {
  *implementsClass {^'GridLayout'}

  *new {
    // get rid of QObject's arguments
    ^super.new;
  }

  *qtClass { ^'QcGridLayout' }

  *parse { arg in, row, col;
    var out = [nil,row,col,1,1,nil];
    var key;
    var i;
    if( in.isKindOf(Array) ) {
      out[0] = in[0];
      i = 1;
      while { i + 1 < in.size } {
        key = in[i];
        case (
          { (key === \rows) || (key === \r) }, { out[3] = in[i+1] },
          { (key === \columns) || (key === \c) }, { out[4] = in[i+1] },
          { (key === \align) || (key === \a) }, { out[5] = QAlignment(in[i+1]) }
        );
        i = i + 2;
      };
    }{
      out[0] = in;
    };
    ^out;
  }


  *rows { arg ...rows ;
    var grid;
    var data;
    grid = this.new;
    rows.do { |row, r|
      if( row.size > 0 ) {
        row.do { |item, c|
          if( item.notNil ) {
            data = this.parse( item, r, c );
            grid.invokeMethod( \addItem, [data], true );
          };
        };
      };
    }
    ^grid;
  }

  *columns { arg ...cols;
    var grid;
    var data;
    grid = this.new;
    cols.do { |col, c|
      if( col.size > 0 ) {
        col.do { |item, r|
          if( item.notNil ) {
            data = this.parse( item, r, c );
            grid.invokeMethod( \addItem, [data], true );
          };
        };
      };
    }
    ^grid;
  }

  add { arg item, row, column, align;
    this.invokeMethod( \addItem, [[item, row, column, 1, 1, QAlignment(align)]], true );
  }

  addSpanning { arg item, row, column, rowSpan=1, columnSpan=1, align;
    this.invokeMethod( \addItem, [[item, row, column,
                                   rowSpan, columnSpan,
                                   QAlignment(align)
                                 ]], true );
  }

  hSpacing_ { arg spacing;
    this.setProperty( \horizontalSpacing, spacing );
  }

  vSpacing_ { arg spacing;
    this.setProperty( \verticalSpacing, spacing );
  }

  setRowStretch{ arg row, factor;
    this.invokeMethod( 'setRowStretch', [row, factor], true );
  }

  setColumnStretch{ arg column, factor;
    this.invokeMethod( 'setColumnStretch', [column, factor], true );
  }

  setAlignment { arg item, align;
    var args = if( item.class === Point )
      { [item.y, item.x, QAlignment(align)] }
      { [item, QAlignment(align)] };

    this.invokeMethod( \setAlignment, args, true );
  }

  minRowHeight { arg row; ^this.invokeMethod( \minRowHeight, row ); }

  setMinRowHeight { arg row, height;
    this.invokeMethod( \setMinRowHeight, [row, height] );
  }

  minColumnWidth { arg column; ^this.invokeMethod( \minColumnWidth, column ); }

  setMinColumnWidth { arg column, width;
    this.invokeMethod( \setMinColumnWidth, [column, width] );
  }
}

QStackLayout : QLayout
{
  *implementsClass {^'StackLayout'}

  *qtClass { ^'QcStackLayout' }

  *new { arg ...views; ^super.new([views]) }

  add { arg view; this.insert(view, -1) }

  insert { arg view, index = 0; this.invokeMethod( \insertWidget, [index, view] ) }

  index { ^this.getProperty(\currentIndex) }
  index_ { arg value; this.setProperty(\currentIndex, value) }

  count { ^this.getProperty(\count) }

  mode { ^this.getProperty(\stackingMode) }
  mode_ { arg value;
    value = value.switch(
      \stackOne, 0,
      \stackAll, 1,
      value
    );
    value = value.clip(0, 1).asInteger;
    this.setProperty(\stackingMode, value)
  }
}
