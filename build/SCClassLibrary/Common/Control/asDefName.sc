

+ SynthDef {

	asSynthDef { ^this }
	asDefName {	^name	}

}

+ Object {

	asSynthDef {
		error("Cannot convert this object to a SynthDef:" + this);
		this.dump;
		^nil
	}
	asDefName { 
		^this.asSynthDef.name
	}

}

	
+ String {
	asDefName { ^this }
}

+ Symbol {
	asDefName { ^this.asString }
}

+ Function {	
	/*
		this is mainly for  {}.play and Synth({ })
		
		Synth({
			SinOsc.ar
		})
		or:
		Synth({
			Out.ar(0,SinOsc.ar)
		})

		it inserts an Out only if it needs it		
	*/
	
	asDefName {
		var def;
		Routine({
			def = this.asSynthDef;
			def.writeDefFile;
			0.2.wait;
		}).play(AppClock);
		Server.local.loadSynthDef(def.name);
		^def.name
	}
	asSynthDef {
		var cycle;
		cycle = Library.at(Function,'__asSynthDef__') ? 0;
		Library.put(Function,'__asSynthDef__', cycle + 1);
		^SynthDef("__asSynthDef__" ++ cycle.asString,{
			var result,rate;
			result = this.value;
			rate = result.rate;
			if(rate === \scalar,{
				// Out, SendTrig etc. probably a 0.0
				result
			},{
				if(rate === \audio,{
					Out.ar(0,result)
				},{
					// allocate a private control bus ?
					// on what server ?
					// you might have played it for its side effects
					result
				})
			})
		})
	}
	play { arg key, server, mixToPresent=false, onComplete, latency;
		var def, synth;
		server = server ? Server.local;
		if(key.isNil,{ 
			def = this.asSynthDef;
			synth = Synth.basicNew(def.name,server);
			def.send(server, synth.newMsg);
			^synth
		}, {
			// this couples the Common library to JITLibrary
			^this.send(key, server, mixToPresent, onComplete, latency)
		})
	}
	
}

