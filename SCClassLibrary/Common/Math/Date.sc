Date {
	var <>year=2000, <>month=1, <>day=1, <>hour=0, <>minute=0, <>second=0, <>dayOfWeek=6,
		<>rawSeconds=0, <>bootSeconds=0;

	*getDate { ^this.localtime }
	*localtime { ^this.new.localtime }
	*gmtime { ^this.new.gmtime }
	*new { arg year, month, day, hour, minute, second, dayOfWeek, rawSeconds, bootSeconds;
		^super.newCopyArgs(year, month, day, hour, minute, second, dayOfWeek,
				rawSeconds, bootSeconds);
	}
	storeArgs {
		^[year, month, day, hour, minute, second, dayOfWeek, rawSeconds, bootSeconds]
	}
	localtime {
		_LocalTime
		^this.primitiveFailed
	}
	gmtime {
		_GMTime
		^this.primitiveFailed
	}
	*seed {
		// return a value suitable for seeding a random number generator.
		_TimeSeed
	}

	// strings for time stamping things like filenames.
	dayStamp {
		var s;
		s = (((year % 100) * 100 + month) * 100 + day
				+ 1000000).asString;
		s.removeAt(0); // get rid of the leading '1' char that was put there to force leading zeroes.
		^s
	}
	secStamp {
		var s;
		s = ((hour * 100 + minute) * 100 + second + 1000000).asString;
		s.removeAt(0); // get rid of the leading '1' char that was put there to force leading zeroes.
		^s
	}
	stamp {
		^this.dayStamp ++ "_" ++ this.secStamp
	}
	hourStamp {
		^hour.asString ++ ":" ++ minute ++ ":"  ++ (rawSeconds % 60).round(0.0001)
	}
	asSortableString {  // standard database format, alphabetically sortable
		^String.streamContents({ arg s;
			s << year;
			if(month < 10,{ s <<  0 });
			s <<  month;
			if(day < 10,{ s <<  0 });
			s <<  day;
			if(hour < 10,{ s <<  0 });
			s <<  hour;
			if(minute < 10, { s <<  0 });
			s <<  minute;
			if(second < 10, { s <<  0 });
			s <<  second;
		})
	}

	asctime {
		_AscTime
		^this.primitiveFailed
	}
	asString {
		^this.asctime
	}
	format {
		arg format;
		_prStrFTime;
		^this.primitiveFailed
	}
}
