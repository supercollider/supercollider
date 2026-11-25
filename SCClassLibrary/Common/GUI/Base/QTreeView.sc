TreeView : View {
	var <itemPressedAction;
	var <onItemChanged;

	*qtClass { ^'QcTreeWidget' }

	columns_ { |labels| this.setProperty(\columns, labels) }
	columns { ^this.getProperty(\columns) }
	numColumns { ^this.getProperty(\columnCount) }

	addItem { |strings|
		^this.invokeMethod(\addItem, [TreeViewItem(), strings]).prValidItem(this)
	}

	insertItem { |index, strings|
		^this.invokeMethod(\insertItem, [TreeViewItem(), index, strings]).prValidItem(this)
	}

	removeItem { |item| this.invokeMethod(\removeItem, item) }

	numItems { ^this.getProperty(\topLevelItemCount) }

	clear { this.invokeMethod(\clear) }

	currentItem {
		^this.getProperty(\currentItem).prValidItem(this)
	}

	currentItem_ { |item|
		this.setProperty(\currentItem, item ? TreeViewItem())
	}

	itemAt { |index|
		^this.invokeMethod(\item, [TreeViewItem(), index]).prValidItem(this)
	}

	canSort { ^this.getProperty(\sortingEnabled) }
	canSort_ { |bool| this.setProperty(\sortingEnabled, bool) }

	sort { |column, descending = false|
		this.invokeMethod(\sort, [column, descending])
	}

	columnWidth { |column|
		^this.invokeMethod(\columnWidth, [column])
	}

	setColumnWidth { |column, width|
		this.invokeMethod(\setColumnWidth, [column, width])
	}

	itemPressedAction_ { |action|
		if(itemPressedAction.notNil) {
			this.disconnectFunction('itemPressedAction()', itemPressedAction)
		};
		if(action.notNil) {
			this.connectFunction('itemPressedAction()', action)
		};
		itemPressedAction = action
	}

	onItemChanged_ { |hook|
		if(onItemChanged.notNil) {
			this.disconnectFunction('currentItemChanged()', onItemChanged)
		};
		if(hook.notNil) {
			this.connectFunction('currentItemChanged()', hook)
		};
		onItemChanged = hook
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

	background { ^this.palette.base }
	background_ { |color| this.palette = this.palette.base_(color) }

	// For compatibility with methods from TreeViewItem

	addChild { |strings|
		^this.addItem(strings)
	}

	insertChild { |index, strings|
		^this.insertItem(index, strings)
	}

	childAt { |index|
		^this.itemAt(index)
	}

	/////////// PRIVATE:

	prForEachColumnDataPair { |data, func|
		var n = this.numColumns;
		var i = 0;
		var d = [] ++ data;
		var nd = d.size - 1;
		var id;
		if(nd >= 0) {
			while { i < n } {
				id = i.wrap(0, nd);
				func.value(i, d[id]);
				i = i + 1
			}
		}
	}

	// dummy for convenience in case tree view is invalid
	prValidItem {
		^nil
	}
}

TreeViewItem {
	var qtObject;
	var <id;
	var finalizer;
	var <treeView;

	== { |other|
		^ other.class == TreeViewItem and: { id == other.id }
	}

	isNull { ^ id.isNil }

	parent {
		^treeView.invokeMethod(\parentItem, this).prValidItem(treeView)
	}

	index {
		var i = treeView.invokeMethod(\indexOfItem, this);
		if(i.isInteger) { ^i } { ^-1 }
	}

	childAt { |index|
		^treeView.invokeMethod(\item, [this, index]).prValidItem(treeView)
	}

	addChild { |strings|
		^treeView.invokeMethod(\addItem, [this, strings]).prValidItem(treeView)
	}

	insertChild { |index, strings|
		^treeView.invokeMethod(\insertItem, [this, index, strings]).prValidItem(treeView)
	}

	strings {
		^treeView.invokeMethod(\strings, this)
	}

	strings_ { |strings|
		strings.do { |string, column| this.setString(column, string) }
	}

	setString { |column, string|
		treeView.invokeMethod(\setText, [this, column, string])
	}

	colors_ { |colors|
		treeView.prForEachColumnDataPair(colors, { |column, color|
			this.setColor(column, color)
		})
	}

	setColor { |column, color|
		treeView.invokeMethod(\setColor, [this, column, color])
	}

	textColors_ { |textColors|
		treeView.prForEachColumnDataPair(textColors, { |column, color|
			this.setTextColor(column, color)
		})
	}

	setTextColor { |column, color|
		treeView.invokeMethod(\setTextColor, [this, column, color])
	}

	setView { |column, view|
		if(view.notNil) {
			treeView.invokeMethod(\setItemWidget, [this, column, view])
		} {
			this.removeView(column)
		}
	}

	removeView { |column|
		treeView.invokeMethod(\removeItemWidget, [this, column])
	}

	view { |column|
		^treeView.invokeMethod(\itemWidget, [this, column])
	}

	/////// PRIVATE:

	prValidItem { |treeView_|
		if(qtObject.notNil) {
			treeView = treeView_;
			^this
		} {
			^nil
		}
	}
}
