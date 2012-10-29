QTreeView : QView {
  var <itemPressedAction;
  var <onItemChanged;

  *qtClass { ^'QcTreeWidget' }

  columns_ { arg labels; this.setProperty( \columns, labels ); }
  columns { ^this.getProperty( \columns ); }
  numColumns { ^this.getProperty( \columnCount ); }

  addItem { arg strings;
    ^this.invokeMethod( \addItem, [QTreeViewItem(),strings] ).prValidItem(this);
  }

  insertItem { arg index, strings;
    ^this.invokeMethod( \insertItem, [QTreeViewItem(),index, strings] ).prValidItem(this);
  }

  removeItem { arg item; this.invokeMethod( \removeItem, item ); }

  numItems { ^this.getProperty( \topLevelItemCount ); }

  clear { this.invokeMethod(\clear); }

  currentItem {
    ^this.getProperty( \currentItem ).prValidItem(this);
  }

  currentItem_ { arg item;
    this.setProperty( \currentItem, item ? QTreeViewItem() );
  }

  itemAt { arg index;
    ^this.invokeMethod( \item, [QTreeViewItem(), index] ).prValidItem(this);
  }

  canSort { ^this.getProperty( \sortingEnabled ) }
  canSort_ { arg bool; this.setProperty( \sortingEnabled, bool ) }

  sort { arg column, descending = false;
    this.invokeMethod( \sort, [column, descending] )
  }

  itemPressedAction_ { arg action;
    if(itemPressedAction.notNil) {
      this.disconnectFunction( 'itemPressedAction()', itemPressedAction );
    };
    if(action.notNil) {
      this.connectFunction( 'itemPressedAction()', action );
    };
    itemPressedAction = action;
  }

  onItemChanged_ { arg hook;
    if(onItemChanged.notNil) {
      this.disconnectFunction( 'currentItemChanged()', onItemChanged );
    };
    if(hook.notNil) {
      this.connectFunction( 'currentItemChanged()', hook );
    };
    onItemChanged = hook;
  }

/*
  NOTE:
  These methods can only operate on top level items
  Should we include them?

  value {
    var i = this.currentItem.index;
    if( i < 0 ) { ^nil } { ^i };
  }

  value_ { arg index;
    var item;
    if( index.notNil ) {
      this.currentItem = this.itemAt(index);
    } {
      this.currentItem = nil;
    };
  }
*/

  background { ^this.palette.base; }
  background_ { arg color; this.palette = this.palette.base_(color) }

/////////// PRIVATE:

  prForEachColumnDataPair { arg data, func;
    var n = this.numColumns;
    var i = 0;
    var d = [] ++ data;
    var nd = d.size - 1;
    var id;
    if( nd >= 0 ) {
      while { i < n } {
        id = i.wrap(0,nd);
        func.value( i, d[id] );
        i = i + 1;
      };
    };
  }

  // dummy for convenience in case tree view is invalid
  prValidItem {
    ^nil;
  }
}

QTreeViewItem {
  var qtObject;
  var <id;
  var finalizer;
  var <treeView;

  == { arg other;
    ^ other.class == QTreeViewItem and: {id == other.id}
  }

  isNull { ^ id.isNil }

  parent {
    ^treeView.invokeMethod( \parentItem, this ).prValidItem(treeView);
  }

  index {
    var i = treeView.invokeMethod( \indexOfItem, this );
    if(i.isInteger) {^i} {^-1};
  }

  childAt { arg index;
    ^treeView.invokeMethod( \item, [this, index] ).prValidItem(treeView);
  }

  addChild { arg strings;
    ^treeView.invokeMethod( \addItem, [this,strings] ).prValidItem(treeView);
  }

  insertChild { arg index, strings;
    ^treeView.invokeMethod( \insertItem, [this,index,strings] ).prValidItem(treeView);
  }

  strings {
    ^treeView.invokeMethod( \strings, this );
  }

  strings_ { arg strings;
    strings.do { |string, column| this.setString(column,string) };
  }

  setString { arg column, string;
    treeView.invokeMethod( \setText, [this,column,string] );
  }

  colors_ { arg colors;
    treeView.prForEachColumnDataPair( colors, {
      |column,color| this.setColor(column,color);
    } );
  }

  setColor { arg column, color;
    treeView.invokeMethod( \setColor, [this,column,color] );
  }

  textColors_ { arg textColors;
    treeView.prForEachColumnDataPair( textColors, {
      |column,color| this.setTextColor(column,color);
    } );
  }

  setTextColor { arg column, color;
    treeView.invokeMethod( \setTextColor, [this,column,color] );
  }

  setView { arg column, view;
    if( view.notNil ) {
      treeView.invokeMethod( \setItemWidget, [this, column, view] );
    } {
      this.removeView( column );
    };
  }

  removeView { arg column;
    treeView.invokeMethod( \removeItemWidget, [this, column] );
  }

  view { arg column;
    ^treeView.invokeMethod( \itemWidget, [this,column] );
  }

  /////// PRIVATE:

  prValidItem { arg treeView_;
    if( qtObject.notNil ) {
      treeView = treeView_;
      ^this;
    } {
      ^nil
    };
  }
}
