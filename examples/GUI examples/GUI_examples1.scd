// GUI examples

////////////////////////////////////////////////////////////////////////

(
// create a window. set properties from a routine.
// note: alpha transparency works on Mac OS X only!
var w, r;
w = Window("trem", Rect(512, 256, 360, 130));
w.front;
r = Routine({
	60.do({ arg i;
		0.05.wait;
		w.bounds = w.bounds.moveBy(10.rand2, 10.rand2);
		w.alpha = cos(i*0.1pi)*0.5+0.5;
	});
	1.wait;
	w.close;
});
AppClock.play(r);
)

////////////////////////////////////////////////////////////////////////

(
// create an array of buttons.
var w;

w = Window("button panel", Rect(128, 64, 340, 360));
w.view.decorator = FlowLayout(w.view.bounds);
32.do({ arg i;
b = Button(w, Rect(0,0, 75, 24));
b.states = [["Start "++i, Color.black, Color.rand],["Stop "++i, Color.white, Color.red]];

});
//w.view.background = Color(0.6,0.8,0.8);
w.front;
)

////////////////////////////////////////////////////////////////////////

(
// create a GUI window with some NumberBoxes.
// You can command click (CocoaGUI) or control click (SwingGUI) on a control
// to drag its value to another control
var w, n, f, s;
w = Window("number box test", Rect(128, 64, 260, 80));
w.view.decorator = f = FlowLayout(w.view.bounds);

n = NumberBox(w, Rect(0,0,80,24));
n.value = 123;

n = NumberBox(w, Rect(0,0,80,24));
n.value = 456;

n = DragBoth(w, Rect(0,0,80,24));
n.object = 789;

f.nextLine;

s = Slider(w, Rect(0,0,240,24));

w.front;
)

////////////////////////////////////////////////////////////////////////

(
// create a GUI for a mixer.
var w, v, s, n;

w = Window("mixer", Rect(128, 64, 340, 360));
w.view.decorator = f = FlowLayout(w.view.bounds,Point(4,4),Point(4,2));
17.do({ arg i;
	var v, s, n, spec, p, height = 16;

	v = StaticText(w, Rect(0, 0, 56, height+2));
	if (i == 0, {
		v.font = Font("Helvetica", 13).boldVariant;
		v.stringColor = Color.yellow;
	},{
		v.font = Font("Helvetica", 12);
		v.stringColor = Color.white;
	});
	v.align = \right;
	s = Slider(w, Rect(0, 0, 182, height));
	s.resize = 2;
	spec = \db.asSpec;
	s.action = {
		var db;
		db = spec.map(s.value).round(0.1);
		//v.string = if (db < -100, { "-inf" },{ db.asString ++ " dB" });
		v.string = db.asString ++ " dB" ;
	};
	s.action.value;

	n = StaticText(w, Rect(0, 0, 72, height+2));
	if (i == 0, {
		n.string = "Main";
		n.stringColor = Color.yellow;
		n.font = Font("Helvetica", 13).boldVariant;
	},{
		n.string = "Channel " ++ i;
		n.stringColor = Color.white;
		n.font = Font("Helvetica", 12);
	});
	n.background = Color.rand(0.1,0.5);
	n.resize = 3;

	f.nextLine;
});
w.view.background = Color.blue;
w.view.background = HiliteGradient(Color.blue, Color.green, \h, 128, 0.3);
w.front;
w.refresh;
)

////////////////////////////////////////////////////////////////////////

(
// jumble of controls

w = Window.new;

a = Slider(w, Rect(20,20,114,24));

b = Button(w, Rect(20, 50, 80, 24));

b.states = [["Start Sound", Color.red, Color.grey(0.9)],["Stop Sound", Color.white, Color.grey(0.4)]];

c = DragSource(w, Rect(120, 50, 80, 24));
c.string = "DragSource";
c.font = Font("Helvetica", 12);
c.beginDragAction = { arg view; [view.object,view.object]; };
//c.stringColor = Color(0.5, 0.2, 0.7);


d = DragSink(w, Rect(120, 85, 80, 24));
d.string = "DragSink";

z = CompositeView(w, Rect(0,100,800,300));

l = Slider(z, Rect(20,120,180,24));

//l.backColor = Color(0,1,0);
//l.knobColor = Color(1,0,0);
l.step = 1/10;

r = RangeSlider(z, Rect(20,170,180,24));
r.step = 1/20;

q = RangeSlider(z, Rect(220, 170, 24, 180));

t = Slider2D(w, Rect(220,20, 80, 80));
t.action = { arg slider; c.object = slider.y.postln.round(0.001); };

p = Slider(z, Rect(248, 170, 24, 180));
p.action = { arg slider; slider.value.postln; };

w.front;
w.refresh;
)

// properties you can set in the window above.
w.view.background = HiliteGradient(Color.blue, Color.red, \v, 128, 0.2);
r.background = Color.gray; r.hi = 0.2;

z.visible = false;
z.visible = true;

z.enabled = false;
z.enabled = true;

b.visible = false;
b.visible = true;

b.enabled = false;
b.enabled = true;

c.visible = false;
c.visible = true;

b.enabled = false;
b.visible = false;
b.visible = true;
b.enabled = true;

t.enabled = false;
t.enabled = true;
t.visible = false;
t.visible = true;

r.step = 0;

t.canFocus = false;
t.canFocus = true;
t.canFocus = false;

c.object = Point(3,4);

c.object = [1, 2, 3, 4];
c.object = 123.456;
c.object = WhiteNoise.ar(1);
c.object = WhiteNoise;
c.object = true;

t.resize = 2;
t.resize = 5;
t.resize = 7;

w.alpha = 0.7;
w.close;

////////////////////////////////////////////////////////////////////////

// create a window to start and stop a synth

(
SynthDef("moto-rev", {
	var x;
	x = RLPF.ar(LFPulse.ar(SinOsc.kr(0.2, 0, 10, 21), [0,0.1], 0.1), 100, 0.1).clip2(0.4);
	Out.ar(0, x);
}).add;
)

(
	var w, b1, b2, b3, name, a, s;
	name = "moto-rev";
	s = Server.local;
	w = Window(name.asString, Rect(128, 64, 260, 80));
	w.view.decorator = FlowLayout(w.view.bounds);

	b1 = Button(w, Rect(0,0, 80, 24));
	b1.states = [["Load", Color.black, Color.green]];
	b1.action = { arg view;
		s.sendMsg("/d_load", SynthDef.synthDefDir ++ name ++ ".scsyndef");
	};
	b1.enabled = s.serverRunning;

	b2 = Button(w, Rect(0,0, 80, 24));
	b2.states = [
		["Play", Color.black, Color.green],
		["Stop", Color.white, Color.red],
	];
	b2.action = { arg view;
		if (b2.value == 1, {
			s.sendMsg("/s_new", name, 1001, 1, 0);
		},{
			s.sendMsg("/n_free", 1001);
		});
	};
	b2.enabled = s.serverRunning;

	a = SimpleController(s);
	f = { arg server;
		b1.enabled = server.serverRunning;
		b2.enabled = server.serverRunning;
	};
	a.put(\serverRunning, f);
	w.onClose = {
		a.remove;
		s.sendMsg("/n_free", 1001);
	};
	w.front;
)

s.boot;


////////////////////////////////////////////////////////////////////////

// GUI to control a synth

(
SynthDef("trem", { arg freq=800, rate=8, amp=0.1;
	var osc;
	freq = Lag.kr(freq, 0.3);
	osc = LFTri.ar(freq, 0, SinOsc.kr( rate + [0,0.1],0,amp).max(0));
	Out.ar(0, osc)
}).add;
)

(
var w, f, d, values, server, id, isOn = false;
var b1, b2, s;

values = IdentityDictionary.new;
server = Server.local;

f = { arg name, spec = \lin, guispec;
	var height = 20, v, s, n;
	guispec = guispec ? spec;
	spec = spec.asSpec;
	guispec = guispec.asSpec;

	v = StaticText(w, Rect(0, 0, 72, height));
	v.font = Font("Helvetica", 12);
	v.stringColor = Color.black;
	v.align = \right;

	s = Slider(w, Rect(0, 0, 182, height));
	s.resize = 2;
	s.action = {
		var val, guival, step;
		val = spec.map(s.value);
		values.put(name, val);
		if (isOn, { server.sendMsg("/n_set", id, name, val); });
		guival = guispec.map(s.value);
		step = pow(10, floor(min(0, guival.abs.log10 - 2)));
		v.string = guival.round(step).asString ++ guispec.units;
	};
	s.value = spec.unmap(spec.default);
	s.action.value;

	n = StaticText(w, Rect(0, 0, 72, height));	n.string = name;
	n.stringColor = Color.black;
	n.font = Font("Helvetica", 12);
	n.resize = 3;

	w.view.decorator.nextLine;
};

id = 2001;
w = Window("trem", Rect(512, 64, 360, 130));
w.view.decorator = d = FlowLayout(w.view.bounds);

b1 = Button(w, Rect(0,0, 80, 24));
b1.states = [["Load", Color.black, Color.green]];
b1.action = { arg view;
	server.sendMsg("/d_load", "synthdefs/trem.scsyndef");
};
b1.enabled = server.serverRunning;

b2 = Button(w, Rect(0,0, 80, 24));
b2.states = [
	["Play", Color.black, Color.green],
	["Stop", Color.white, Color.red],
];
b2.action = { arg view;
	var msg;
	if (view.value == 1, {
		isOn = true;
		msg = ["/s_new", "trem", 2001, 1, 0];
		values.keysValuesDo({ arg key, value;
			msg = msg.addAll([key, value]);
		});
		server.performList(\sendMsg, msg);
	},{
		isOn = false;
		server.sendMsg("/n_free", 2001);
	});
};
b2.enabled = server.serverRunning;
d.nextLine;

f.value(\freq, \freq);
f.value(\rate, \lofreq);
f.value(\amp, \amp, \db);

a = SimpleController(server);
f = { arg server;
	b1.enabled = server.serverRunning;
	b2.enabled = server.serverRunning;
	if (server.serverRunning.not, { b2.value = 0 });
};
a.put(\serverRunning, f);
w.onClose = {
	if (isOn, { server.sendMsg("/n_free", 2001) });
 	a.remove;
};

w.front;
)

////////////////////////////////////////////////////////////////////////


(
// create a GUI window with some SCPopUpMenus.
var w, n, f, s, a;
w = Window("popup menu test", Rect(128, 64, 260, 110));
w.view.decorator = f = FlowLayout(w.view.bounds);
a = ["linear", \exponential, "very long item name", \sine, "welch", "curve -2", "db fader", "squared", "cubed"];

n = PopUpMenu(w, Rect(0,0,80,22));
n.items = a;

n = PopUpMenu(w, Rect(0,0,80,22));
n.items = a;

n = PopUpMenu(w, Rect(0,0,80,22));
n.items = a;

f.nextLine;

n = PopUpMenu(w, Rect(0,0,80,22));
n.items = a;
n.value = 5;

n = PopUpMenu(w, Rect(0,0,80,22));
n.items = a;
n.background = Color.red;

n = PopUpMenu(w, Rect(0,0,80,22));
n.items = a;
n.background = Color.blue;
n.stringColor = Color.white;
n.value = 3;
n.action = { arg view; view.value.postln; };

f.nextLine;

s = Slider(w, Rect(0,0,240,24));

w.front;
)
