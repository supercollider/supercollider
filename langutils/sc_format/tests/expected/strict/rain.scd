// Rain on a tin roof: a SynthDef for the drops, a pattern that plays them, and a window to start and stop it.
// Boot the server, then evaluate each block in turn.

s.boot;

(
/*
Each drop is a short sine blip that bends down in pitch.
    Raise \freq for a smaller roof.  
*/
SynthDef(\drop, { |out = 0, freq = 2000, amp = 0.1, pan = 0|
	var env = Env.perc(0.001, 0.08).ar(2);
	var sig = SinOsc.ar(freq * Line.ar(1.5, 1, 0.02))
		* env
		* amp;
	Out.ar(out, Pan2.ar(sig, pan));
}).add;
)

(
~settings = (
	density: 8, // drops a second
	low: 800,
	high: 4000
);

~rain = Pbind(
	\instrument, \drop,
	\dur, Pfunc { ~settings[\density].reciprocal } * Pwhite(0.5, 1.5),
	\freq, Pfunc { exprand(~settings[\low], ~settings[\high]) },
	\amp, Pwhite(0.02, 0.1),
	\pan, Pwhite(-1.0, 1.0)
	// add \legato here for longer drops
);
)

(
var window = Window("Rain", Rect(200, 200, 240, 90))
	.alwaysOnTop_(true)
	.front;
var button = Button(window, Rect(20, 20, 200, 50))
	.states_([
		["Start rain", Color.black, Color.green(0.8)],
		["Stop rain", Color.white, Color.red(0.7)]
	])
	.action_({ |view|
		if(view.value == 1) {
			~player = ~rain.play(quant: 1);
		} {
			~player.stop;
		};
	});

window.onClose = { ~player.stop };
)

(
// Change how hard it rains every few seconds.
~weather = Routine {
	loop {
		~settings[\density] = [2, 4, 8, 16].choose;
		("Rain: % drops a second, "
			"from % to % Hz.").format(~settings[\density], ~settings[\low], ~settings[\high]).postln;
		4.wait;
	}
}.play;
)

(
~scale = (60..84)
	.select { |note| note.odd }
	.collect { |note| note.midicps };
~settings[\low] = ~scale.first;
~settings[\high] = ~scale.last
)
