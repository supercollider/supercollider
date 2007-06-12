Win SC3-Complete Test file:

		// text editor features:
Synth		// select and try alt-j
play			// try alt-y
Instr:play	// try alt-j
defer		// try alt-shift-y	to see references

			// posting from a task
(
Task { 
	5.reverseDo {|i| (i+1).postln; 1.wait; };
	"zero!".postln;
}.play;
)

s.boot;
	// server is booted when starting up
{ SinOsc.ar(LFNoise0.kr([8, 12], 200, [300, 400]),0, 0.1) }.play;


	// quit and boot work from lang:
s.quit;

s.boot;
{ SinOsc.ar(LFNoise0.kr([8, 12], 200, [300, 400]),0, 0.1) }.play;
s.reboot;
{ SinOsc.ar(LFNoise0.kr([8, 12], 200, [300, 400]),0, 0.1) }.play;
	// cmdLine string from SCSpawn gets parsed back into server options.
s.options.dump;	

{ "defer works now with appclock".postln }.defer(1);

	// patterns work, no late complaints, trace works
Pbind(\degree, Pxrand((0..7), 20), \dur, 0.15).trace.play;

	// swingOSC GUI: 
	// a larger test from JEZSlider.help
(
var w, startButton, noteControl, cutoffControl, resonControl;
var balanceControl, ampControl;
var id, cmdPeriodFunc;

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
).load(s);

id = s.nextNodeID; // generate a note id.

// make the window
w = JSCWindow("another control panel", Rect(20, 200, 440, 180));
w.front; // make window visible and front window.
w.view.decorator = FlowLayout(w.view.bounds);

//w.view.background = HiliteGradient(Color.rand(0.0,1.0),Color.rand(0.0,1.0),
//					[\h,\v].choose, 100, rrand(0.1,0.9));
//g.sendMsg( "/set", w.view.id, "doubleBuffered", false );

// add a button to start and stop the sound.
startButton = JSCButton(w, 75 @ 24);
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
noteControl = JEZSlider(w, 400 @ 24, "Note", ControlSpec(24, 60, \lin, 1), 
	{|ez| s.sendMsg("/n_set", id, "note", ez.value); }, 36);
	
w.view.decorator.nextLine;
cutoffControl = JEZSlider(w, 400 @ 24, "Cutoff", ControlSpec(200, 5000, \exp), 
	{|ez| s.sendMsg("/n_set", id, "fc", ez.value); }, 1000);
	
w.view.decorator.nextLine;
resonControl = JEZSlider(w, 400 @ 24, "Resonance", ControlSpec(0.1, 0.7), 
	{|ez| s.sendMsg("/n_set", id, "rq", ez.value); }, 0.2);
	
w.view.decorator.nextLine;
balanceControl = JEZSlider(w, 400 @ 24, "Balance", \bipolar, 
	{|ez| s.sendMsg("/n_set", id, "bal", ez.value); }, 0);
	
w.view.decorator.nextLine;
ampControl = JEZSlider(w, 400 @ 24, "Amp", \db, 
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


// Recording also works...
s.prepareForRecord("c:\\temp\\test.aiff");
s.record;
s.stopRecording;

