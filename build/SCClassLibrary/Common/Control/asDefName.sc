
+ Object {

	asSynthDef {
		error("Cannot convert this object to a SynthDef");
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
		// could cycle through some integers per session
		// so these get overwritten.
		// can we save and load to /tmp ?
		^SynthDef("def" ++ this.identityHash.asString,{
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
}

+ SynthDef {

	asSynthDef { ^this }
	asDefName {	^name	}
	
}

