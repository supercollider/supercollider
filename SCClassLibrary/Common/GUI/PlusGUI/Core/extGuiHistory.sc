+ History {

	*makeWin { |where, numItems=8| ^current.makeWin(where, numItems) }

	makeWin { |where, numItems=8|
		var gui = HistoryGui(this, numItems);
		if (where.notNil) { gui.moveTo(where.x, where.y) };
		^gui
	}

	*document { current.document }

	document { |title = ""|
		var docTitle = title ++ Date.getDate.format("%Y-%m-%d-%Hh%M-History");
		Document.new(docTitle, this.storyString)
		//.path_(docTitle); //  path not working yet in Qt
		// - would be nice to set and show a path
		// where the doc can be saved later, by hand or automatically.
	}

}
