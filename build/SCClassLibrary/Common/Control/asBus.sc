
+ Server {
	asBus { arg rate,numChannels,server;
		^Bus.next(rate,numChannels,this,server)
	}
}

+ Bus {
	asBus { ^this }
}

+ Integer {
	asBus { arg rate,numChannels,server;
		^Bus.new(rate,this,numChannels,server)
	}
}

+ Nil {
	asBus { arg rate,numChannels,server;
		^if(rate == \audio,{
			Bus(\audio,0,numChannels,server); // out yer speakers
		},{
			Bus.next(\control,numChannels,server); // private control bus
		})
	}
}

