Score {
	var <>score, routine, isPlaying = false;
	
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
	
	*playFromFile { arg path, server;
		var list;
		list = thisProcess.interpreter.executeFile(path);
		^this.new(list).play(server);
	}
	
	*play { arg list, server;
		^this.new(list).play(server);
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
					msg = osccmd[1];
					(deltatime-timekeep).wait;
					inserver.sendBundle(inserver.latency, msg);
					timekeep = deltatime;
				};
				isPlaying = false;
			});
			isPlaying = true;
			routine.play;
		}, {"Score already playing".warn;}
		);
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
	
	*write {arg list, oscFilePath;
		var osccmd, f;
		f = File(oscFilePath, "w");
		list.size.do { |i|
			osccmd = list[i].asRawOSC;
			f.write(osccmd.size).write(osccmd);
		};
		f.close;
		"done".postln;
	}
	
	write {arg oscFilePath;
		this.class.write(score, oscFilePath);
	}
}	
