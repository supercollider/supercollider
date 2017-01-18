// strike
// click and drag to draw
// Julian Rohrhuber, 2006


// neon
(
var xx, yy, x, y, c, width, z=1, pp, zz=0, mm=10, alph=1.0;
width = 1;
w = Window("u", Rect(100, 100, 600, 600)).front;
w.view.background_(Color.gray(0.6));

v = UserView(w, w.view.bounds).mouseMoveAction_({|v,ax,ay|
	x = ax; y = ay;

	4.do {
		[xx, yy].choose.value([5, -5, 2, -2, 8, -8].choose * 2);

	};

});

x = 200; y = 100;
xx = { |d=1| c = c.add((x = x + d) @ y ).keep(-150) };
yy = { |d=1| c = c.add(x @ (y = y + d)).keep(-150) };
c = [];
20.do {
	[xx, yy].choose.value([10, -10, 5, -5, 2, -2].choose * 2);

};

w.drawFunc = {
	// set the Color
	try { Pen.smoothing_(false) };
	Pen.width = width;
	z.do { |i|
		Pen.strokeColor = Color.rand.alpha_(i.linexp(0, z-1, 1.0, alph));
		Pen.moveTo(c[0]);
		c.size.do { |i| Pen.lineTo(c.wrapAt(i)) };
		Pen.stroke;
		Pen.translate([-2, -2, 2, 2] @@ zz, [-2, 2, -2, 2]*2 @@ zz);

	};
	z = z + 1 % mm;
	if(z % mm == 0) { zz = zz + 1; mm = rrand(5, 20); alph = #[0.1, 1.0].choose };
};
w.refresh;
fork { loop { 0.1.wait; defer { w.refresh; } } };

)






// "tetris"
// click and drag to draw
(
var xx, yy, x, y, c, d, width, viewheight;
var phunz, steps, nKeep;

q = ();

width = 1;
w = Window("u", Rect(100, 100, 400, 400)).front;
w.view.background_(Color.black);

viewheight = w.bounds.height;
nKeep = 20;
x = 200;
y = 100;
steps = #[1, 1, 2, 2, 2, 2, 4];

v = UserView(w, w.view.bounds);
v.mouseMoveAction_({|v,ax,ay|
	x = ax; y = ay;
	phunz.(8);
	q.updateData(d);
});
v.mouseUpAction_({|v,ax,ay|
	q.sendData(d);
});

phunz = { arg n=1;
	var scale = y.linexp(0, viewheight, 1, 40);
	c = [];
	n.do {
		[xx, yy].choose.value(steps.choose * #[1, -1].choose * scale);

	};
	d = d.add(c).keep(nKeep.neg);
};

xx = { |d=1| c = c.add((x = x + d) @ y ) };
yy = { |d=1| c = c.add(x @ (y = y + d)) };

phunz.(20);
w.drawFunc = {

	try { Pen.smoothing_(false) };

	Pen.width = width;

	d.do { |x, i|
		Pen.moveTo(x[0]);
		x.do { |point|
			Pen.strokeColor = blend(
				Color.green,
				Color.yellow,
				point.y.linlin(0, viewheight, 1, -1)
			).alpha_(point.y.linexp(0, viewheight, 0.5, 0.1));
			Pen.lineTo(point);
		};
		Pen.stroke;
	};
	d = d.deepCollect(2, { |x| x.y = x.y + 0.5 % viewheight });

};
w.refresh;
{ loop { 0.015.wait; w.refresh }}.fork( AppClock );
w.onClose = { Ndef(\tetris).clear(2) };
)



// sound for "tetris"

(
q[\sendData] = {|q, data|
	Ndef(\tetris, {
		var ugens;
		data.do { |array|
			var xdata, ydata, xmul, ymul;

			array.do { |point|
				xdata = xdata.add(point.x);
				ydata = ydata.add(point.y);
			};
			xmul = 11;
			ymul = 1;
			ugens = ugens.addAll([
				LFPulse.ar(Duty.ar(0.4, 0, Dseq(xdata, inf) * xmul), 0, 0.5)
				,
				Formant.ar(Duty.ar(0.5 * ymul, 0, Dseq(ydata, inf) * ymul), 500, Duty.ar(0.02 * xmul, 0, Dseq(xdata, inf) * xmul))
			]);
		};
		LPF.ar(ugens.mean, 3000) * 0.2 ! 2
	}).play;

};
);
