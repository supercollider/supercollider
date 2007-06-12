

ProxySwitch : NodeProxy {
	var <proxy;
	
	*new { arg proxy;
		^super.new(proxy.server).initProxy(proxy)
	}
	
	initProxy { arg prx;
		proxy = prx;
		this.put(this.outFunc, 0, true, true, false, { this.input_(prx) });
	}
	
	input_ { arg prx;
		var i;
		
		if(prx.rate === proxy.rate, { 
			proxy = prx;
			i = this.inputIndex;
			if(i.notNil, {this.set(\inputIndex, i)})
		}, { "incompatible rates".inform })
	}
	outFunc {
		var i;
		i = this.inputIndex;
		^{
			var ctl;
			ctl = Control.names(\inputIndex).kr(i);
			In.ar(ctl, proxy.numChannels ? 2)
		}
	}
	inputIndex {
		^proxy.outbus.tryPerform(\index)
	}

}