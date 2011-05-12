QLayout : QObject {
  spacing_ { arg spacing;
    this.setProperty( \spacing, spacing );
  }

  margins_ { arg margins;
    this.setProperty( \margins, margins + [0,0,0,0] );
  }
}

// LINE LAYOUTS ///////////////////////////////////////////////////

LineItem {
  var <object;
  var <stretch;
  var <align;

  *new { arg object, stretch = 0, align = 0;
    ^super.new.initLineItem( object, stretch, align );
  }

  *wrap { arg other;
    if( other.isKindOf( LineItem ) ) {^other} {^LineItem.new(other)};
  }

  initLineItem { arg o, s, a;
    object = o;
    stretch = s;
    align = a;
  }

  serialized { ^[ object, stretch, QAlignment(align)] }
}

StretchItem : LineItem {
  *new { arg factor = 0; ^super.new( 0, factor ) }
}

SpacerItem : LineItem {
  *new { arg size; ^super.new( size ); }
}

QLineLayout : QLayout {
  *new { arg ...items;
    var serializedItems = items.collect( { |x| this.parse(x) } );
    ^super.new( this.layoutClass, serializedItems );
  }

  *parse { arg in;
    var i = 1;
    var out;
    if( in.isKindOf(Array) ) {
      out = [in[0],0,0];
      while { i + 1 < in.size } {
        switch ( in[i],
          \stretch, { out[1] = in[i+1] },
          \align, { out[2] = QAlignment(in[i+1]) }
        );
        i = i + 2;
      };
    }{
      out = LineItem.wrap(in).serialized;
    };
    out.postln;
    ^out;
  }

  *layoutClass { ^'' }

  add { arg item, stretch = 0, align;
    var data;
    if( item.isKindOf(LineItem) )
      {data = item.serialized} {data = [item, stretch, QAlignment(align)]};
    this.invokeMethod( \addItem, [data], true );
  }

  setStretch { arg item, stretch;
    this.invokeMethod( \setStretch, [item, stretch], true );
  }

  setStretchAt { arg index, stretch; this.setStretch( index, stretch ); }

  setAlignment { arg item, align;
    this.invokeMethod( \setAlignment, [item, QAlignment(align)], true );
  }

  setAlignmentAt { arg index, align; this.setAlignment( index, align ); }
}

QHLayout : QLineLayout {
  *layoutClass { ^'QcHBoxLayout'; }
}

QVLayout : QLineLayout {
  *layoutClass { ^'QcVBoxLayout'; }
}

// GRID LAYOUT ///////////////////////////////////////////////////

GridItem {
  var <object, <rowSpan, <columnSpan, <alignment;

  *new { arg object, rowSpan=1, columnSpan=1, align;
    ^super.new.initGridItem( object, rowSpan, columnSpan, align; );
  }

  *wrap { arg other;
     if( other.isKindOf( GridItem ) ) {^other} {^GridItem(other);}
  }

  initGridItem { arg o, rs, cs, a;
    object = o;
    rowSpan = rs;
    columnSpan = cs;
    alignment = a;
  }

  serialize { arg r, c; ^[object, r, c, rowSpan, columnSpan, QAlignment(alignment)] }
}

QGridLayout : QLayout {
  *new {
    ^super.new( "QcGridLayout" );
  }

  *parse { arg in, row, col;
    var i = 1;
    var out;
    if( in.isKindOf(Array) ) {
      out = [in[0],row,col,1,1,nil];
      while { i + 1 < in.size } {
        switch ( in[i],
          \rowSpan, { out[3] = in[i+1] },
          \colSpan, { out[4] = in[i+1] },
          \align, { out[5] = QAlignment(in[i+1]) }
        );
        i = i + 2;
      };
    }{
      out = GridItem.wrap(in).serialize(row,col);
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
    this.invokeMethod( \setAlignment, [item, QAlignment(align)], true );
  }

  setAlignmentAt { arg row, column, align;
    this.invokeMethod( \setAlignment, [row, column, QAlignment(align)], true );
  }
}
