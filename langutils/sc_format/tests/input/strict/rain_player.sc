// Plays rain drops on the server, with a density that can change while it plays.  


RainPlayer {
classvar <defaultDensity = 8;
  var <density, <low, <high, player;
  


    *new { |density, low = 800, high = 4000|
        ^super.newCopyArgs(density ? defaultDensity, low, high)
    }

	*initClass {
	StartUp.add {
		SynthDef(\rainDrop, { |out = 0, freq = 2000, amp = 0.1, pan = 0|
			var env = Env.perc(0.001, 0.08).ar(2);   
			var sig = SinOsc.ar(freq * Line.ar(1.5, 1, 0.02))
			* env
			* amp;
			Out.ar(out, Pan2.ar(sig, pan));
		}).add;
	}
	}

  pattern {
    ^Pbind(
      \instrument, \rainDrop,
      \dur, Pfunc { density.reciprocal } * Pwhite(0.5, 1.5),
      \freq, Pfunc { exprand(low, high) },
      \amp, Pwhite(0.02, 0.1)
    )
  }	

play { |quant = 1|
if(player.notNil) { ^this };
player = this.pattern
.collect { |event| event.put(\pan, 1.0.rand2) }
.play(quant: quant);
}



	stop {
			player !? { player.stop };
	  player = nil;
	}

    density_ { |value|
            density = value.clip(1, 32);
        this.changed(\density);
    }

        // The notes of a scale between low and high, as frequencies.
	scaleFreqs { |scale|
		^(low.cpsmidi.round.asInteger..high.cpsmidi.round.asInteger)
		.select { |note| scale.includes(note % 12) }
		.collect { |note| note.midicps }
	}
}