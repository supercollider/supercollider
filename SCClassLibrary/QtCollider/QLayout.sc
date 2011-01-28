
LayoutItem {
  var <object;
  var <stretch;
  var <align;

  *new { arg object, stretch = 0, align = 0;
    ^super.new.initLayoutItem( object, stretch, align );
  }

  *wrap { arg other;
    if( other.isKindOf( LayoutItem ) ) {^other} {^LayoutItem.new(other)};
  }

  initLayoutItem { arg o, s, a;
    object = o;
    stretch = s;
    align = QView.alignmentDict[a];
  }

  serialized { ^[ object, stretch, align] }
}

StretchItem : LayoutItem {
  *new { arg factor = 0; ^super.new( 0, factor ) }
}

SpacerItem : LayoutItem {
  *new { arg size; ^super.new( size ); }
}

QLayout : QObject {

  *new { arg items, spacing, margins;
    var serializedItems = items.collect( { |x| LayoutItem.wrap(x).serialized } );
    ^super.new( this.layoutClass, [serializedItems, spacing, margins] )
          .initQLayout( items );
  }

  initQLayout { arg argItems;
  }

  spacing_ { arg spacing;
    this.setProperty( \spacing, spacing );
  }

  margins_ { arg margins;
    this.setProperty( \margins, margins );
  }

  add { arg item;
    var i = LayoutItem.wrap(item);
    this.invokeMethod( \addItem, [i.serialized] );
  }
}

QLineLayout : QLayout {

  *layoutClass { ^'' }
}

QHLayout : QLineLayout {
  *layoutClass { ^'QcHBoxLayout'; }
}

QVLayout : QLineLayout {
  *layoutClass { ^'QcVBoxLayout'; }
}
