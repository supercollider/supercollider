Score {
	*play { arg path, server;
		var eventList;
		eventList = thisProcess.interpreter.executeFile(path);
		this.playList(eventList, server);
	}
	
	*playList { arg eventList, server;
		var size, osccmd, timekeep, inserver;
		inserver = server ? Server.default;
		size = eventList.size;
		timekeep = 0;
		Routine({
			size.do { |i|
				var deltatime, msg;
				osccmd = eventList[i];
				deltatime = osccmd[0];
				msg = osccmd[1];
				(deltatime-timekeep).wait;
				inserver.sendBundle(inserver.latency, msg);
				timekeep = deltatime;
			}
		}).play;
	}
		
	*write { arg path, oscfile;
		var osccmd, eventList, f;
		eventList = thisProcess.interpreter.executeFile(path);
		this.writeList(eventList, oscfile);
	}
	
	*writeList {arg eventList, oscfile;
		var osccmd, f;
		f = File(oscfile, "w");
		eventList.size.do { |i|
			osccmd = eventList[i].asRawOSC;
			f.write(osccmd.size).write(osccmd);
		};
		f.close;
		"done".postln;
	}
}	
