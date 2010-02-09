
// rotary hommage ˆ Duchamp
// Julian Rohrhuber 2006
(
		var  w, width, center, bounds, a, b, c, z = 0, colors, fillOrStroke, shape;
		var rate = 25, speed = 33, seed = Date.seed;
		width = 1;
		w = GUI.window.new("o").front;
		w.view.background_(Color.black);
		colors = [Color.new255(255, 150, 0).alpha_(0.5), Color.black.alpha_(0.5)];
		fillOrStroke = [GUI.pen.fillOval(_), GUI.pen.strokeOval(_)];
		shape = { (2 ** rrand(1, 5)).asInteger + rrand(0, 4) }.dup(rrand(5, 7));
		shape = shape.sort.reverse;
		w.drawHook = {
			thisThread.randSeed = seed;
			GUI.pen.width = width;
			center = 30;
			GUI.pen.translate(0, 0);
			z = z + (speed / rate);
			
			GUI.pen.beginPath; 
			bounds = Rect.aboutPoint(w.view.bounds.center, 100, 100);
			shape.do { |i, j|
				var bb;
				a = sin(z * 0.1 + (j / 10));
				b = sin(z * 0.13 + (j / 4));
				c = sin(z * 0.45);
				bounds = bounds.insetBy(i, i);
				bb = bounds.moveBy(13 * a, 13 * b);
				GUI.pen.color=colors.wrapAt(i);
				GUI.pen.width = 30.rand;
				fillOrStroke.choose.value(bb);
			};
			GUI.pen.stroke;
			
		};
		w.refresh;
			fork {Êloop {Êrate.reciprocal.wait; defer {Êw.refresh; } } };		
)
