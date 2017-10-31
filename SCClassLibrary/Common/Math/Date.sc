Date {
	var <>year=2000, <>month=1, <>day=1, <>hour=0, <>minute=0, <>second=0, <>dayOfWeek=6,
		<>rawSeconds=0;

	*getDate { ^this.localtime }
	*localtime { ^this.new.localtime }
	*gmtime { ^this.new.gmtime }
	*new { arg year, month, day, hour, minute, second, dayOfWeek, rawSeconds;
		var instance = super.newCopyArgs(year, month, day, hour, minute, second, dayOfWeek,
			rawSeconds);
		case
		{ [year, month, day, hour, minute, second, dayOfWeek, rawSeconds].every(_.isNil) } {
			// All properties are nil: return current (local) time
			instance.localtime;
		}
		{ rawSeconds.notNil } {
			// rawSeconds was specified: use it to calculate all other properties
			instance.prResolveFromRawSeconds;
		}
		{
			// In all other cases, resolve dayOfWeek and rawSeconds from the other
			// properties (YMD, HMS).  If no year is provided, will throw an error.
			instance.resolve;
		};

		^instance;
	}
	*newFromFormattedString { arg string, format;
		^this.new.prFromString(string, format)
	}
	storeArgs {
		^[year, month, day, hour, minute, second, dayOfWeek, rawSeconds]
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
		^this.primitiveFailed
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

	resolve {
		_Date_Resolve
		^this.primitiveFailed
	}
	prFromString { arg string, format;
		_Date_FromString
		^this.primitiveFailed
	}
	prResolveFromRawSeconds {
		_Date_ResolveFromRawSeconds
		^this.primitiveFailed
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
	< { arg other;
		^this.rawSeconds < other.rawSeconds
	}
	<= { arg other;
		^this.rawSeconds <= other.rawSeconds
	}
	> { arg other;
		^this.rawSeconds > other.rawSeconds
	}
	>= { arg other;
		^this.rawSeconds >= other.rawSeconds
	}
	== { arg other;
		^this.rawSeconds == other.rawSeconds
	}
	!= { arg other;
		^this.rawSeconds != other.rawSeconds
	}
}
