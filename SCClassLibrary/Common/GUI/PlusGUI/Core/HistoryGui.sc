
HistoryGui : JITGui {

	classvar <>docTitle = "History repeats", <>docHeight=120;

	var <textV;
	var <startBut, <filtBut, <filTextV, <keyPop, <topBut, <listV;
	var <doc, <oldDocs, <docFlag = \sameDoc, <>stickMode=0;

	var <filters, <filteredIndices, <filteredShorts, <filtering = false;
	var lastLineSelected = 0, lastLinesShown;

	*new { |object, numItems = 8, parent, bounds, makeSkip = true, options = #[]|
		^super.newCopyArgs(nil, numItems, parent, bounds)
		.init(makeSkip, options)
		.object_(object);
	}

	accepts { |obj| ^obj.isNil or: { obj.isKindOf(History) } }

	setDefaults { |options|
		if (parent.isNil) {
			defPos = 10@260
		} {
			defPos = skin.margin;
		};
		minSize = 300 @ 400;	// calculate later
		minSize = 300 @ (
			(numItems * skin.buttonHeight) // textView
			+ (skin.buttonHeight + (skin.margin.y * 2)) // buttonline
			+ (numItems * skin.buttonHeight * 1.62).round(1)
		);
	}

	makeViews { |options|
		var font, flow;
		var textViewHeight = numItems * skin.buttonHeight;
		var listViewHeight = textViewHeight * 1.6;

		font = Font(Font.defaultSansFace, 9);
		flow = zone.addFlowLayout(2@2, 1@1);

		zone.resize_(5);

		// this should move to JITGui,
		// as it is useful for many JITGuis.
		if (hasWindow.not) {
			nameView = DragBoth(zone, Rect(0,0, zone.bounds.width - 4, skin.buttonHeight))
			// .font_(skin.font)
			.align_(\center)
			.receiveDragHandler_({ arg obj; this.object = View.currentDrag });
			listViewHeight = listViewHeight - (skin.buttonHeight + 2);
		};

		this.name_("History");

		filters = [\all, ""];

		textV = TextView(zone, Rect(0, 0, (zone.bounds.width - 4), textViewHeight))
		.string_("")
		.enterInterpretsSelection_(false)
		.keyDownAction_({ |txvw, char, mod, uni, keycode|
			char !? {
				if ([3, 13].includes(char.ascii)) {
					this.rip(textV.string);
				};
			};
		})
		.resize_(2);

		// to do: disable if history is not current!
		startBut = Button(zone, Rect(0, 0, 40, 20)) ////
		.states_([ ["start"], ["end"]])
		.canFocus_(false)
		.action_({ |btn, mod|
			switch(btn.value,
				0, {
					if (object == History.current) {
						if (mod.isAlt) {
							History.end
						} {
							"please alt-click to end history.".postln;
							btn.value_(1);
						}
				} },
				1, { if (object == History.current) { History.start } }
			);
		});

		filtBut = Button(zone, Rect(0, 0, 32, 20)) ////
		.canFocus_(false)
		.states_([["all"], ["filt"]]).action_({ |btn|
			this.filtering_(btn.value > 0);
			if (filtering) { this.filterLines };
			object.hasMovedOn = true;
		});

		keyPop = PopUpMenu(zone, Rect(0, 0, 50, 20))
		.items_([\all]).value_(0)
		.action_({ |pop| this.setKeyFilter(pop.items[pop.value]) });

		filTextV = TextView(zone, Rect(0,0, 80 + 12, 20)).string_("")
		.enterInterpretsSelection_(false)
		.resize_(2)
		.keyDownAction_({ |txvw, char, mod, uni, keycode|
			defer ({
				this.setStrFilter(txvw.string);
				if (this.filtering) {
					this.updateLines;
				};
			}, 0.01)
		});

		topBut = Button(zone, Rect(0, 0, 32, 20))
		.states_([["top"], ["keep"]]).value_(0)
		.resize_(3)
		.canFocus_(false)
		.action_({ |but| this.stickMode_(but.value) });

		Button(zone, Rect(0, 0, 32, 20)) ////
		.states_([["rip"]])
		.resize_(3)
		.canFocus_(false)
		.action_({ |btn| this.rip(textV.string) });

		// on resize, keep textview or listview at equal height:
		Button(zone, Rect(0,0, 16, 20))
		.states_([["v"], ["^"]])
		.resize_(3)
		.action_ { |btn|
			var index = btn.value.asInteger;
			var constView = ["textV", "listV"][index];
			var views = zone.children;
			var resizes = [
				[2, 1, 1, 1, 2, 3, 3, 3, 5],
				[5, 7, 7, 7, 8, 9, 9, 9, 8]
			][index];
			"% resize: keep % height constant.\n".postf(this, constView);
			views.do { |v, i| v.resize_(resizes[i]) };
		};

		listV = ListView(zone, bounds.copy.insetBy(2).height_(listViewHeight))
		.font_(font)
		.items_([])
		.resize_(5)
		.background_(Color.grey(0.62))
		.action_({ |lview|
			var index = lview.value;
			if (lview.items.notEmpty) {
				lastLineSelected = listV.items[index];
				if (filtering.not) {
					this.showLineAt(index)
				} {
					this.showLineAt(filteredIndices[index])
				}
			}
		})
		.enterKeyAction_({ |lview|
			var index = lview.value;
			if (filtering) { index = filteredIndices[index] };
			object.evaluateLineAt(index);
		});
		this.checkUpdate;
	}

	getState {
		var newState;
		if (object.isNil) { ^(hasMovedOn: false) };

		newState = (
			object: object,
			hasMovedOn: object.hasMovedOn,
			isCurrent: object.isCurrent,
			started: History.started,

			filtStr: filTextV.string,
			numLines: object.lines.size,
			filtering: filtering
		);
		^newState
	}

	resetViews {
		[startBut, filtBut, keyPop].do(_.value_(0));
		textV.string = "";
		filTextV.string = "";
		listV.items = [];
	}

	// these three should move to JITGui in general,
	// to simplify the checkUpdate methods
	updateFunc { |newState, key, func, post = false|
		var val = newState[key];
		if (val != prevState[key]) { func.value(val) }
	}

	updateVal { |newState, key, guiThing, post = false|
		var val = newState[key];
		if (val != prevState[key]) { guiThing.value_(val).refresh }
	}

	updateBinVal { |newState, key, guiThing, post = false|
		var val = newState[key];
		if (val != prevState[key]) { guiThing.value_(val.binaryValue).refresh }
	}

	updateLines {
		var linesToShow, newIndex, selectedLine;

		// remember old selection
		if (stickMode == 1) {
			selectedLine = (lastLinesShown ? [])[listV.value];
		} {
			// something else here?
		};
		linesToShow = if (filtering.not) {
			object.lineShorts.array
		} {
			this.filterLines;
			filteredShorts;
		} ? [];

		listV.items_(linesToShow);
		lastLinesShown = linesToShow;

		newIndex = if (selectedLine.isNil) { 0 }
		{ linesToShow.indexOf(selectedLine) };
		listV.value_(newIndex ? 0);
		if(stickMode == 0) { listV.action.value(listV) };

		this.showLineAt(newIndex);
	}

	checkUpdate {
		var newState = this.getState;
		var keys;

		///	if (newState == prevState) { ^this };

		this.updateFunc(newState, \object, { |obj| zone.enabled_(obj.notNil) });

		if (newState[\object].isNil) {
			this.resetViews;
			prevState = newState;
			^this
		};

		this.updateFunc(newState, \isCurrent, { |val| startBut.enabled_(val.binaryValue) });
		this.updateBinVal(newState, \started, startBut);
		this.updateBinVal(newState, \filtering, filtBut);

		// // clumsy, but filTextV has no usable action...
		// if (filTextV.hasFocus and: (newState[\filtStr] != filters[1])) {
		// 	this.setStrFilter(newState[\filtStr]);
		// };

		// could be factored a bit more
		if (newState[\hasMovedOn] or: {
			newState[\numLines] != prevState[\numLines] }) {

			keys = [\all] ++ object.keys.asArray.sort;
			keyPop.items_(keys);
			keyPop.value_(keys.indexOf(filters[0]) ? 0);

			this.updateLines;

			object.hasMovedOn = false;
		};

		prevState = newState;
	}

	setKeyFilter { |key|
		filters.put(0, key);
		object.hasMovedOn_(true);
		if (filtering) { this.filterLines; };
	}
	setStrFilter { |str|
		filters.put(1, str);
		object.hasMovedOn_(true);
		if (filtering) { this.filterLines; };
	}

	filtering_ { |flag=true|
		filtering = flag;
		object.hasMovedOn_(true);
		if (filtering) { this.filterLines; };
		this.updateLines;
	}
	filterOn { this.filtering_(true) }
	filterOff { this.filtering_(false) }

	filterLines {
		filteredIndices = object.indicesFor(*filters);
		filteredShorts = object.lineShorts[filteredIndices];
		defer {
			keyPop.value_(keyPop.items.indexOf(filters[0] ? 0));
			// filTextV.string_(filters[1]);
		};
		if (filtering) { object.hasMovedOn = true; };
	}

	showLineAt { |index|
		var line;
		if (object.lines.isNil) { ^this };
		line = object.lines[index];
		if (line.isNil) { ^this };
		textV.string_(line[2]);
	}

	postDoc { |index|
		var line;
		if (object.lines.isNil) { ^this };
		line = object.lines[index];
		if (line.isNil) { ^this };
		this.setDocStr(line[2]);
		doc.front;
		try { this.alignDoc };
		// w.front;
	}
	setDocStr { |str|
		this.findDoc;
		doc.view.children.first.string_(str);
	}

	alignDoc {
		var docbounds, winbounds;
		this.findDoc;
		docbounds = doc.bounds;
		winbounds = parent.bounds;
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
		this.setDocStr(textV.string); doc.front;
		try { doc.align };
	}

	findDoc {
		if (docFlag == \newDoc) { oldDocs = oldDocs.add(doc) };
		if (docFlag == \newDoc or: doc.isNil or: { Window.allWindows.includes(doc).not }) {
			doc = Window(docTitle, Rect(300, 500, 314, 114));
			doc.addFlowLayout;
			TextView(doc, doc.bounds.resizeBy(-8, -8)).resize_(5);
		};
		oldDocs = oldDocs.select {|d| d.notNil and: { d.dataptr.notNil } };
	}
}
