History { 		// adc 2006, Birmingham. 

	classvar <>forwardFunc, <date, <startTimeStamp, <time0, function;
	classvar <lines, <lineShorts, <>displayTime = 0.1;
	classvar <>title = "History repeats";
	classvar <w, <bounds, <doc, pop, startbut;
	classvar <docFlag = \sameDoc, <hasMovedOn = true;
	classvar <>verbose = false, <>recordLocally = true, <started=false;
	classvar <>saveFolder = "~/Desktop/";
	classvar <player;
	
	*initClass { 
		Class.initClassTree(TaskProxy);
		bounds = Rect(0, 0, 300, 200); 
		
		player = TaskProxy.new.source_({ |e| 
			var lineIndices; 
			var lastTimePlayed;
			
			lineIndices = (e.endLine, e.endLine - 1 .. e.startLine); 
			
			lineIndices.do { |index| 
				var time, id, code, waittime;
				#time, id, code = lines[index];
				waittime = time - (lastTimePlayed ? time);
			//	[\waittime, waittime].postln;
				lastTimePlayed = time;
				waittime.wait; 
				if (e.verbose) { code.postln };
				code.compile.value;	// so it does not change cmdLine.
			};
			0.5.wait;
			"history is over.".postln;
		});
		
		player.set(\startLine, 0, \endLine, 0);
		
		this.clear;
	}
	*clear { 
		lines = List(1024);
		lineShorts = List(1024); 
		hasMovedOn = true;
	}
			// replace (\n ..." with (... ))!!!
	*start { 
		var interp = thisProcess.interpreter;
		if(started.not) {
			function = { |str, val, func| 
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
			interp.codeDump = interp.codeDump.addFunc(function);
			hasMovedOn = true;
			started = true;
		} {
			"History has started already.".postln;
		}
	}
	*stop {
		thisProcess.interpreter.codeDump = thisProcess.interpreter.codeDump.removeFunc(function); 
		hasMovedOn = true;
		started = false;
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
	*findDoc {
		if (doc.isNil or: { Document.allDocuments.includes(doc).not } or: { pop.value == 1 }) { 
			doc = Document(title).bounds_(Rect(300, 500, 300, 100));
		};	
	} 
	
	*storyString {
		var alone = lines.flop[1].as(IdentitySet).size == 1;
		var str, d, date = Date.getDate;
		
		str = "///////////////////////////////////////////////////\n";
		str = str ++ format("// History, as it was on %.\n", date);
		str = str ++ "///////////////////////////////////////////////////\n\n";
		
		lines.reverseDo { |x|
			var now, id, cmdLine;
			#now, id, cmdLine = x;
				str = str ++ 
				format("// - % - % \n", 
					this.formatTime(now), 
					if(alone) { "" } { "(" ++ id ++ ")" }
				);
			if(cmdLine.find("\n").notNil and: { cmdLine[0] != $( }) {
				cmdLine = format("(\n%\n);", cmdLine)
			};
			str = str ++ cmdLine ++ "\n\n";
		};
		^str;
	}
	
	*document { arg title="";
		var docTitle = title ++ Date.getDate.format("%Y-%d-%e-%Hh%m-History");
		Document.new(docTitle, this.storyString)
			.path_(docTitle); // don't lose title.
	}
	
	*saveStory { |path| 
		var file;
		path = path ?? { saveFolder ++ "History_" ++ Date.getDate.stamp ++ ".sc" };
		
		file = File(path.standardizePath, "w");
		file.write(this.storyString);
		file.close;
	}
		
	*formatTime { arg val;
			var h, m, s;
			h = val div: (60 * 60);
			val = val - (h * 60 * 60);
			m = val div: 60;
			val = val - (m * 60);
			s = val;
			^"%:%:%".format(h, m, s.round(0.01))
	}
	*getTimeFromString { arg str;
		var ts, i;
		if(str.beginsWith("// - ").not) { ^nil };
		i = str.find(" - ", offset: 4);
		if(i.isNil) { i = 10 }; // assume it's ok.
		ts = str[5..i+2].postln.split($:).collect(_.asFloat);
		^ts[0] * (60 * 60) + (ts[1] * 60) + ts[2]
	}
	*asLines { arg str;
		var indices;
		indices = str.findAll("// -");
		^str.clumps(indices.differentiate)
	}
	/*
	// problem: interpreter cancels backslashes etc.
	*stream { arg str, func;
		var lastTime=0, time;
		func = func ?? {
			{|str|
				var dt = ~prev / str.size;
				fork {
					0.2.wait; // wait until result from last evaluation is printed
					str.do {|char|
						char.post;
						dt.wait;
					};
					str.compile.value;
					
				};
			}
		
		};
		^Routine {
			this.asLines(str).do { |line|
				time = this.getTimeFromString(line) ? lastTime;
				(prev:lastTime, delta: time - lastTime, play: { func.(line); }).yield;
				lastTime = time;
			}
		}
	}
	*play { arg str, clock;
		str = str ? Document.current.string;
		^this.stream(str).asEventStreamPlayer.play(clock);
	}
	*playDocument {
	
	}
	*/	
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
		
		lines.addFirst([ now, id, lineStr ]);
		lineShorts.addFirst( "" + now.round(displayTime) + "-" + id + "-" + lineStr.keep(50) );
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
	*removeLast { this.removeAt(lines.size - 1) }
	
	*saveCS { |path, forward=false| 
		var file, lines2write; 

		lines2write = if (forward) { lines.reverse } { lines };
		path = path ?? { saveFolder ++ "history_" ++ this.startTimeStamp ++ ".sc" };
		file = File(path.standardizePath,"w");
		file.write(lines2write.asCompileString);
		file.close;
	}
	
	*loadCS { |path| 
		var file, ll;
		protect {
			file = File(path.standardizePath, "r");
			ll = file.readAllString;
		} {
			file.close;
		};
		ll !? { lines = ll.compile.value };	
	}
	
	*play { |start=0, end, verbose=true| 	// line numbers; 
						// starting from past 0 may not work.
		start = start.clip(0, lines.lastIndex); 
		end = (end ? lines.lastIndex).clip(0, lines.lastIndex); 
		
		player.set(\startLine, start, \endLine, end, \verbose, verbose); 
		player.play;
	}

}
