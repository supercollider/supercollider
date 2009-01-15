SCLevelIndicator : SCView {

	*paletteExample { arg parent, bounds;
		^this.new(parent, bounds).initBackGround.value_("edit me");
	}
	
	value {
		^this.getProperty(\value)
	}
	value_ { arg val;
		this.setProperty(\value, val);
	}	
	valueAction_ { arg val;
		this.setPropertyWithAction(\value, val);
	}
	
	warning_ {arg val;
		this.setProperty(\warning, val);
	}
	
	critical_ {arg val;
		this.setProperty(\critical, val);
	}
	
	style_ {arg val;
		this.setProperty(\style, val);
	}
	
	numSteps_ {arg val;
		this.setProperty(\numSteps, val);
	}
	
	image_ {arg image;
		this.setProperty(\image, image);
	}
	
	numTicks_ {arg ticks;
		this.setProperty(\numTicks, ticks);
	}
	
	numMajorTicks_ {arg ticks;
		this.setProperty(\numMajorTicks, ticks);
	}
	
	drawsPeak_ {arg bool;
		this.setProperty(\drawsPeak, bool);
	}
	
	peakLevel_ { arg val;
		this.setProperty(\peakLevel, val);
	}
}

+ Server {
	meterOutput {
		var window, view, meters, resp, synth;
		
		window = Window.new(this.name ++ " Output Levels");
		view = VLayoutView(window, Rect(10,10,380,300) );
		meters = Array.fill( options.numOutputBusChannels, { arg i;
			SCLevelIndicator( view, Rect(0,0,75,20) ).warning_(0.9).critical_(1.0);
		});
		window.front;
		resp = OSCresponder(this.addr, "/" ++ this.name ++ "OutputLevels", { |t, r, msg| 			{msg.copyToEnd(3).do({|val, i| meters[i].value = val.ampdb.linlin(-40, 0, 0, 1);}) }.defer; 
		}).add;
		synth = SynthDef(this.name ++ "OutputLevels", {
			SendReply.kr(
				Impulse.kr(20), 
				"/" ++ this.name ++ "OutputLevels", 
				Amplitude.kr(In.ar(0, options.numOutputBusChannels), 0.2, 0.2);
			);
		}).play(RootNode(this), nil, \addToTail);
		
		window.onClose({synth.free; resp.remove});
	}
}