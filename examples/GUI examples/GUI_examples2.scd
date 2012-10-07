// Creating a window.

w = Window.new("a control panel", Rect(20, 400, 440, 360));
w.front; // make window visible and front window.


// Views are controlled by setting properties.

w.view.background = Color.rand;
w.view.background = Color.rand;
w.view.background = Color.rand;
w.view.background = Gradient(Color.blue,Color.green,\v);
w.view.background = Gradient(Color.black,Color.red,\h);
w.view.background = Gradient(Color.black,Color.red,\h, 16);
w.view.background = Gradient(Color.black,Color.red,\h, 128);
w.view.background = HiliteGradient(Color.blue,Color.yellow,\v);
w.view.background = HiliteGradient(Color.blue,Color.yellow,\v, 16);
w.view.background = HiliteGradient(Color.blue,Color.yellow,\v, 256);
w.view.background = HiliteGradient(Color.red(0.6),Color.green,\h, 16);
w.view.background = HiliteGradient(Color.red(0.6),Color.green,\h, 256);

(
Routine({
	30.do {
		w.view.background =
			HiliteGradient(Color.rand(0.0,1.0),Color.rand(0.0,1.0),
					[\h,\v].choose, 100, rrand(0.1,0.9));
		0.5.wait;
	};
}).play(AppClock);
)


// The FlowLayout decorator places views one after another.

w.view.decorator = FlowLayout(w.view.bounds);

// A button that adds another one like itself.

(
f = {
	// the arguments for creating a view are the window it is in,
	// and the bounds of the view
	b = Button(w, 75 @ 24);
	// states defines the colors and label for the button in different states.
	b.states = [["Add", Color.black, Color.rand]];
	b.action = f;
};
f.value;
)

// A button that changes the background.

(
b = Button(w, 75 @ 24);
b.states = [["Backgnd", Color.white, Color.rand]];
b.action = {
		w.view.background = Color.rand(0.0,1.0); // fallback for SwingOSC
		w.view.background =
			HiliteGradient(Color.rand(0.0,1.0),Color.rand(0.0,1.0),
					[\h,\v].choose, 100, rrand(0.1,0.9));
};
)

// A multi-state button.

(
b = Button(w, 75 @ 24);
b.states = [
	["Red", Color.white, Color.red],
	["Green", Color.black, Color.green],
	["Blue", Color.white, Color.blue],
	["Yellow", Color.black, Color.yellow]
];
b.action = {| view |
	if (view.value == 0) { w.view.background = Color.yellow };
	if (view.value == 1) { w.view.background = Color.red };
	if (view.value == 2) { w.view.background = Color.green };
	if (view.value == 3) { w.view.background = Color.blue };
};
)

// A slider that controls window transparency.

(
w.view.decorator.nextLine;
v = Slider(w, 130 @ 24);
v.action = {| view |
	// Sliders output values from zero to one.
	w.alpha = view.value;
};
v.value = 1;
)

w.alpha = 1;
w.front;

// A slider that controls window width.

(
w.view.decorator.nextLine;
v = Slider(w, 130 @ 24);
v.action = {| view |
	var bounds;
	bounds = w.bounds;
	bounds.width = 400 + (400 * view.value);
	w.bounds = bounds;
};
)

w.close;
w = nil;

// A more useful window.

(
// start server
s.boot;
)

(
// define a synth
SynthDef("window-test", { arg note = 36, fc = 1000, rq = 0.25, bal=0, amp=0.4, gate = 1;
		var x;
		x = Mix.fill(4, {
			LFSaw.ar((note + {0.1.rand2}.dup).midicps, 0, 0.02)
		});
		x = RLPF.ar(x, fc, rq).softclip;
		x = RLPF.ar(x, fc, rq, amp).softclip;
		x = Balance2.ar(x[0], x[1], bal);
		x = x * EnvGen.kr(Env.cutoff, gate, doneAction: 2);
		Out.ar(0, x);
	}, [0.1, 0.1, 0.1, 0.1, 0.1, 0]
).add;
)

(
// make the window
w = Window("another control panel", Rect(20, 400, 440, 360));
w.front; // make window visible and front window.
w.view.decorator = FlowLayout(w.view.bounds);
w.view.background = Color.rand(0.0,1.0); // fallback for SwingOSC
w.view.background = HiliteGradient(Color.rand(0.0,1.0),Color.rand(0.0,1.0),
					[\h,\v].choose, 100, rrand(0.1,0.9));
)

(
// add a button to start and stop the sound.
b = Button(w, 75 @ 24);
b.states = [["Start", Color.black, Color.green],["Stop", Color.white, Color.red]];
b.action = {|view|
		if (view.value == 1) {
			s.sendMsg("/s_new", "window-test", 9999, 0, 0);
		};
		if (view.value == 0) {
			s.sendMsg("/n_set", 9999, "gate", 0);
		};
};
)

(
// add a label
w.view.decorator.nextLine;
v = StaticText(w, 80 @ 24);
v.string = "Note";
v.stringColor = Color.white;
v.align = \right;
)

v.align = \left;
v.align = \center;
v.align = \right;

(
// create a ControlSpec for mapping values to correct range.
~noteSpec = ControlSpec(24, 60, \lin, 1);
// create slider and number views.
~noteSlider = Slider(w, 200 @ 24);
~noteNumBox = NumberBox(w, 64 @ 24);

~noteSlider.step = 1/(60-24);
~noteSlider.action = {|view|
	var note;
	note = ~noteSpec.map(view.value);
	~noteNumBox.value = note;
	s.sendMsg("/n_set", 9999, "note", note);
};

~noteNumBox.action = {|view|
	var note;
	note = view.value;
	s.sendMsg("/n_set", 9999, "note", note);
	~noteSlider.value = ~noteSpec.unmap(note);
};
~noteNumBox.align = \center;
)





/*
That is a lot of work for something simple, so I made a wrapper class.
EZSlider takes care of the details for you.


EZSlider is a wrapper for a SCStaticText, an SCSlider, and an SCNumberBox along with the logic to manage them.
*/

(
// create controls.
w.view.decorator.nextLine;
v = EZSlider(w, 400 @ 24, "Note", ControlSpec(24, 60, \lin, 1),
	{|ez| s.sendMsg("/n_set", 9999, "note", ez.value); });

w.view.decorator.nextLine;
v = EZSlider(w, 400 @ 24, "Cutoff", ControlSpec(200, 5000, \exp),
	{|ez| s.sendMsg("/n_set", 9999, "fc", ez.value); });

w.view.decorator.nextLine;
v = EZSlider(w, 400 @ 24, "Resonance", ControlSpec(0.1, 0.7),
	{|ez| s.sendMsg("/n_set", 9999, "rq", ez.value); });

w.view.decorator.nextLine;
v = EZSlider(w, 400 @ 24, "Balance", \bipolar,
	{|ez| s.sendMsg("/n_set", 9999, "bal", ez.value); });

w.view.decorator.nextLine;
v = EZSlider(w, 400 @ 24, "Amp", \db,
	{|ez| s.sendMsg("/n_set", 9999, "amp", ez.value.dbamp); });
)


/*
There are still some problems:
	- Restarting the sound doesn't remember the slider settings.
	- cmd-period doesn't change the button.
	- Closing window doesn't stop the sound.
Need a more comprehensive approach.
*/


(
var w, startButton, noteControl, cutoffControl, resonControl;
var balanceControl, ampControl;
var id, cmdPeriodFunc;

id = s.nextNodeID; // generate a note id.

// make the window
w = Window("another control panel", Rect(20, 400, 440, 180));
w.front; // make window visible and front window.
w.view.decorator = FlowLayout(w.view.bounds);

w.view.background = HiliteGradient(Color.rand(0.0,1.0),Color.rand(0.0,1.0),
					[\h,\v].choose, 100, rrand(0.1,0.9));

// add a button to start and stop the sound.
startButton = Button(w, 75 @ 24);
startButton.states = [
	["Start", Color.black, Color.green],
	["Stop", Color.white, Color.red]
];
startButton.action = {|view|
		if (view.value == 1) {
			// start sound
			s.sendMsg("/s_new", "window-test", id, 0, 0,
				"note", noteControl.value,
				"fc", cutoffControl.value,
				"rq", resonControl.value,
				"bal", balanceControl.value,
				"amp", ampControl.value.dbamp);
		};
		if (view.value == 0) {
			// set gate to zero to cause envelope to release
			s.sendMsg("/n_set", id, "gate", 0);
		};
};

// create controls for all parameters
w.view.decorator.nextLine;
noteControl = EZSlider(w, 400 @ 24, "Note", ControlSpec(24, 60, \lin, 1),
	{|ez| s.sendMsg("/n_set", id, "note", ez.value); }, 36);

w.view.decorator.nextLine;
cutoffControl = EZSlider(w, 400 @ 24, "Cutoff", ControlSpec(200, 5000, \exp),
	{|ez| s.sendMsg("/n_set", id, "fc", ez.value); }, 1000);

w.view.decorator.nextLine;
resonControl = EZSlider(w, 400 @ 24, "Resonance", ControlSpec(0.1, 0.7),
	{|ez| s.sendMsg("/n_set", id, "rq", ez.value); }, 0.2);

w.view.decorator.nextLine;
balanceControl = EZSlider(w, 400 @ 24, "Balance", \bipolar,
	{|ez| s.sendMsg("/n_set", id, "bal", ez.value); }, 0);

w.view.decorator.nextLine;
ampControl = EZSlider(w, 400 @ 24, "Amp", \db,
	{|ez| s.sendMsg("/n_set", id, "amp", ez.value.dbamp); }, -6);


// set start button to zero upon a cmd-period
cmdPeriodFunc = { startButton.value = 0; };
CmdPeriod.add(cmdPeriodFunc);

// stop the sound when window closes and remove cmdPeriodFunc.
w.onClose = {
	s.sendMsg("/n_free", id);
	CmdPeriod.remove(cmdPeriodFunc);
};

)

