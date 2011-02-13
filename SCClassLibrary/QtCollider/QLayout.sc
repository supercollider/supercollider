QLayout : QObject {
  spacing_ { arg spacing;
    this.setProperty( \spacing, spacing );
  }

  margins_ { arg margins;
    this.setProperty( \margins, margins );
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
    align = QAlignment(a);
  }

  serialized { ^[ object, stretch, align] }
}

StretchItem : LineItem {
  *new { arg factor = 0; ^super.new( 0, factor ) }
}

SpacerItem : LineItem {
  *new { arg size; ^super.new( size ); }
}

QLineLayout : QLayout {
  *new { arg ...items;
    var serializedItems = items.collect( { |x| LineItem.wrap(x).serialized } );
    ^super.new( this.layoutClass, serializedItems );
  }

  *layoutClass { ^'' }

  add { arg item;
    var i = LineItem.wrap(item);
    this.invokeMethod( \addItem, [i.serialized] );
  }
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
}

QGridLayout : QLayout {
  *new {
    ^super.new( "QcGridLayout" );
  }

  *rows { arg ...rows ;
    var grid;
    var gi;
    grid = this.new;
    rows.do { |row, r|
      if( row.size > 0 ) {
        row.do { |item, c|
          if( item.notNil ) {
            gi = GridItem.wrap(item);
            grid.addSpanning( gi.object, r, c,
                              gi.rowSpan, gi.columnSpan,
                              gi.alignment );
          };
        };
      };
    }
    ^grid;
  }

  *columns { arg ...cols;
    var grid;
    var gi;
    grid = this.new;
    cols.do { |col, c|
      if( col.size > 0 ) {
        col.do { |item, r|
          if( item.notNil ) {
            gi = GridItem.wrap(item);
            grid.addSpanning( gi.object, r, c,
                              gi.rowSpan, gi.columnSpan,
                              gi.alignment );
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
}
