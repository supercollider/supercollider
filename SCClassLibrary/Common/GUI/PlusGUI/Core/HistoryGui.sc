HistoryGui {

	classvar <w, <>docTitle = "History repeats", <>docHeight=120;

	var <history, <w, <textV;
	var <startBut, <filtBut, <filTextV, <filtBut, <keyPop, <topBut;
	var <doc, <oldDocs, <docFlag = \sameDoc, <>stickMode=0;

	var <filters, <filteredIndices, <filteredShorts, <filtering = false;
	var lastLineSelected = 0, lastLinesShown;

	*new { |history, where, numTextLines=12|
		^super.new.init(history, where ? (0@0), numTextLines);
	}

	init { |inHist, where, numTextLines=12|
		var bounds;
		var closebut, listV, font, flow;
		bounds = where @ (where + (300@400));

		font = Font.monospace(9);	////
		w = Window("History", bounds).front;	////
		flow = w.addFlowLayout(2@2, 1@1);
		history = inHist;

		filters = [\all, ""];

		textV = TextView(w, Rect(0,0, 300 - 4, numTextLines * 12)).string_("")
			.enterInterpretsSelection_(false)
			.keyDownAction_({ |txvw, char, mod, uni, keycode|
				// char.postcs;
				if ([3, 13].includes(char.ascii)) {
					this.rip(textV.string);
				};
			})
			.resize_(2);

			// to do: disable if history is not current!
		startBut = Button(w, Rect(0, 0, 50, 20)) ////
			.states_([ ["start"], ["end"]])
			.canFocus_(false)
			.action_({ |btn|
				switch(btn.value,
					0, { if (history == History.current) { History.end } },
					1, { if (history == History.current) { History.start } }
				);
			});

		filtBut = Button(w, Rect(0, 0, 32, 20)) ////
			.canFocus_(false)
			.states_([["all"], ["filt"]]).action_({ |btn|
				this.filtering_(btn.value > 0);
				if (filtering) { this.filterLines };
				 history.hasMovedOn = true;
			});

		keyPop = PopUpMenu(w, Rect(0, 0, 40, 20))
			.items_([\all] ++ history.keys).value_(0)
			.action_({ |pop| this.setKeyFilter(pop.items[pop.value]) });

		filTextV = TextView(w, Rect(0,0, 88, 20)).string_("")
			.enterInterpretsSelection_(false)
			.resize_(2)
			.keyDownAction_({ |txvw, char, mod, uni, keycode|
				this.setStrFilter(txvw.string);
				if (this.filtering) { this.filterLines; }
			});
		topBut = Button(w, Rect(0, 0, 32, 20))
			.states_([["top"], ["keep"]]).value_(0)
			.resize_(3)
			.canFocus_(false)
			.action_({ |but| this.stickMode_(but.value) });

		Button(w, Rect(0, 0, 32, 20)) ////
			.states_([["rip"]])
			.resize_(3)
			.canFocus_(false)
			.action_({ |btn| this.rip(textV.string) });

		Button(w, Rect(0,0, 16, 20))
			.states_([["v"], ["^"]])
			.resize_(3)
			.action_ { |btn|
				var views = w.view.children;
				var resizes = [
					[2, 1, 1, 1, 2, 3, 3, 3, 5],
					[5, 7, 7, 7, 8, 9, 9, 9, 8]
				][btn.value.asInteger];

				views.do { |v, i| v.resize_(resizes[i]) };

			};

		listV = ListView(w, bounds.copy.insetBy(2).height_(230))
			.font_(font)
			.items_([])
			.resize_(5)
			.background_(Color.grey(0.62))
			.action_({ |lview|
				var index = lview.value;
				if (lview.items.isEmpty) {
					"no entries yet.".postln;
				} {
					lastLineSelected = listV.items[index];
					if (filtering.not) {
						this.postInlined(index)
					} {
						this.postInlined(filteredIndices[index])
					}
				}
			})
			.enterKeyAction_({ |lview|
				var index = lview.value;
				if (filtering) { index = filteredIndices[index] };
				try {
					history.lines[index][2].postln.interpret.postln;
				//	"did execute.".postln;
				} {
					"execute line from history failed.".postln;
				};
			});
		history.hasMovedOn = true;

		SkipJack({
			var newIndex, selectedLine, linesToShow, keys;
			var newStr = filTextV.string;
			if (filTextV.hasFocus and: (newStr != filters[1])) {
				this.setStrFilter(newStr);
			}; // clumsy, but filTextV has no usable action...

			if (history.hasMovedOn) {
				startBut.enabled_(history.isCurrent);
				startBut.value_(History.started.binaryValue).refresh;

				filtBut.value_(filtering.binaryValue).refresh;
				if (filTextV.hasFocus.not) { filTextV.string_(filters[1]) };
				keys = [\all] ++ history.keys.asArray.sort;
				keyPop.items_(keys);
				keyPop.value_(keys.indexOf(filters[0]) ? 0);
				if (stickMode == 1) {
						// remember old selection
					selectedLine = (lastLinesShown ? [])[listV.value];
				} { };

				linesToShow = if (filtering.not) {
					history.lineShorts.array.copy
				} {
					this.filterLines;
					filteredShorts;
				} ? [];

				if (linesToShow != lastLinesShown) {
				//	"or updating listview here?".postln;
					listV.items_(linesToShow);
					lastLinesShown = linesToShow;
				};
				newIndex = if (selectedLine.isNil) { 0 }
					{ linesToShow.indexOf(selectedLine) };
				listV.value_(newIndex ? 0);
				if(stickMode == 0) { listV.action.value(listV) };
				history.hasMovedOn = false;
			};
		}, 1, { w.isClosed }, "histwin");
	}
	setKeyFilter { |key| filters.put(0, key); this.filterLines; }
	setStrFilter { |str| filters.put(1, str); this.filterLines; }

	filtering_ { |flag=true|
		 filtering = flag;
		 history.hasMovedOn_(true);
	}
	filterOn { this.filtering_(true) }
	filterOff { this.filtering_(false) }

	filterLines {
		filteredIndices = history.indicesFor(*filters);
		filteredShorts = history.lineShorts[filteredIndices];
		defer {
			keyPop.value_(keyPop.items.indexOf(filters[0] ? 0));
			filTextV.string_(filters[1]);
		};
		if (filtering) { history.hasMovedOn = true; };
	}
	postInlined { |index|
		var line;
		if (history.lines.isNil) { "no history lines yet.".postln; ^this };
		line = history.lines[index];
		if (line.isNil) { "history: no line found!".inform; ^this };
		textV.string_(line[2]);
	}
	postDoc { |index|
		var line;
		if (history.lines.isNil) { "no history lines yet.".postln; ^this };
		line = history.lines[index];
		if (line.isNil) { "history: no line found!".inform; ^this };
		this.findDoc;
		doc.string_(line[2]).front;
		try { this.alignDoc };
		w.front;
	}
	alignDoc {
		var docbounds, winbounds;
		docbounds = doc.bounds;
		winbounds = w.bounds;
		doc.bounds_(
			Rect(
				winbounds.left,
				winbounds.top + winbounds.height + 24,
				winbounds.width,
				docHeight
			)
		)
	}

	rip {
		this.findDoc; doc.view.children.first.string_(textV.string); doc.front;
	}

	findDoc {
		if (docFlag == \newDoc) { oldDocs = oldDocs.add(doc) };
		if (docFlag == \newDoc or: doc.isNil or: { Window.allWindows.includes(doc).not }) {
			doc = Window(docTitle, Rect(300, 500, 300, 100));
			doc.addFlowLayout;
			TextView(doc, doc.bounds.resizeBy(-8, -8)).resize_(5);
		};
		oldDocs = oldDocs.select {|d| d.notNil and: { d.dataptr.notNil } };
	}
}
