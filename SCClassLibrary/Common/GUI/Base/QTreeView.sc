TreeView : View {
	var <onItemExpanded, <onItemCollapsed, <onItemPressed, <onItemClicked, <onCurrentItemChanged;

	*qtClass { ^'QcTreeWidget' }

	columns_ { arg labels; this.setProperty( \columns, labels ); }
	columns { ^this.getProperty( \columns ); }
	numColumns { ^this.getProperty( \columnCount ); }

	addItem { arg strings;
		^this.invokeMethod( \addItem, [TreeViewItem(),strings] ).prValidItem(this);
	}

	insertItem { arg index, strings;
		^this.invokeMethod( \insertItem, [TreeViewItem(),index, strings] ).prValidItem(this);
	}

	removeItem { arg item; this.invokeMethod( \removeItem, item ); }

	numItems { ^this.getProperty( \topLevelItemCount ); }

	clear { this.invokeMethod(\clear); }

	currentItem {
		^this.getProperty( \currentItem ).prValidItem(this);
	}

	currentItem_ { arg item;
		this.setProperty( \currentItem, item ? TreeViewItem() );
	}

	itemAt { arg index;
		^this.invokeMethod( \item, [TreeViewItem(), index] ).prValidItem(this);
	}

	canSort { ^this.getProperty( \sortingEnabled ) }
	canSort_ { arg bool; this.setProperty( \sortingEnabled, bool ) }

	sort { arg column, descending = false;
		this.invokeMethod( \sort, [column, descending] )
	}

	columnWidth { arg column;
		^this.invokeMethod( \columnWidth, [column] )
	}

	setColumnWidth { arg column, width;
		this.invokeMethod( \setColumnWidth, [column, width] )
	}

	expandItemAt { arg index;
		this.expandItem( this.itemAt(index) );
	}

	collapseItemAt { arg index;
		this.collapseItem( this.itemAt(index) );
	}

	expandItem { arg item;
		this.invokeMethod( \expandItem, [item] )
	}

	collapseItem { arg item;
		this.invokeMethod( \collapseItem, [item] )
	}

	// SIGNALS
	onItemPressed_{ arg action;
		this.manageFunctionConnection( onItemPressed, action, 'itemPressed(QcTreeWidget::ItemPtr, int)' );
		onItemPressed = action;
	}

	itemPressedAction{ ^onItemPressed }
	itemPressedAction_{ arg action;
		^this.onItemPressed_(action)
	}

	onItemClicked_{ arg action;
		this.manageMethodConnection( onItemClicked, action, 'itemClicked(QcTreeWidget::ItemPtr, int)', 'doItemClicked' );
		this.manageMethodConnection( onItemClicked, action, 'itemDoubleClicked(QcTreeWidget::ItemPtr, int)', 'doItemDoubleClicked'  );
		onItemClicked = action;
	}

	doItemClicked { arg item, column;
		onItemClicked.value(this, item, column, 1 /* single click*/);
	}

	doItemDoubleClicked { arg item, column;
		onItemClicked.value(this, item, column, 2 /* double click*/);
	}

	scrollToItem { arg item, behavior;
		this.invokeMethod( \scrollToItem, [item, QScrollHint(behavior)] )
	}

	action_{ arg newAction;
		this.manageFunctionConnection( action, newAction, 'action(QcTreeWidget::ItemPtr, int)' );
		action = newAction;
	}

	onItemExpanded_{ arg action;
		this.manageFunctionConnection( onItemExpanded, action, 'itemExpanded(QcTreeWidget::ItemPtr)');
		onItemExpanded = action;
	}

	onItemCollapsed_{ arg action;
		this.manageFunctionConnection( onItemCollapsed, action, 'itemCollapsed(QcTreeWidget::ItemPtr)');
		onItemCollapsed = action;
	}

	onCurrentItemChanged_{ arg action;
		this.manageFunctionConnection( onCurrentItemChanged, action, 'currentItemChanged(QcTreeWidget::ItemPtr, QcTreeWidget::ItemPtr)');
		onCurrentItemChanged = action;
	}

	onItemChanged_ { arg action;
		^this.onCurrentItemChanged_(action);
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

	// For compatibility with methods from TreeViewItem

	addChild { arg strings;
		^this.addItem(strings);
	}

	insertChild { arg index, strings;
		^this.insertItem(index, strings);
	}

	childAt { arg index;
		^this.itemAt(index);
	}

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

TreeViewItem {
	var qtObject;
	var <id;
	var finalizer;
	var <treeView;

	== { arg other;
		^ other.class == TreeViewItem and: {id == other.id}
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

	expand {
		treeView.expandItem(this);
	}

	collapse {
		treeView.collapseItem(this);
	}

	scrollToItem {
		|behavior|
		treeView.scrollToItem(this, behavior);
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
