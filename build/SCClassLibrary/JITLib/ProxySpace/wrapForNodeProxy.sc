
//these extensions provide means to wrap Objects so that they 
//make sense within the server bus system according to a node proxy.

////////////////////// graphs, functions, numericals and arrays //////////////////

+Object { 
	
	//objects can define their own wrapper classes dependant on 
	//how they play, stop, build, etc. see SynthDefControl for example
	//the original (wrapped) object can be reached by the .source message 
	
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
}

+SequenceableCollection {
	prepareForProxySynthDef { arg proxy;
		^{ this.collect({ arg item; item.value(proxy) }) }
	}
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
	buildForProxy { arg proxy, channelOffset=0, nameEnd;
		var ok;
		ok = proxy.initBus(this.rate, this.numChannels);
		^if(ok) {
			^{ this.value(proxy) }.buildForProxy(proxy, channelOffset, nameEnd) 
		}
	}

}

//needs a visit: lazy init + channelOffset

+Bus {
	makeProxyControl { arg channelOffset=0;
		^BusPlug.for(this).makeProxyControl(channelOffset);
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
	makeProxyControl {} //already wrapped
}







///////////////////////////// Pattern - Streams ///////////////////////////////////


+Pattern {
	proxyControlClass {
		^StreamControl
	}
	buildForProxy { arg proxy, channelOffset=0;
		^this.asStream.buildForProxy(proxy, channelOffset)
	}
}


+Stream {


	buildForProxy { arg proxy, channelOffset=0;
		var player, ok, index, server, event, numChannels, rate;
		player = this.asEventStreamPlayer;
		ok = if(proxy.isNeutral) { 
			rate = player.event.at(\rate) ? 'audio';
			numChannels = player.event.at(\numChannels) ? 2;
			proxy.initBus(rate, numChannels);
		} {
			rate = proxy.rate; // if proxy is initialized, it is user's responsibility
			numChannels = proxy.numChannels;
			true
		};
		^if(ok) { 
				index = proxy.index;
				server = proxy.server;
				
				// event = player.event;
				event = Event.default;
				event.use({
					//~player = MapNotePlayer.new;
					~channelOffset = channelOffset; // default value
					~addAction = 1;
					~finish = {
						~group = proxy.group.nodeID;
						//~mapping = proxy.nodeMap.mapArgs;
						~out = ~channelOffset % numChannels + index;
						~server = server;
						~freq = ~freq.value + ~detune;
						~amp = ~amp.value;
						~sustain = ~sustain.value;
					}
				});
				player.event = event;
			^player
		} { nil }

	}
	
	proxyControlClass {
		^StreamControl
	}
}



+Task {
	buildForProxy {}

}



///////////////// cx players ////////////

+SynthlessPlayer {
	
	proxyControlClass {
		^CXPlayerControl
	}
	
	wrapInFader {}
	
	
}

+AbstractPlayer {
	
	proxyControlClass {
			^CXSynthPlayerControl
	}
	
	makeProxyControl { arg channelOffset=0, proxy;
			var player;
			//this.prepareToPlayWithProxy(proxy); //do it here for now.
			player = if(proxy.isNeutral) { this } { this.wrapInFader(proxy) }Õ;
			^this.proxyControlClass.new(player, channelOffset); 
	}
	
	wrapInFader { arg bus;
			var n;
			n = bus.numChannels ? this.numChannels;
			
			^EnvelopedPlayer(
				this, 
				Env.asr(1, 1, 1), 
				n
			);
			
	}
	
}


+Instr {
	
	makeProxyControl { arg channelOffset=0, proxy;
		^Patch(this.name).makeProxyControl(channelOffset, proxy)
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
		if(controlNames.isNil) { ^false };
		^controlNames.any { arg cn; 
			cn.notNil and: { cn.name == "gate" } and: { cn.rate !== 'scalar' } 
		};
	}
}
