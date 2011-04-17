+ Server {
	asBus { arg rate,numChannels=1;
		^Bus.alloc(rate,this,numChannels)
	}
}

+ Bus {
	asBus { ^this }
}

+ Integer {
	asBus { arg rate = \control,numChannels=1,server;
		^Bus.new(rate,this,numChannels,server)
	}
}

+ Nil {
	asBus { arg rate,numChannels=1,server;
		^if(rate == \audio,{
			Bus(\audio,0,numChannels,server); // out yer speakers
		},{
			Bus.alloc(\control,server,numChannels); // private control bus
		})
	}
}
