Date {	var <>year=2000, <>month=1, <>day=1, <>hour=0, <>minute=0, <>second=0, <>dayOfWeek=6, 		<>rawSeconds=0, <>bootSeconds=0;		*getDate {		^this.new.getDate	}	*new { arg year, month, day, hour, minute, second, dayOfWeek, rawSeconds, bootSeconds;		^super.newCopyArgs(year, month, day, hour, minute, second, dayOfWeek, 				rawSeconds, bootSeconds);	}	getDate {		_GetDate		^this.primitiveFailed	}	*seed {		// return a value suitable for seeding a random number generator.		_TimeSeed	}
	
	// strings for time stamping things.	dayStamp {
		var s;
		s = (((year % 100) * 100 + month) * 100 + day 
				+ 1000000).asString;
		s.removeAt(0);
		^s
	}
	secStamp {
		var s;
		s = ((hour * 100 + minute) * 100 + second + 1000000).asString;
		s.removeAt(0);
		^s
	}
	stamp {
		^this.dayStamp ++ "_" ++ this.secStamp
	}
	// PRIVATE}