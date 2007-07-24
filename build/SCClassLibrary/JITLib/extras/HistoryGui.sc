HistoryGui { 
		// move to gui class. 
	classvar <w, <>docTitle = "History repeats", <>docHeight=120;
	
	var <history, <w, <startBut, <pop, <keyPop;
	var <doc, <oldDocs, <docFlag = \sameDoc; 
	
	var <filters, <filteredIndices, <filteredShorts, <filtering = false;
	var <filtBut, <keyPop, <textV, <filterBtn;
	var lastLineSelected = 0, lastLinesShown;
	 
	*new { |history, where| ^super.new.init(history, where ? (0@0)); }
	
	init { |inHist, where| 
		var sys = GUI.scheme, bounds;
		var closebut, listV, font;
		bounds = where @ (where + (300@300));
		
		font = sys.font.new("Osaka", 9);				////
		w = sys.window.new("History", bounds).front;	////
		history = inHist; 
	//	w.view.decorator = FlowLayout(w, w.bounds.moveTo(0,0), 0@0, 0@0);
		
		filters = [\all, ""]; 
		
			// to do: disable if history is not current!
		startBut = sys.button.new(w, Rect(0, 0, 80, 20)) ////
			.states_([ ["start"], ["end"]])
			.action_({ |btn|
				switch(btn.value, 
					0, { if (history == History.current) { History.end } }, 
					1, { if (history == History.current) { History.start } }
				);
			});

		pop = sys.popUpMenu.new(w, Rect(84,0,80,20))	////
			.items_([\sameDoc, \newDoc])
			.value_([\sameDoc, \newDoc].indexOf(docFlag))
			.action_({ |pop| docFlag = [\sameDoc, \newDoc][pop.value] });
		
		sys.button.new(w, Rect(228, 0, 60, 20)) ////
			.states_([["closeOld"]]).action_({ this.oldDocs.do(_.close) });
		sys.button.new(w, Rect(168, 0, 60, 20)) ////
			.states_([["closeAll"]]).action_({ 
				Document.allDocuments.do { |doc| if (doc.title == docTitle) { doc.close } };
			});
		
					// filter section
		sys.staticText.new(w, Rect(0, 22, 60, 20)).string_("Filter: ").align_(5); 
		
		filtBut = sys.button.new(w, Rect(62, 22, 20, 20)) ////
			.states_([["-"], ["X"]]).action_({ |btn| 
				this.filtering_(btn.value > 0);
				if (filtering) { this.filterLines };
				 history.hasMovedOn = true;
			});

		keyPop = sys.popUpMenu.new(w, Rect(84, 22, 60, 20))
			.items_([\all] ++ history.keys).value_(0)
			.action_({ |pop| this.setKeyFilter(pop.items[pop.value]) });

		textV = sys.textView.new(w, Rect(146,22,120,20)).string_("")
			.enterInterpretsSelection_(false);
		//	.action_({ |tx| this.setStrFilter(tx.string.postcs) }); // action is dead.
					
	//	w.view.decorator.nextLine; 
		listV = sys.listView.new(w,bounds.moveTo(2, 44).resizeBy(-4, -48))
			.font_(font)
			.items_([])
			.resize_(5)							////
			.background_(Color.grey(0.62))
			.action_({ |lview| 
				var index = lview.value;
				lastLineSelected = listV.items[index]; 
				if (filtering.not) { 
					this.postDoc(index, docFlag) 
				} {
					this.postDoc(filteredIndices[index], docFlag) 
				};
			})
			.enterKeyAction_({ this.doc.front });
		history.hasMovedOn = true;
		
		SkipJack({ 
			var newIndex, selectedLine, linesToShow, keys; 
			var newStr = textV.string; 
			if (textV.hasFocus and: (newStr != filters[1])) {
				this.setStrFilter(newStr);
			}; // clumsy, but textV has no usable action...
			
			if (history.hasMovedOn) { 
			//	"ever get here?".postln; 
				startBut.enabled_(history.isCurrent); 
				startBut.value_(History.started.binaryValue).refresh;
				
				filtBut.value_(filtering.binaryValue).refresh;
				if (textV.hasFocus.not) { textV.string_(filters[1]) };
				keys = [\all] ++ history.keys.asArray.sort;
				keyPop.items_(keys);
				keyPop.value_(keys.indexOf(filters[0]) ? 0);

						// remember old selection
				selectedLine = (lastLinesShown ? [])[listV.value]; 
				
				linesToShow = if (filtering.not) { 
					history.lineShorts.array.copy
				} { 
					this.filterLines; 
					filteredShorts;
				};
			//	[\linesToShow, linesToShow].postln; 
				
				if (linesToShow != lastLinesShown) { 
				//	"or updating listview here?".postln; 
					listV.items_(linesToShow); 
					lastLinesShown = linesToShow;
				}; 
				newIndex = if (selectedLine.isNil) { 0 }
					{ linesToShow.indexOf(selectedLine) };
				listV.value_(newIndex ? 0);

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
			textV.string_(filters[1]);
		};
		if (filtering) { history.hasMovedOn = true; };
	}
	
	postDoc { |index, str| 
		var line = history.lines[index]; 
		
		if (line.isNil) { "no history yet!".inform; ^this };
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
	findDoc { 
		if (docFlag == \newDoc) { oldDocs = oldDocs.add(doc) }; 
		if (docFlag == \newDoc or: doc.isNil or: { Document.allDocuments.includes(doc).not }) { 
			doc = Document(docTitle).bounds_(Rect(300, 500, 300, 100));
		};	
		oldDocs = oldDocs.select {|d| d.notNil and: { d.dataptr.notNil } };
	} 
}