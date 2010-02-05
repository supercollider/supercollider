HistoryDispatch : EnvirDispatch {

	var <histories, <startTime;
	var <>manual = false, <>alwaysReplace = true;
	var <>virtualTime, <>action, <timepoints;

	*new {
		^super.new.init
	}
	init {
		histories = IdentityDictionary.new;
		startTime = Main.elapsedTime;
		timepoints = [];
	}

	dispatch { arg key, obj;
		var delta = Main.elapsedTime - startTime;
		var index;
		if(manual.not) {
			histories[key] = histories[key].add(delta).add(obj);
		} {
			index = this.findTimeIndex(key, virtualTime);
			if(index.notNil && alwaysReplace) {
				histories[key][index + 1] = obj;
			} {
				if(index.isNil) {
					histories[key] = histories[key].add(virtualTime).add(obj);
				} {
					if(index == -1) {
						histories[key] = histories[key].addFirst( obj);
						histories[key] = histories[key].addFirst(virtualTime);
					} {
						if(virtualTime == histories[key][index]) {
							histories[key] = histories[key].put(index + 1, obj);
						} {
							histories[key] = histories[key].insert(index + 2, virtualTime);
							histories[key] = histories[key].insert(index + 3, obj);
						}
					}
				}
			}
		};
		this.updateTimePoints;
	}

	removeAtTime { arg key, time, updateTimePoints=true;
		var index = this.findTimeIndex(key, time);
		if(index.notNil and: { index != -1 }) {
			2.do { histories[key].removeAt(index) };
			if(updateTimePoints) { this.updateTimePoints };
		};

	}
	removeAllAtTime { arg time;
		histories.keys.do { |key|
			this.removeAtTime(key, time, false)
		};
		//timepoints.remove(time);
		this.updateTimePoints;
		histories.postln;
	}

	updateTimePoints {
		var timeset;
		timeset = IdentitySet.new;
		histories.do {|x|
				forBy(0, x.size - 2, 2) { |i|
					timeset.add(x.at(i))
				}
		};
		timepoints = timeset.asArray.sort;
	}

	setTime { arg time;
		var changed = false;
		//"set time to %\n".postf(time);
		histories.keys.do { |key|
			changed = changed || this.prGotoTime(key, time);
		};
		virtualTime = time;
		^changed
	}

	findObjectTimeIndex { arg key, obj;
		var hist = histories[key];
		if(hist.isNil) { ^nil };
		hist.pairsDo { arg time, item, i;
			if(item === obj) { ^i }
		};
		^nil
	}

	findTimeIndex { arg key, time;
		var hist = histories[key];
		if(hist.isNil or: { hist.isEmpty }) { ^nil };
		if(time < hist[0]) { ^-1 };
		forBy(0, hist.size - 3, 2) { |i|
			if(hist[i] <= time and: { hist[i + 2] > time }) {
				^i
			}
		};
		^hist.size - 2

	}

	findExactTimeIndex { arg key, time;
		var hist = histories[key];
		if(hist.isNil) { ^nil };
		if(time < hist[0]) { ^nil };
		forBy(0, hist.size, 2) { |i|
			if(hist[i] == time) { ^i };
		};
		^nil
	}

	getPairsForTime { arg time;
		var res;
		histories.keys.do { |key|
			if(key.notNil) {
				res = res.add([key, this.getObjectForTime(key, time)])
			}
		};
		^res
	}

	getObjectForTime { arg key, time;
		var index, timeIndex;
		var hist = histories[key];

		if(hist.isNil) { ^false };
		index = this.findObjectTimeIndex(key, envir.envir.at(key).source);
		timeIndex = this.findTimeIndex(key, time);
		// [index, timeIndex, hist[timeIndex + 1]].postln;
		^if(timeIndex == -1) { nil } { hist[timeIndex + 1] }
	}

	// printing and saving.
	// using History format.
	storyString {
		var str, keys;

		str = "///////////////////////////////////////////////////\n";
		str = str ++ format("// History, as it was on %.\n", History.dateString);
		str = str ++ "///////////////////////////////////////////////////\n\n";

		keys = histories.keys.asArray.sort;

		timepoints.do {|t|
			str = str ++ format("// - % \n",
					History.formatTime(t)
			);
			str = str ++ "(\n";
			keys.do { |key|
				var i = this.findExactTimeIndex(key, t);
				i !? {

					str = str ++ format("~% = %;\n", key,
						histories.at(key).at(i + 1).asCompileString
					);
				};
			};
			str = str ++ ");\n\n";
		};
		^str
	}
	/*
	// not yet implemented..
	loadFromHistory { |path|
		var file, str, delim, times;
		protect {
			file = File(path.standardizePath, "r");
			str = file.readAllString;
		} {
			file.close;
		};
		str = str.stripRTF;
		delim = str.findAll("// -").add(str.size - 1);

		str = str.clumps(delim.differentiate).drop(1).drop(-1);
		times = str.collect { |x|
			History.getTimeFromString(x);
		};
		^this.notYetImplemented(thisMethod);
		// uups, this is not that easy. we need to parse out the proxy assignments.
		// "~*=".matchRegexp(x) // etc..



	}
	*/

	document { arg title="";	// platform dependent ...
		var docTitle;
		if (thisProcess.platform.isKindOf(UGen)) {
			docTitle = title ++ Date.getDate.format("%Y-%m-%e-%Hh%M-History");
			Document.new(docTitle, this.storyString)
			.path_(docTitle); // don't lose title.
		} {
			this.storyString.newTextWindow("History_documented");
		}
	}

	// returns whether a change happened

	prGotoTime { arg key, time;
		var obj;
		obj = this.getObjectForTime(key, time);
		if(obj !== envir.envir.at(key).source) {
			envir.localPut(key, obj);
			^obj !== nil
		} {
			^false
		};
	}

}
