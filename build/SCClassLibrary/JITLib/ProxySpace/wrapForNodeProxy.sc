
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

+SimpleNumber { //some more efficient way needed to put a value here
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
		^{ this.value(proxy) }
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

+SoundDef {
	buildForProxy {
		^synthDef
	}
	proxyControlClass {
		^SoundDefControl
	}

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
		var player, ok, index, newParent, event;
		player = this.asEventStreamPlayer;
		ok = proxy.initBus(player.event.at(\rate) ? 'audio', player.event.at(\numChannels) ? 2);
		index = proxy.index;
		^if(ok)
			// remember to add to event's parent, so that 
			// an external change of player's event doesn't override this.
			{ 
				event = player.event;
				newParent = Event.make({
				~msgFunc = { arg id, freq;
					var args, bundle, names, nodeMap;
					names = ~argNames;
					args = currentEnvironment.hatch(names);
					bundle = [9, ~instrument, id, 0, ~group] ++ args;
					bundle = [bundle ++ [\freq, freq]];
					//mapping to buses
					nodeMap = proxy.nodeMap;
					nodeMap.mapToBundle(bundle, id);
					~mapArgs = nodeMap.mapArgs; //polyplayer support
					bundle
				};
				~finish = {
					~group = proxy.group.asNodeID;
					~i_out = ~out = index;
					~server = proxy.server;
					~freq = ~freq.value + ~detune;
					~amp = ~amp.value;
					~sustain = ~sustain.value;
				}
				}).parent_(event.parent);
				event.parent = newParent;
			player
			
		} { nil }

	}
	
	proxyControlClass {
		^StreamControl
	}
}

+StreamPlayerReference {
	proxyControlClass {
		^StreamControl
	}
	buildForProxy {}
}

+Pdef {
	
	buildForProxy { arg proxy, channelOffset=0;
		^this.player.buildForProxy(proxy, channelOffset);
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
			
			//this.prepareToPlayWithProxy(proxy); //do it here for now.
			^this.proxyControlClass.new(this.wrapInFader(proxy), channelOffset); 
			//^this.proxyControlClass.new(this, channelOffset); 

	}
	
	wrapInFader { arg bus;
			var n;
			n = bus.numChannels ? this.numChannels;
			
			^EnvelopedPlayer(
				this, //Patch({ arg input; NumChannels.ar(input, n, true) },[this]),
				Env.asr(1, 1, 1), 
				n
			);
			
	}
	
}

+Instr {
	
	makeProxyControl { arg channelOffset=0, proxy;
		^Patch(this).makeProxyControl(channelOffset, proxy)
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
		^if(inputs.at(4) > 1, { //doneAction
			if(inputs.at(7) != -99 and: { inputs.at(0).isNumber }, { //releaseNode, gate
				false
			}, {
				true
			})
		}, { 
			false 
		})
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
	
	canReleaseSynth {
		^if(controlNames.isNil, { 
			false 
		}, {
			//gate should be actually input to envgen, but this ist too much to check.
			controlNames.any({ arg name; name.name == "gate" })  
			and:
			{ this.canFreeSynth }
		})
	}
}
