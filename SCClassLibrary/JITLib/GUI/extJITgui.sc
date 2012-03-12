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
	/*
		This method was introducing a conflict with the use of .gui(view,bounds)
		which is what the Object.gui system expects everything to be able to respond to.
		For the 3.5 release we decided (felix, julian, adc) to detect what args
		are passed in and just switch between systems.
		a Number indiciates jitlib usage: how many items to show in an editor.
		Nil would result in the super implementation which is a simple string representation
		of the key values list.
		A cleaner solution will be decided upon later.
	*/
	gui { |...args|
		var  numItems, bounds, preset;
		if(args[0].isNumber,{
			# numItems, bounds, preset = args;
			^EnvirGui(this, numItems, nil, bounds, options: preset);
		},{
			^super.gui(*args)
		})
	}
}

