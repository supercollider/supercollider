
// james mccartney

/*
Triggering functions from the Keyboard

Drag Functions to keys in the window.
Typing keys will execute the Function.

If the Function returns another Function, the new Function will replace the action of a key.

*/

GUI.cocoa;	// use CocoaGUI (Mac OS X native)
GUI.swing;	// use SwingGUI (Java based GUI)

(
var w; // window object
var courier; // font object

// an Array of Strings representing the key layout.
var keyboard = #["`1234567890-=", "QWERTYUIOP[]\\",
							"ASDFGHJKL;'", "ZXCVBNM,./"];

// horizontal offsets for keys.
var offsets = #[42, 48, 57, 117];

var actions; // an IdentityDictionary mapping keys to action functions.
var makeKey; // function to create an SCDragSink for a key.

courier = Font("Courier", 14).boldVariant;

// an IdentityDictionary is used to map keys to functions so that
// we can look up the action for a key
actions = IdentityDictionary.new; // create actions dictionary

// define a function that will create an SCDragSink for a key.
makeKey = {|char, keyname, bounds|
	var v, clz;

	keyname = keyname ? char.asString;
	bounds = bounds ? (24 @ 24);

	v = DragBoth(w, bounds);
	v.font = courier;
	v.string = keyname;
	v.align = \center;
	v.setBoth = false;
	clz = View;
	v.canReceiveDragHandler = {
		clz.currentDrag.isKindOf(String)
	};
	v.action = {
		var function = v.object.interpret;
		("added key action : " ++ keyname).postln;
		if (char.isAlpha) {
			actions[char.toUpper] = function;
			actions[char.toLower] = function;
		}{
			actions[char] = function;
		};
		w.front;
	};
};

w = Window("keyboard", Rect(128, 320, 420, 150));

w.view.decorator = FlowLayout(w.view.bounds);

// define a function to handle key downs.
w.view.keyDownAction = {|view, char, modifiers, unicode, keycode|
	 var result;

	// call the function
	result = actions[char].value(char, modifiers);

	// if the result is a function, that function becomes the
	// new action for the key
	if (result.isKindOf(Function)) {
		actions[char] = result;
	};
};

// make the rows of the keyboard
keyboard.do {|row, i|
	row.do {|key| makeKey.(key) };
	if (i==0) { makeKey.(127.asAscii, "del", 38 @ 24) };
	if (i==2) { makeKey.($\r, "rtrn", 46 @ 24) };
	w.view.decorator.nextLine;
	w.view.decorator.shift(offsets[i]);
};

// make the last row
makeKey.($ , "space", 150 @ 24);
makeKey.(3.asAscii, "entr", 48 @ 24);

w.front;
)




////////////////////

// Drag these things to the keyboard to test it.

(
{
	var synth, original;
	original = thisFunction;
	synth = { SinOsc.ar(exprand(500,1200),0,0.2) }.play;
	{ synth.free; original }
}
)


(
{
	{
		Pan2.ar(
			SinOsc.ar(
				ExpRand(300,3000),
				0,
				SinOsc.kr(ExpRand(1,15),0,0.05).max(0)),
			Rand(-1,1))
	}.play;
}
)

{ s.sendMsg(\n_free, \h, 0); } // kill head

{ s.sendMsg(\n_free, \t, 0); } // kill tail

(
{{
	var eg, o, freq, noise;
	eg = EnvGen.kr(Env.linen(0.1,2,0.4,0.2), doneAction: 2);
	freq = Rand(600,1200);
	noise = {LFNoise2.ar(freq*0.1, eg)}.dup;
	o = SinOsc.ar(freq,0,noise);
	Out.ar(0, o);
}.play})


(
{{
	var in, sr;
	in = LFSaw.ar([21000,21001], 0, LFPulse.kr(ExpRand(0.1,1),0,0.3,0.2,0.02));
	sr = ExpRand(300,3000) + [-0.6,0.6];
	Out.ar(0,  RLPF.ar(in * LFPulse.ar(sr, 0, MouseY.kr(0.01, 0.99)), sr * (LFPulse.kr(ExpRand(0.1,12),0,0.4,0.2,0.2) + LFPulse.kr(ExpRand(0.1,12),0,0.7,0.2)), 0.1));
}.play;})

(
{{ var in;
	in = In.ar(0,2);
	ReplaceOut.ar(0, CombN.ar(in, 0.24, 0.24, 8, 1, in.reverse).distort);
}.play})

(
{{ var in;
	in = In.ar(0,2);
	ReplaceOut.ar(0, in * SinOsc.ar(MouseX.kr(2,2000,1)));
}.play})
