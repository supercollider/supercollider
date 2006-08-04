History { 		// adc 2006, Birmingham. 

	classvar <>forwardFunc, <date, <startTimeStamp, <time0;
	classvar <lines, <lineShorts;
	classvar <>title = "History repeats";
	classvar <w, <bounds, <doc, pop, startbut;
	classvar <docFlag = \sameDoc, <hasMovedOn = true;
	classvar <>verbose = false, <>recordLocally = true;
	classvar <>saveFolder = "~/Desktop/", <>recordLocally = true;
	
	*initClass { 
		bounds = Rect(0, 0, 300, 200); 
		this.clear;
	}
	*clear { 
		lines = List(1024);
		lineShorts = List(1024); 
	}
			// replace (\n ..." with (... ))!!!
	*start { 
		var interp = thisProcess.interpreter;
		interp.codeDump = { |str, val, func| 
			if ( func.notNil 
					and: { str.notEmpty } 
					and: { str != "\n" } 
					and: { str.keep(7) != "History" }) 
				{ 
					if (History.verbose, { [str, val, func].postcs });
					if (History.recordLocally, { 
						History.enter(thisProcess.interpreter.cmdLine) 
					});
					History.forwardFunc.value(str, val, func);
				}
		};
		hasMovedOn = true;
	}
	*stop { 
		thisProcess.interpreter.codeDump = nil; 
		hasMovedOn = true;
	}

	*localOn { recordLocally = true }
	*localOff { recordLocally = false }
	
	*network { }
	
	*makeWin { 
		var sys = GUI.scheme;						////
		var closebut, v, f, font;
		
		font = sys.font.new("Osaka", 9);				////
		w = sys.window.new("History", bounds).front;	////
	//	w.view.decorator = FlowLayout(w, w.bounds.moveTo(0,0), 0@0, 0@0);
		
		startbut = sys.button.new(w, Rect(0, 0, 80, 20)) ////
			.states_([ ["start"], ["stop"]])
			.action_({ |btn|
				switch(btn.value, 
					0, { this.stop }, 
					1, { this.start }
				);
			});

		pop = sys.popUpMenu.new(w, Rect(84,0,80,20))	////
			.items_([\sameDoc, \newDoc])
			.value_([\sameDoc, \newDoc].indexOf(docFlag))
			.action_({ |pop| docFlag = [\sameDoc, \newDoc][pop.value] });
		
		closebut = sys.button.new(w, Rect(168, 0, 80, 20)) ////
			.states_([["closeAll"]]).action_({ 
				Document.allDocuments.do { |doc| if (doc.title == title) { doc.close } };
			});
		
	//	w.view.decorator.nextLine; 
		v = sys.listView.new(w,bounds.moveTo(0, 20))	////
			.font_(font)
			.items_([])
		//	.resize_(5)							////
			.background_(Color.grey(0.62))
			.action_({ |entry| 
				var val = entry.value; 
				this.postDoc(val, docFlag) 
			})
			.enterKeyAction_({ History.doc.front });
		hasMovedOn = true;
		
		SkipJack({ 
			if (hasMovedOn) { 
				v.items_(lineShorts.array).value_(0);
				if (this.hasMovedOn, { v.value_(0) });
				startbut.value_(thisProcess.interpreter.codeDump.notNil.binaryValue).refresh;
				hasMovedOn = false;
			};
		}, 1, { w.isClosed }, "histwin");
		
		^w;
	}
	*postDoc { |index, str| 
		var line = lines[index]; 
		if (line.isNil) { "no history yet!".inform; ^this };
		this.findDoc; 
		doc.string_(line[2]).front;
		w.front;
	}
	*findDoc {Ê
		if (doc.isNil or: { Document.allDocuments.includes(doc).not } or: { pop.value == 1 }) { 
			doc = Document(title).bounds_(Rect(300, 500, 300, 100));
		};	
	} 

	*document {
		var alone = lines.flop[1].as(IdentitySet).size == 1;
		var str, d, date = Date.getDate;
		
		str = "///////////////////////////////////////////////////\n";
		str = str ++ format("// History, as it was on %.\n", date);
		str = str ++ "///////////////////////////////////////////////////\n\n";
		
		lines.reverseDo { |x|
			var now, id, cmdLine;
			#now, id, cmdLine = x;
			if(cmdLine.beginsWith("//").not) {
				str = str ++ 
				format("// - % - % \n", 
					this.formatTime(now), 
					if(alone) { "" } { "(" ++ id ++ ")" }
				);
			};
			str = str ++ cmdLine ++ "\n\n";
		};
		
		d = Document.new("History: " ++ date, str);
		d.path = d.title; // don't loose title.
	}

	*formatTime { arg val;
			var h, m, s;
			h = val div: (60 * 60);
			val = val - (h * 60 * 60);
			m = val div: 60;
			val = val - (m * 60);
			s = val;
			^"%:%:%".format(h, m, s.round(0.1))
	}
	
	*prettyString { |str| 
		// remove returns at beginning or end of the string
		var startIndex = str.detectIndex({ |ch| ch != $\n });
		var endChar = str.last; 
		var lastCharIndex = str.lastIndex; 
		while { endChar == $\n } { 
			lastCharIndex = lastCharIndex - 1; 
			endChar = str[lastCharIndex];
		};
		// [startIndex, lastCharIndex].postln;
		^str.copyRange(startIndex, lastCharIndex);
	}
	
	*enter { |lineStr, id=\me|
		var dateNow, now; 
		lineStr = this.prettyString(lineStr); 
		
		if (lineStr.isEmpty) { ^this };	// nothing worth remembering
		
		if (lines.isEmpty) { 			// get startTime if first entry
			dateNow = Date.getDate;
			startTimeStamp = dateNow.stamp;
			date = dateNow.asString;
			time0 = Main.elapsedTime;
		}; 
		hasMovedOn = true;
		now = (Main.elapsedTime - time0);
//		lineStr.postcs;
//		if (lineStr.first == $\n) { lineStr.drop(1) };
//		if (lineStr.first == $\n) { lineStr.drop(-1) };
//		lineStr.postcs;
		
		lines.addFirst([ now, id, lineStr ]);
		lineShorts.addFirst( "" + now.round(1) + "-" + id + "-" + lineStr.keep(50) );
	}
	
	*removeAt { |index| 
		if (index.isKindOf(Collection)) { index.do (this.removeAt(_)); ^this };
		
		if (index < lines.size) { 		// ignore out of range indices, keep lists synced.
			[lines, lineShorts].do(_.removeAt(index));		};
		hasMovedOn = true;
	}
	*keep { |num| 
		lines = lines.keep(num); 
		lineShorts = lineShorts.keep(num); 
		hasMovedOn = true;
	}
	*drop { |num| 
		lines = lines.drop(num); 
		lineShorts = lineShorts.drop(num); 
		hasMovedOn = true;
	}
	
	*saveCS { |path, forward=false| 
		var file, lines2write; 

		lines2write = if (forward) { lines.reverse } { lines };
		path = path ?? { saveFolder ++ "history_" ++ this.startTimeStamp ++ ".sc" };
		file = File(path.standardizePath,"w");
		file.write(lines2write.asCompileString);
		file.close;
	}
	*loadCS { |path| 
		
	}
}