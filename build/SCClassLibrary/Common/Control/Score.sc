Score {
	*play { arg server, file;
		var size, osccmd, timekeep, eventList;
		eventList = thisProcess.interpreter.executeFile(file);
		size = eventList.size;
		timekeep = 0;
		Routine({
			size.do { |i|
				var deltatime, msg;
				osccmd = eventList[i];
				deltatime = osccmd[0];
				msg = osccmd[1];
				(deltatime-timekeep).wait;
				server.sendBundle(server.latency, msg);
				timekeep = deltatime;
			}
		}).play;
	}
		
	*write { arg file, oscfile;
		var osccmd, eventList, f;
		eventList = thisProcess.interpreter.executeFile(file);
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