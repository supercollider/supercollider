ProxySwitch : AudioProxy {
	*new { arg proxy, numChannels;
		^if(proxy.rate === 'audio', {
			AudioProxySwitch.new(proxy, numChannels)
		}, {
			ControlProxySwitch.new(proxy, numChannels)
		})
	}
}


AudioProxySwitch : AudioProxy {
	var <proxy;
	
	*new { arg proxy, numChannels;
		^super.new(proxy.server.postln, numChannels ?? {proxy.numChannels}).pinit(proxy)
	}
	
	pinit { arg prx;
		proxy = prx;
		this.obj_(this.outFunc);
	}
	
	proxy_ { arg prx;
		if(prx.rate === proxy.rate, { 
			proxy = prx;
			this.set(\inputIndex, this.index)
		})
	}
	outFunc {
		var i;
		i = this.index;
		^{
			var ctl;
			ctl = Control.names(\inputIndex).kr(i);
			In.ar(ctl, this.numChannels)
		}
	}
	index {
		^proxy.bus.index
	}

}

ControlProxySwitch : AudioProxySwitch {
	outFunc {
		var i;
		i = this.index;
		^{
			var ctl;
			ctl = Control.names(\inputIndex).kr(i);
			In.kr(ctl, this.numChannels)
		}
	}
	//get ControlProxy behaviour: redesign AudioProxy as neutral?
	
	*rate { ^\control }
	getOutput {
		var out;
		this.wakeUpParents;
		this.addToBuildSynthDef;
		^In.kr(bus.index, bus.numChannels);
	}
		
	value { ^this.kr }
	

}