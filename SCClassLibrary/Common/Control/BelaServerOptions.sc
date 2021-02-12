BelaServerOptions : ServerOptions {

	classvar defaultValues;

	var <>numAnalogInChannels;
	var <>numAnalogOutChannels;
	var <>numDigitalChannels;
	var <>headphoneLevel;
	var <>pgaGainLeft;
	var <>pgaGainRight;
	var <>speakerMuted;
	var <>dacLevel;
	var <>adcLevel;
	var <>numMultiplexChannels;
	var <>belaPRU;
	var <>belaMaxScopeChannels;

	*initClass {
		defaultValues = super.defaultValues.copy.putAll((
			numAnalogInChannels: 2,
			numAnalogOutChannels: 2,
			numDigitalChannels: 16,
			headphoneLevel: -6,
			pgaGainLeft: 10,
			pgaGainRight: 10,
			speakerMuted: 0,
			dacLevel: 0,
			adcLevel: 0,
			numMultiplexChannels: 0,
			belaPRU: 1,
			belaMaxScopeChannels: 0
		));
	}

	init {
		defaultValues.keysValuesDo { |key, val| this.instVarPut(key, val) };
	}

	asOptionsString { | port = 57110 |
		var o = super.asOptionsString(port);
		o = o ++ " -J " ++ numAnalogInChannels;
		o = o ++ " -K " ++ numAnalogOutChannels;
		o = o ++ " -G " ++ numDigitalChannels;
		o = o ++ " -Q " ++ headphoneLevel;
		o = o ++ " -X " ++ pgaGainLeft;
		o = o ++ " -Y " ++ pgaGainRight;
		o = o ++ " -A " ++ speakerMuted;
		o = o ++ " -x " ++ dacLevel;
		o = o ++ " -y " ++ adcLevel;
		o = o ++ " -g " ++ numMultiplexChannels;
		o = o ++ " -T " ++ belaPRU;
		o = o ++ " -E " ++ belaMaxScopeChannels;
		^o
	}
}
