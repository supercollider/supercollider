

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
	
	asSynthDef { arg lags, prependArgs, outClass=\Out, xfade=false;
		
		^SynthDef.newWrapOut(this.hash.asString, this, lags, prependArgs, outClass, xfade);	}
		
	play { arg target, xfade=true;
		var def, synth, server;
		target = target.asTarget;
		server = target.server;
		def = this.asSynthDef(xfade:xfade);
		synth = Synth.basicNew(def.name,server);
		server.waitForBoot({
			def.send(server, synth.newMsg(target));
		});
		^synth
	}
	
}

