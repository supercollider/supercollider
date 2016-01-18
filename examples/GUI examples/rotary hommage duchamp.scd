
// rotary hommage à Duchamp
// Julian Rohrhuber 2006
(
	var  w, width, center, bounds, a, b, c, z = 0, colors, fillOrStroke, shape;
	var rate = 25, speed = 33, seed = Date.seed;
	width = 1;
	w = Window("o").front;
	w.view.background_(Color.black);
	colors = [Color.new255(255, 150, 0).alpha_(0.5), Color.black.alpha_(0.5)];
	fillOrStroke = [Pen.fillOval(_), Pen.strokeOval(_)];
	shape = { (2 ** rrand(1, 5)).asInteger + rrand(0, 4) }.dup(rrand(5, 7));
	shape = shape.sort.reverse;
	w.drawFunc = {
		thisThread.randSeed = seed;
		Pen.width = width;
		center = 30;
		Pen.translate(0, 0);
		z = z + (speed / rate);

		Pen.beginPath;
		bounds = Rect.aboutPoint(w.view.bounds.center, 100, 100);
		shape.do { |i, j|
			var bb;
			a = sin(z * 0.1 + (j / 10));
			b = sin(z * 0.13 + (j / 4));
			c = sin(z * 0.45);
			bounds = bounds.insetBy(i, i);
			bb = bounds.moveBy(13 * a, 13 * b);
			Pen.color=colors.wrapAt(i);
			Pen.width = 30.rand;
			fillOrStroke.choose.value(bb);
		};
		Pen.stroke;

	};
	w.refresh;
	fork { loop { rate.reciprocal.wait; defer { w.refresh; } } };
)
