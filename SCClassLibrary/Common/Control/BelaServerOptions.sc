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

	asOptionsArray { | port = 57110 |
		var o = super.asOptionsArray(port);
		o = o ++ ["-J", numAnalogInChannels.asString];
		o = o ++ ["-K", numAnalogOutChannels.asString];
		o = o ++ ["-G", numDigitalChannels.asString];
		o = o ++ ["-Q", headphoneLevel.asString];
		o = o ++ ["-X", pgaGainLeft.asString];
		o = o ++ ["-Y", pgaGainRight.asString];
		o = o ++ ["-A", speakerMuted.asString];
		o = o ++ ["-x", dacLevel.asString];
		o = o ++ ["-y", adcLevel.asString];
		o = o ++ ["-g", numMultiplexChannels.asString];
		o = o ++ ["-T", belaPRU.asString];
		o = o ++ ["-E", belaMaxScopeChannels.asString];
		^o
	}
}
