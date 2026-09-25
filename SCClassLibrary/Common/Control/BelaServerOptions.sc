BelaServerOptions : ServerOptions {
	var <>numAnalogInChannels = 2;
	var <>numAnalogOutChannels = 2;
	var <>numDigitalChannels = 16;
	var <>headphoneLevel = -6;
	var <>pgaGainLeft = 10;
	var <>pgaGainRight = 10;
	var <>speakerMuted = 0;
	var <>dacLevel = 0;
	var <>adcLevel = 0;
	var <>numMultiplexChannels = 0;
	var <>belaPRU = 1;
	var <>belaMaxScopeChannels = 0;

	asOptionsString { | port = 57110 |
		var o = super.asOptionsString(port);
		o = o ++ " -J " ++ numAnalogInChannels.asInteger;
		o = o ++ " -K " ++ numAnalogOutChannels.asInteger;
		o = o ++ " -G " ++ numDigitalChannels.asInteger;
		o = o ++ " -Q " ++ headphoneLevel.asInteger;
		o = o ++ " -X " ++ pgaGainLeft.asInteger;
		o = o ++ " -Y " ++ pgaGainRight.asInteger;
		o = o ++ " -A " ++ speakerMuted.asInteger;
		o = o ++ " -x " ++ dacLevel.asInteger;
		o = o ++ " -y " ++ adcLevel.asInteger;
		o = o ++ " -g " ++ numMultiplexChannels.asInteger;
		o = o ++ " -T " ++ belaPRU.asInteger;
		o = o ++ " -E " ++ belaMaxScopeChannels.asInteger;

		^o
	}
}
