+ NodeProxy { 
	gui { | numItems, bounds, preset|
		// which options to support?
		^NdefGui(this, numItems, nil, bounds, options: preset);
	}
}

+ ProxySpace {
	gui { | numItems, bounds, preset|
		^ProxyMixer(this, numItems, nil, bounds, options: preset);
	}
}

+ Ndef {
	*gui { |numItems, bounds, preset|
		^NdefMixer(numItems, nil, bounds, options: preset);
	}
}

+ Tdef { 
	*gui { | numItems, bounds, preset|
		^TdefAllGui(numItems, nil, bounds, options: preset);
	}
	gui { | numItems, bounds, preset|
		^TdefGui(this, numItems, nil, bounds, options: preset);
	}
}
+ Pdef { 
	*gui { | numItems, bounds, preset|
		^PdefAllGui(numItems, nil, bounds, options: preset);
	}
	gui { | numItems, bounds, preset|
		^PdefGui(this, numItems, nil, bounds, options: preset);
	}
}

+ Dictionary { 
	gui { | numItems, bounds, preset|
		^EnvirGui(this, numItems, nil, bounds, options: preset);
	}
}