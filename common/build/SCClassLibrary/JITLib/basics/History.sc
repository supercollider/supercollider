History { 		// adc 2006, Birmingham; rewrite 2007.

	classvar <>forwardFunc, <date, <startTimeStamp, <time0, listenFunc;
	classvar <>verbose = false, <>recordLocally = true, <started=false;

	classvar <>saveFolder = "~/Desktop/", <logFolder, <logFile, <logPath, <>keepsLog = true;
	classvar <>current, <>maxShortLength=65;

	var <lines, <lineShorts, <keys, <player, <hasMovedOn;

	*timeStamp {
			// hope it works on linux?
		if (thisProcess.platform.isKindOf(UnixPlatform)) {
			^Date.getDate.stamp
		} {
			// "// temporary kludge to fix Date's brokenness on windows".postln;
			^Main.elapsedTime.round(0.01)
		};
	}

	*dateString {
			// hope it works on linux?
		if (thisProcess.platform.isKindOf(UnixPlatform)) {
			^Date.getDate.asString
		} {
			// temporary kludge to fix Date's brokenness on windows
			^"__date_time_please__"
		};
	}

	*initClass {
		Class.initClassTree(TaskProxy);
		this.makeLogFolder;
		current = this.new;

		listenFunc = { |str, val, func|
			if ( func.notNil
					and: { str.notEmpty }
					and: { str != "\n" }
					and: { str.keep(7) != "History" })
				{
					if (this.verbose, { [str, val, func].postcs });
					if (this.recordLocally, {
						this.enter(thisProcess.interpreter.cmdLine)
					});
					this.forwardFunc.value(str, val, func);
				}
		};
	}

		// top level interface :

	*start {
		var interp = thisProcess.interpreter;
		if(started.not) {
			interp.codeDump = interp.codeDump.addFunc(listenFunc);
			this.hasMovedOn_(true);
			started = true;
			this.startLog;
		} {
			"History has started already.".postln;
		}
	}

	*end {
		thisProcess.interpreter.codeDump = thisProcess.interpreter.codeDump.removeFunc(listenFunc);
		this.endLog;
		this.hasMovedOn_(true);
		started = false;
	}

	*hasMovedOn_ { |flag=true| current.hasMovedOn_(flag) }
	*hasMovedOn  { ^current.hasMovedOn }

	*play { |start=0, end, verbose=true| ^current.play(start, end, verbose) }
	*stop { ^current.stop }

	*enter { |lineStr, id=\me|
		var dateNow, now;
		lineStr = this.prettyString(lineStr);

		if (lineStr.isEmpty) { ^this };	// nothing worth remembering

		if (current.lines.isEmpty) { 	// get startTime if first entry
			startTimeStamp = this.timeStamp;
			date = this.dateString;
			time0 = Main.elapsedTime;

		};

		this.hasMovedOn_(true);
		now = (Main.elapsedTime - time0);
		if (now < 1e-04) { now = 0 }; // on start

		if (keepsLog) { this.addToLog([now, id, lineStr]) };
		current.addLine(now, id, lineStr);
	}

		// forward to current for backwards compat...
	*lines { ^current.lines }
	*lineShorts { ^current.lineShorts }
		// editing
	*removeAt {|index| current.removeAt(index) }
	*removeLast { current.removeLast }
	*keep {|num| current.keep(num) }
	*drop {|num| current.drop(num) }

		// instance methods:
	*new { |lines|
		^super.new.init(lines);
	}

	hasMovedOn_ { |flag=true| hasMovedOn = flag; }

	lines_ { |inLines|
		lines.array_(inLines);
		lineShorts.array_(lines.collect { |line| this.class.shorten(line) });
		keys.clear;
		keys.addAll(lines.collect(_[1]));
	}

	clear {
		lines = List[];
		lineShorts = List[];
		hasMovedOn = true;
	}
	*clear { current.clear }

	init { |inLines|
		keys = IdentitySet.new;
		this.clear.lines_(inLines);

		player = TaskProxy.new.source_({ |e|
			var linesSize, lineIndices, lastTimePlayed;
			linesSize = lines.size;

			if (linesSize > 0) {	// reverse indexing
				lineIndices = (e.endLine.min(linesSize) .. e.startLine.min(linesSize));

				lineIndices.do { |index|
					var time, id, code, waittime;
					#time, id, code = lines[index];

					waittime = time - (lastTimePlayed ? time);
					lastTimePlayed = time;
					waittime.wait;
					if (e.verbose) { code.postln };
					code.compile.value;	// so it does not change cmdLine.
				};
			};
			0.5.wait;
			"history is over.".postln;
		}).set(\startLine, 0, \endLine, 0);
	}

	makeCurrent { History.current = this; hasMovedOn = true }
	isCurrent { ^this === History.current }

	play { |start=0, end, verbose=true|	// line numbers;
									// starting from past 0 may not work.
		start = start.clip(0, lines.lastIndex);
		end = (end ? lines.lastIndex).clip(0, lines.lastIndex);

		player.set(\startLine, start, \endLine, end, \verbose, verbose);
		player.play;
	}

	stop { player.stop; }

	addLine { |now, authID, lineStr|
		var line = [ now, authID, lineStr ];
		if (lines.isEmpty) {
			lines.add(line);
			lineShorts.add(this.class.shorten(line));
		} {
			lines.addFirst(line);
			lineShorts.addFirst(this.class.shorten(line));
		};
		keys.add(authID);
	}
		// simple editing
	removeAt { |index|
		if (index.isKindOf(Collection)) { index.sort.reverseDo (this.removeAt(_)); ^this };

		if (index < lines.size) { 	// ignore out of range indices, keep lists synced.
			[lines, lineShorts].do(_.removeAt(index));		};
		hasMovedOn = true;
	}
	removeLast { this.removeAt(lines.size - 1) }
	keep { |num|
		lines = lines.keep(num);
		lineShorts = lineShorts.keep(num);
		hasMovedOn = true;
	}
	drop { |num|
		lines = lines.drop(num);
		lineShorts = lineShorts.drop(num);
		hasMovedOn = true;
	}
		// loading from and saving to files
	*saveCS { |path, forward=false| current.saveCS(path, forward) }
	saveCS { |path, forward=false|
		var file, lines2write;

		lines2write = if (forward) { lines.reverse } { lines };
		path = path ?? { saveFolder ++ "history_" ++ this.class.timeStamp ++ ".scd" };
		file = File(path.standardizePath, "w");
		file.write(lines2write.asCompileString);
		inform("History written to:" + path);
		file.close;
	}

	*loadCS { |path, forward=false| current.loadCS(path, forward) }

	loadCS { |path, forward=false|
		var file, ll;
		protect {
			file = File(path.standardizePath, "r");
			ll = file.readAllString;
		} {
			file.close;
		};
		ll !? {
			ll = ll.compile.value;
			if (forward) { ll = ll.reverse };
			this.lines_(ll)
		};
	}

			// network setups support
	*network { }
	*localOn { recordLocally = true }
	*localOff { recordLocally = false }

			// string formatting utils
	storyString {
		var alone = lines.collectAs({ |line| line[1] }, IdentitySet).size == 1;
		var str;

		str = "///////////////////////////////////////////////////\n";
		str = str ++ format("// History, as it was on %.\n", this.class.dateString);
		str = str ++ "///////////////////////////////////////////////////\n\n";

		lines.reverseDo { |x|
			var now, id, cmdLine;
			#now, id, cmdLine = x;
				str = str ++
				format("// - % - % \n",
					this.class.formatTime(now),
					if(alone) { "" } { "(" ++ id ++ ")" }
				);
			if(cmdLine.find("\n").notNil and: { cmdLine[0] != $( }) {
				cmdLine = format("(\n%\n);", cmdLine)
			};
			str = str ++ cmdLine ++ "\n\n";
		};
		^str;
	}

	*saveStory { |path| current.saveStory(path) }

	saveStory { |path|
		var file;
		path = path ?? { saveFolder ++ "History_" ++ this.class.timeStamp ++ ".scd" };

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
	*unformatTime { arg str;
			var h, m, s;
			#h, m, s = str.split($:).collect(_.interpret);
			^h * 60 + m * 60 + s
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

		// convert to shortline for gui
	*shorten { |line, maxLength|
		var  time, id, lineStr, head, length;
		#time, id, lineStr = line;
		head = (this.formatTime(time) + "-" + id + "- ");
		maxLength = maxLength ? maxShortLength;
		^head ++ lineStr.keep(maxLength  - head.size);
	}

		// not used yet
	*getTimeFromString { arg str;
		var ts, i;
		if(str.beginsWith("// - ").not) { ^nil };
		i = str.find(" - ", offset: 4);
		if(i.isNil) { i = 10 }; // assume it's ok.
		ts = str[5..i+2].postln.split($:).collect(_.asFloat);
		^ts[0] * (60 * 60) + (ts[1] * 60) + ts[2]
	}
		// not used yet
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

	*cmdPeriod { this.enter("// thisProcess.cmdPeriod"); }

				// log file support - global only
	*makeLogFolder {
		var supportDir = thisProcess.platform.userAppSupportDir;
		var specialFolder = supportDir ++ "/HistoryLogs";

		if (pathMatch(supportDir).isEmpty) { logFolder = ""; ^this };

		if (pathMatch(specialFolder).isEmpty) {
			unixCmd("mkdir \"" ++ specialFolder ++ "\"");
			if (pathMatch(specialFolder).isEmpty) {
				logFolder = supportDir; // if not there, put it in flat
			}
		} {
			logFolder = specialFolder;
		};

		// ("// History.logFolder:" +  logFolder).postln;
	}

	*showLogFolder { unixCmd("open \"" ++ logFolder ++ "\"") }

	*checkLogStarted {

		var isOpen;
		if (logFile.isNil) { this.startLog };

		isOpen = logFile.isOpen;
		^if (isOpen.not) { this.startLog; ^logFile.isOpen } { true };
	}

	*startLog {
		var timeStamp = this.timeStamp;
		var timeString = this.dateString;
		// open file with current date
		logPath = logFolder ++ "/log_History_" ++ timeStamp ++ ".scd";
		logFile = File(logPath, "w");
		if (logFile.isOpen) {
			logFile.write(format("// History, as it was on %.\n\n",
				timeString) ++ "[\n" /*]*/ );
			"// History.logFile opened.".inform;
		} {
			"// History: could not open logFile!".warn;
		};
	}

	*addToLog { |line|
		// add a single line
		if (this.checkLogStarted) {
			try {
				logFile.write(line.asCompileString ++ ",\n")
			} {
				"// History: could not write to logFile!".warn;
			}
		} {
			warn("// History: logFile is not open!");
		};
	}

	*endLog {
		// close file
		try { logFile.write( /*[*/ "];") };
		try { logFile.close; "// History.logFile closed.".inform; };
	}

	*showLogFile { Document.open(this.logPath) }

	matchKeys { |key|
		var indices = [];
		if (key == \all) { ^(0..lines.size - 1) };
		if (key.isNil) { ^nil };

			// list of keys:
		if (key.isArray) {
			lines.do { |line, i| if (key.includes(line[1])) { indices = indices.add(i) } }
		} {
			lines.do { |line, i| if (line[1] == key) { indices = indices.add(i) } }
		};
		^indices
	}

	matchString { |str, ignoreCase=true|
		var indices = [];
		if (str.notNil and: (str != "")) {
			lines.do { |line, i| if (line[2].find(str, ignoreCase).notNil) { indices = indices.add(i) } };
			^indices
		} { ^nil }
	}

	indicesFor { |keys, string=""|
		var indicesK, indicesS, indicesFound;
		indicesK = this.matchKeys(keys);
		indicesS = this.matchString(string);
	//	[\indicesK, indicesK, \indicesS, indicesS].postln;

		indicesFound = if (indicesK.notNil) {
			if (indicesS.notNil) { indicesK.sect(indicesS) } { indicesK }
		} {
			if (indicesS.notNil) { indicesS } { (0.. lines.size - 1) }
		};
		^indicesFound
	}

	*makeWin { |where, textHeight=12| ^current.makeWin(where, textHeight) }

	makeWin { |where, textHeight=12| ^HistoryGui(this, where, textHeight) }

	*document { current.document }

	document { arg title="";	// platform dependent ...
		var docTitle;
		Platform.case(
			\windows, {
				this.storyString.newTextWindow("History_documented");
			},
			{
				docTitle = title ++ Date.getDate.format("%Y-%m-%e-%Hh%M-History");
				Document.new(docTitle, this.storyString)
					.path_(docTitle); // don't lose title.
			}
		)
	}

	*readFromDoc { |path|
		var file, line, count = 0, lineStrings = [], comLineIndices = [], splitPoints;
		file = File(path.standardizePath, "r");

		if (file.isOpen.not) {
			("History: file" + path + "not found!").warn;
			^false
		};
			// read all lines, keep indices of commentlines
		while { line = file.getLine; line.notNil } {
			lineStrings = lineStrings.add(line);
			if (line.beginsWith("// - ")) {
				splitPoints = line.findAll(" - ");
				comLineIndices = comLineIndices.add([count] ++ splitPoints);
			};
			count = count + 1;
		};

		^comLineIndices.collect { |list, i|
			var lineIndex, sep1, sep2, nextComIndex;
			var comLine, timeStr, time, key, codeStr;
			#lineIndex, sep1, sep2 = list;

			comLine = lineStrings[lineIndex];
			timeStr = comLine.copyRange(sep1 + 3, sep2 - 1);
			time = History.unformatTime(timeStr);

			key = comLine.copyToEnd(sep2 + 3).select(_.isAlphaNum).asSymbol;
			nextComIndex = (comLineIndices[i + 1] ? [lineStrings.size]).first;

			codeStr = lineStrings.copyRange(lineIndex + 1, nextComIndex - 2).join;

			[time, key, codeStr];
		};
	}
	*checkPath { |path|
		var ext = path.splitext[1];
		if ([\sc, \scd, \txt, \nil, \rtf].includes(ext.asSymbol)) {
			^true
		} {
			warn("History: file format" + ext + "for story files likely not supported!				Please use .txt, .scd, or other text format.");
			^false
		};
	}
		// load file saved with saveStory
	*loadStory { |path| current.loadStory(path) }

	loadStory { |path|
		var lines;
		if (this.class.checkPath(path)) {
			lines = this.class.readFromDoc(path);
			if (lines == false) {
				warn("History: no lines, so could not be loaded.")
			} {
				this.lines_(lines.reverse)
			}
		};
	}
	
	*rewrite { |path, open = true| 
		var lines, time, repath, file2; 
		lines = path.load;
		
		if (lines.isNil) { "no history, no future.".warn; ^this }; 
		
		time = path.basename.splitext.first.keep(-13).split($_).collect { |str, i| 
			str.clump(2).join("-:"[i]);
		}.join($ );
		
		repath = path.splitext.collect { |str, i| str ++ ["_rewritten.", ""][i] }.join;
	
		file2 = File.open(repath, "w");
		file2.write("// History rewritten from" + time); 
		lines.do { |line| 
			var time, tag, code; 
			#time, tag, code = line;
			file2.write("\n\n\n// when: % - who: % \n\n(\n%\n)\n".format(time, tag, code));
		};
		file2.close;
		if (open) { repath.openTextFile };
	}
}
