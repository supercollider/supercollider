
/*

Diamond by James McCartney



Partch "otonalities" (i.e. a set of ratios with the same denominator
and changing numerator, i.e. overtone series) run horizontally.

Partch "utonalities" (i.e. a set of ratios with the same numerator and
changing denominator, a.k.a. undertone series) run vertically.

The ratio between a pair of buttons in any two columns is the same for
any row in those columns.
The ratio between a pair of buttons in any two rows is the same for any
column in those rows.
..there must be a less confusing way of stating that...
a set of buttons pressed in one row will be a transposition of the same
columns' buttons pressed in any row.
a set of buttons pressed in one column will be a transposition of the
same rows' buttons pressed in any column.

*/


s.boot;

(
var w, buttons;
var series, series2;

SynthDef("tone", { arg freq = 440, amp=0.1, gate =1;
	var tone, env;
	env = Linen.kr(gate, 0.01, amp, 0.2, 2);
	tone = OnePole.ar(Saw.ar(freq + {Rand(-0.05,0.05)}.dup, env), 0.95);
	Out.ar(0, tone);
}).add;

series = (8..15);
series.remove(13);

series2 = series ++ (2 * series) ++ (4 * (series ++ 16));
series = 4 * (series ++ 16);

w = Window("diamond", Rect(200,200,460,560));
w.view.decorator = FlowLayout(w.view.bounds);

series2.reverse.do {|denom|
	series.do {|numer|
		var rnumer, rdenom;
		var b, gcd, label, ratio, id, freq;
		var color;
		[numer, denom].post;
		rnumer = numer;
		rdenom = denom;
		gcd = rnumer gcd: rdenom;
		rnumer = rnumer div: gcd;
		rdenom = rdenom div: gcd;
		[rnumer, rdenom].postln;
		ratio = rnumer / rdenom;
		freq = 50.midicps * ratio;
		b = Button(w, 50 @ 20);
		label = rnumer.asString ++ " / " ++ rdenom;
		color = if (rnumer.isPowerOfTwo && rdenom.isPowerOfTwo) {Color.red(1,0.4)}{
			if(numer.isPowerOfTwo || denom.isPowerOfTwo) {Color.yellow(1,0.4)}{Color.clear}};
		b.states = [[label, Color.black, color],[label, Color.white, Color.black]];
		b.action = {
			if (b.value == 1) {
				s.sendMsg(\s_new, \tone, id = s.nextNodeID, 0, 1, \freq, freq);
				[\on, rnumer, rdenom].postln
			}{
				s.sendMsg(\n_set, id, \gate, 0);
				[\off, rnumer, rdenom].postln
			};
		};
	};
	w.view.decorator.nextLine;
};
w.front;
)








////////////////////////








// alternate, horizontal layout:

s.boot;

(
var w, buttons;
var series, series2;

SynthDef("tone", { arg freq = 440, amp=0.1, gate =1;
	var tone, env;
	env = Linen.kr(gate, 0.01, amp, 0.2, 2);
	//tone = OnePole.ar(Saw.ar(freq + {Rand(-0.05,0.05)}.dup, env), 0.95);
	tone = OnePole.ar(Saw.ar(freq + {Rand(-0.05,0.05)}.dup, env), 0.8);
	Out.ar(0, tone);
}).add;

series = (8..15);
series.remove(13);

series2 = series ++ (2 * series) ++ (4 * (series ++ 16));
series = (series ++ 16);

w = Window("diamond", Rect(50,200,980,220));
w.view.decorator = FlowLayout(w.view.bounds);

series.reverse.do {|denom|
	series2.do {|numer|
		var rnumer, rdenom;
		var b, gcd, label, ratio, id, freq;
		var color;
		[numer, denom].post;
		rnumer = numer;
		rdenom = denom;
		gcd = rnumer gcd: rdenom;
		rnumer = rnumer div: gcd;
		rdenom = rdenom div: gcd;
		[rnumer, rdenom].postln;
		ratio = rnumer / rdenom;
		freq = 50.midicps * ratio;
		b = Button(w, 40 @ 20);
		label = rnumer.asString ++ "/" ++ rdenom;
		color = if (rnumer.isPowerOfTwo && rdenom.isPowerOfTwo)
{Color.red(1,0.4)}{
			if(numer.isPowerOfTwo || denom.isPowerOfTwo)
{Color.yellow(1,0.4)}{Color.clear}};
		b.states = [[label, Color.black, color],[label, Color.white,
Color.black]];
		b.action = {
			if (b.value == 1) {
				s.sendMsg(\s_new, \tone, id = s.nextNodeID, 0, 1, \freq, freq);
			}{
				s.sendMsg(\n_set, id, \gate, 0);
			};
		};
	};
	w.view.decorator.nextLine;
};
w.front;
)


//////////////////////////////



// This one displays the cyclic cents values for the ratios.



s.boot;

(
var w, buttons, buttonFont;
var series, series2;

SynthDef("tone", { arg freq = 440, amp=0.1, gate =1;
	var tone, env;
	env = Linen.kr(gate, 0.01, amp, 0.2, 2);
	//tone = OnePole.ar(Saw.ar(freq + {Rand(-0.05,0.05)}.dup, env), 0.95);
	tone = OnePole.ar(Saw.ar(freq + {Rand(-0.05,0.05)}.dup, env), 0.8);
	Out.ar(0, tone);
}).add;

series = (8..15);
series.remove(13);

series2 = series ++ (2 * series) ++ (4 * (series ++ 16));
series = (series ++ 16);

w = Window("diamond", Rect(50,200,980,370), resizable: false);
w.view.decorator = FlowLayout(w.view.bounds);

buttonFont = Font("Helvetica", if( GUI.id === \cocoa, 11, 7 )).boldVariant;

series.reverse.do {|denom|
	series2.do {|numer|
		var rnumer, rdenom;
		var b, gcd, label, ratio, id, freq;
		var color;
		[numer, denom].post;
		rnumer = numer;
		rdenom = denom;
		gcd = rnumer gcd: rdenom;
		rnumer = rnumer div: gcd;
		rdenom = rdenom div: gcd;
		[rnumer, rdenom].postln;
		ratio = rnumer / rdenom;
		freq = 50.midicps * ratio;
		b = Button(w, 40 @ 40);
		label = rnumer.asString ++ " / " ++ rdenom ++ "\n" ++
((ratio.ratiomidi * 100).round.asInteger % 1200);
		color = if (rnumer.isPowerOfTwo && rdenom.isPowerOfTwo)
{Color.red(1,0.4)}{
			if(numer.isPowerOfTwo || denom.isPowerOfTwo)
{Color.yellow(1,0.4)}{Color.clear}};
		b.states = [[label, Color.black, color],[label, Color.white,
Color.black]];
		b.font = buttonFont;
		b.action = {
			if (b.value == 1) {
				s.sendMsg(\s_new, \tone, id = s.nextNodeID, 0, 1, \freq, freq);
			}{
				s.sendMsg(\n_set, id, \gate, 0);
			};
		};
	};
	w.view.decorator.nextLine;
};
w.front;
)



/////////////////////////////////
