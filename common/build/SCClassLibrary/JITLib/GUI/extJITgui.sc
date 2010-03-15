+ NodeProxy { 
	gui { | numItems, bounds, preset|
		^NodeProxyEditor(this, numItems, nil, bounds, preset);
	}
}

+ ProxySpace {
	gui { | numItems, bounds, preset|
		^ProxyMixer(this, numItems, nil, bounds, preset);
	}
}

+ Ndef {
	*gui { |numItems, bounds, preset, server|
		^NdefMixer(numItems, bounds, preset, server);
	}
}

+ Tdef { 
	*gui { | numItems, bounds, preset|
		^TdefAllGui(numItems, nil, bounds, preset);
	}
	gui { | numItems, bounds, preset|
		^TdefGui(this, numItems, nil, bounds, preset);
	}
}
+ Pdef { 
	*gui { | numItems, bounds, preset|
		^PdefAllGui(numItems, nil, bounds, preset);
	}
	gui { | numItems, bounds, preset|
		^PdefGui(this, numItems, nil, bounds, preset);
	}
}

+ Event { 
	gui { | numItems, bounds, preset|
		^EnvirGui(this, numItems, nil, bounds, preset);
	}
}