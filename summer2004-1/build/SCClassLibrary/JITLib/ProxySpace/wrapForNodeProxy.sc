
//these extensions provide means to wrap Objects so that they 
//make sense within the server bus system according to a node proxy.

////////////////////// graphs, functions, numericals and arrays //////////////////

+Object { 
	
	//objects can define their own wrapper classes dependant on 
	//how they play, stop, build, etc. see SynthDefControl for example
	//the original (wrapped) object can be reached by the .source message
	//for objects that only create ugen graphs, define prepareForProxySynthDef(proxy)
	
	proxyControlClass {
		^SynthDefControl
	}
	
	makeProxyControl { arg channelOffset=0;
		^this.proxyControlClass.new(this, channelOffset); 
	}
	
	
	//any preparations that have to be done to prepare the object
	//implement 'prepareForProxySynthDef' to return a ugen func
	
	
	//this method is called from within the Control
	buildForProxy { arg proxy, channelOffset=0, nameEnd;
		var argNames;
		argNames = this.argNames;
		^ProxySynthDef(
			proxy.generateUniqueName ++ nameEnd,
			this.prepareForProxySynthDef(proxy),
			proxy.nodeMap.ratesFor(argNames),
			nil, 
			true, 
			channelOffset,
			proxy.numChannels
		); 
	}
	prepareForProxySynthDef { ^this.subclassResponsibility(thisMethod) }
	
	argNames { ^nil }
	defaultArgs { ^nil }
	numChannels { ^1 } 
	clear { }
	//support for unop / binop proxy
	isNeutral { ^true }
	initBus { ^true }
	wakeUp {}
	
}

+Function {
	prepareForProxySynthDef { ^this }
	argNames { ^def.argNames }
	defaultArgs { ^def.prototypeFrame }
}


+SimpleNumber {
	prepareForProxySynthDef { arg proxy;
		^if(proxy.rate === 'audio', {
			{ Array.fill(proxy.numChannels ? 1, { K2A.ar(this) }) }
		}, { 
			{  Control.kr(Array.fill(proxy.numChannels ? 1, {this})) }
		})
	}
}

+BusPlug {
	prepareForProxySynthDef { arg proxy;
		proxy.initBus(this.rate, this.numChannels);
		^{ this.value(proxy) }
	}

}

//needs a visit: lazy init + channelOffset

+Bus {
	prepareForProxySynthDef { arg proxy;
		^BusPlug.for(this).prepareForProxySynthDef(proxy);
	}
}




///////////////////////////// SynthDefs and alike ////////////////////////////////////


+SynthDef {
	buildForProxy {}
	numChannels { ^nil } //don't know
	rate { ^nil }
}



+Symbol {
	buildForProxy {}
	proxyControlClass {
		^SynthControl
	}
}


+AbstractPlayControl {
	makeProxyControl { ^this.deepCopy } //already wrapped, but needs to be copied
}







///////////////////////////// Pattern - Streams ///////////////////////////////////

+Stream { // assumes event stream

	proxyControlClass {
		^StreamControl
	}
	buildForProxy { 
		^PauseStream.new(this)
	}
}



+PauseStream {
	buildForProxy {}
	
	proxyControlClass {
		^StreamControl
	}
}

/*
+Pdef {
	buildForProxy { arg proxy, channelOffset=0;
		isPlaying = true;
		^super.buildForProxy(proxy, channelOffset);
	}
}
*/

+Pattern {
	proxyControlClass {
		^PatternControl
	}
	
	buildForProxy { arg proxy, channelOffset=0;
		var player, ok, index, server, numChannels, rate;
		player = this.asEventStreamPlayer;
		ok = if(proxy.isNeutral) { 
			rate = player.event.at(\rate) ? 'audio';
			numChannels = player.event.at(\numChannels) ? NodeProxy.defaultNumAudio;
			proxy.initBus(rate, numChannels);
		} {
			rate = proxy.rate; // if proxy is initialized, it is user's responsibility
			numChannels = proxy.numChannels;
			true
		};
		^if(ok) { 
				index = proxy.index;
				server = proxy.server;
				player.event.use({
					~channelOffset = channelOffset; // default value
					~out = { ~channelOffset % numChannels + index };
					~server = { server };
					~finish = {
						~out = ~out.value;
						~group = proxy.group.asNodeID;// group shouldn't be assigned by the user
					}
				});
			^player
		} { nil }

	}

}





///////////  query whether some UGen can possibly free its synth from within


+ Object {
	canFreeSynth {
		^false
	}
}

+SequenceableCollection {
	canFreeSynth {
		^this.any({ arg item; item.canFreeSynth })
	}
}


+ EnvGen {
	canFreeSynth {
		^(inputs.at(4) > 1) and: //doneAction
			{ // it can happen that the gate is 1 and the envelope runs out
				inputs.at(0).isNumber.not or: //variable gate
				{ inputs.at(7) == -99 }		//or self releasing envelope
			}
		
	}
}

+ Linen {
	canFreeSynth {
		^(inputs.at(4) > 1) and: { inputs.at(0).isNumber.not }
	}
}

+ Free {
	canFreeSynth { ^true }
}

+ FreeSelf {
	canFreeSynth { ^true }
}

+ DetectSilence {
	canFreeSynth { ^inputs.at(3) > 1 }
}

+ SynthDef {
	canFreeSynth { ^children.canFreeSynth }
	canReleaseSynth { ^this.hasGateControl and: { this.canFreeSynth } }
	hasGateControl {
		if(allControlNames.isNil) { ^false };
		^allControlNames.any { arg cn; 
			cn.notNil and: { cn.name == 'gate' } and: { cn.rate !== 'scalar' } 
		};
	}
}
