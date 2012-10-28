Score {
	var <>score, routine, isPlaying = false;
	classvar <>program, <>options;

	*initClass {
		options = ServerOptions.new;
	}

	*new { arg list;
		^super.new.init(list);
	}

	*newFromFile { arg path;
		var list;
		list = thisProcess.interpreter.executeFile(path);
		^super.new.init(list);
	}

	init { arg list;
		score = [[0.0, ["/g_new", 1, 0, 0]]] ++ list;
		this.sort;
	}

	add { arg bundle;
		score = score.add(bundle)
	}

	*playFromFile { arg path, server;
		var list;
		list = thisProcess.interpreter.executeFile(path);
		^this.new(list).play(server);
	}

	*play { arg list, server;
		^this.new(list).play(server);
	}
	sort {
		score = score.sort({ arg a, b; b[0] >= a[0] });
	}
	play { arg server, clock, quant=0.0;
		var size, osccmd, timekeep, inserver, rout;
		isPlaying.not.if({
			inserver = server ? Server.default;
			size = score.size;
			timekeep = 0;
			routine = Routine({
				size.do { |i|
					var deltatime, msg;
					osccmd = score[i];
					deltatime = osccmd[0];
					msg = osccmd.copyToEnd(1);
					(deltatime-timekeep).wait;
					inserver.listSendBundle(inserver.latency, msg);
					timekeep = deltatime;
				};
				isPlaying = false;
			});
			isPlaying = true;
			routine.play(clock, quant);
		}, {"Score already playing".warn;}
		);
	}

	endTime { ^score.last[0] }
	startTime { ^score.first[0] }

	section { arg start = 0, end, configevents;
		var sectionlist;
		if(end.isNil) { end =  this.endTime };
		sectionlist = Array.new;
		score.do { arg item;
			if(item[0].inclusivelyBetween(start, end)) {
			item = item.copy;
			item[0] = item[0] - start;
			sectionlist = sectionlist.add(item);
			}
		};
		sectionlist = sectionlist.add([end - start, [0]]); // add dummy command (cmd_none)
		if(configevents.notNil,
			{if(configevents.isArray,
				{if(configevents[0] == 0.0,
					{sectionlist = sectionlist.addFirst(configevents)},
					{"Configuration events should have a timestamp of 0.0".warn; ^nil})},
				{"Configuration events need to be a bundle array: [time, [events]]".warn;
					^nil})});
		^this.class.new(sectionlist);
	}

	writeOSCFile { arg path, from, to, clock;
		if(to.notNil or: {from.notNil}) {
			from = from ? 0.0;
			to = to ? this.endTime;
			this.section(from, to).write(path, clock)
		} {
			this.write(path, clock)
		};
	}

	recordNRT { arg oscFilePath, outputFilePath, inputFilePath, sampleRate = 44100, headerFormat =
		"AIFF", sampleFormat = "int16", options, completionString="", duration = nil, action = nil;
		this.writeOSCFile(oscFilePath, 0, duration);
		unixCmd(program + " -N" + oscFilePath.quote
			+ if(inputFilePath.notNil, { inputFilePath.quote }, { "_" })
			+ outputFilePath.quote
		 	+ sampleRate + headerFormat + sampleFormat +
			(options ? Score.options).asOptionsString
			+ completionString, action);
	}

	*recordNRT { arg list, oscFilePath, outputFilePath, inputFilePath, sampleRate = 44100,
		headerFormat = "AIFF", sampleFormat = "int16", options, completionString="", duration = nil, action = nil;
		this.new(list).recordNRT(oscFilePath, outputFilePath, inputFilePath, sampleRate,
		headerFormat, sampleFormat, options, completionString, duration, action);
	}


	stop {
		isPlaying.if({routine.stop; isPlaying = false; routine = nil;}, {"Score not playing".warn;}
		);
	}

	*writeFromFile { arg path, oscFilePath, clock;
		var list;
		list = thisProcess.interpreter.executeFile(path);
		this.write(list, oscFilePath, clock);
	}

	*write { arg list, oscFilePath, clock;
		var osccmd, f, tempoFactor;
		f = File(oscFilePath, "w");
		tempoFactor = (clock ? TempoClock.default).tempo.reciprocal;
		protect {
			list.size.do { |i|
				var msg = list[i].copy;
				msg[0] = msg[0]* tempoFactor;
				osccmd = msg.asRawOSC;
				f.write(osccmd.size).write(osccmd);
			};
		}{
			f.close;
		};
		//"done".postln;
	}

	write { arg oscFilePath, clock;
		this.class.write(score, oscFilePath, clock);
	}

	saveToFile { arg path;
		var f;
		f = File.new(path, "w");
		f.putString("[ // SuperCollider Score output " ++ Date.getDate ++ "\n");
		score.do{ arg me;
			f.putString((me).asCompileString ++ ",\n");
		};
		f.putString("]");
		f.close;
	}

	storeArgs {
		^score
	}

	asScore {}
}
