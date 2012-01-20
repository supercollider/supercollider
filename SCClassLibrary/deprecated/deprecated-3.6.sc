+ MIDIIn {
	*new { arg port;
		DeprecatedError(this, thisMethod).throw;
		^super.new.port_(port)
	}
	port {
		DeprecatedError(this, thisMethod).throw;		
		^port
	}
	port_ { |newPort|
		DeprecatedError(this, thisMethod).throw;		
		port = newPort;
	}
}