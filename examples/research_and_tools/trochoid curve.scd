
(
a = 1.0;
b = 1.8;
x = { |phi, a, b| a * phi - (b * sin(phi)) };
y = { |phi, a, b| a - (b * sin(phi + 0.5pi)) };
)

GUI.cocoa;	// use CocoaGUI (Mac OS X native)
GUI.swing;	// use SwingGUI (Java based GUI)

(
		var pen = GUI.pen;
		w = GUI.window.new("Trochoid", Rect(128, 64, 800, 400)).front;
		w.view.background_(Color.white);
		w.drawFunc = {
			pen.strokeColor = Color.black;
			pen.moveTo(0 @ 150);
			((0..900) / 50).do { |phi|
				var x0 = x.(phi, a, b);
				var y0 = y.(phi, a, b);
				pen.lineTo((50 * x0) @ (50 * y0 + 150));
			};

			pen.stroke
		};
		w.refresh;
);


(
Tdef(\x, {
	loop {
		((0..100) / 25).do { |m|
			b = m;
			defer {w.refresh};
			0.1.wait
		}
	}
}).play
)


// sonification of a trochoid


p = ProxySpace.push(s.boot); // enter
~out.play;

(
~out = {
	SinOsc.ar(500).dup * ~modulation.kr(2) * 0.04

};
)

(
~phi = {
	LFSaw.kr(3).range(0, 2pi)
};

~modulation = {
	var x, y;
	var a, b;
	a = 1.0;
	b = XLine.kr(0.1, 2, 30);
	x = { |phi, a, b| a * phi - (b * sin(phi)) }.value(~phi.kr, a, b);
	y = { |phi, a, b| a - (b * sin(phi + 0.5pi)) }.value(~phi.kr, a, b);
	[x, y]
}
)

p.clear.pop; // exit
