Score {
	var <>score, routine, isPlaying = false;
	classvar <>program;

	*initClass {
		program = "./scsynth";
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
		score = list;
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
		score = score.sort({ arg a, b; a[0] < b[0] });
	}
	play { arg server;
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
			routine.play;
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
		//sectionlist = sectionlist.add([end - start, [\c_set, 0, 0]]);
		if(configevents.notNil, 
			{if(configevents.isArray, 
				{if(configevents[0] == 0.0,
					{sectionlist = sectionlist.addFirst(configevents)},
					{"Configuration events should have a timestamp of 0.0".warn; ^nil})},
				{"Configuration events need to be a bundle array: [time, [events]]".warn;
					^nil})});
		^this.new(sectionlist);	
	}
	
	writeOSCFile { arg path, from, to;
		if(to.notNil or: {from.notNil}) {Ê
			from = from ? 0.0;
			to = to ? this.endTime;
			this.section(from, to).write(path) 
		} { 
			this.write(path) 
		};
	}
		
	recordNRT { arg oscFilePath, outputFilePath, inputFilePath, sampleRate = 44100, headerFormat = 
		"AIFF", sampleFormat = "int16", options, completionString="", duration = nil;
		this.writeOSCFile(oscFilePath, 0, duration);
		unixCmd(program + " -N" + oscFilePath + (inputFilePath ? "_") + outputFilePath + 			sampleRate + headerFormat + sampleFormat + 
			(options ?? {ServerOptions.new}).asOptionsString  
			+ completionString);
	}
	
	*recordNRT { arg list, oscFilePath, outputFilePath, inputFilePath, sampleRate = 44100, 
		headerFormat = "AIFF", sampleFormat = "int16", options, completionString="", duration = nil;
		this.new(list).recordNRT(oscFilePath, outputFilePath, inputFilePath, sampleRate = 44100, 
		headerFormat, sampleFormat, options, completionString, duration);
	}
	
	
	stop {
		isPlaying.if({routine.stop; isPlaying = false; routine = nil;}, {"Score not playing".warn;}
		);
	}
		
	*writeFromFile { arg path, oscFilePath;
		var list;
		list = thisProcess.interpreter.executeFile(path);
		this.write(list, oscFilePath);
	}
	
	*write { arg list, oscFilePath;
		var osccmd, f;
		f = File(oscFilePath, "w");
		protect {
			list.size.do { |i|
				osccmd = list[i].asRawOSC;
				f.write(osccmd.size).write(osccmd);
			};
		}{
			f.close;
		};
		"done".postln;
	}
	
	write { arg oscFilePath;
		this.class.write(score, oscFilePath);
	}
	
	storeArgs {
		^score
	}
	
}	

