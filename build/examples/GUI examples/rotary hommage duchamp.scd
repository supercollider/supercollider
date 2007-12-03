
// rotary hommage ˆ Duchamp
// Julian Rohrhuber 2006
(
		var  w, width, center, bounds, a, b, c, z = 0, colors, fillOrStroke, shape;
		width = 1;
		w = GUI.window.new("o").front;
		w.view.background_(Color.black);
		colors = [Color.new255(255, 150, 0).alpha_(0.5), Color.black.alpha_(0.5)];
		fillOrStroke = [GUI.pen.fillOval(_), GUI.pen.strokeOval(_)];
		shape = { (2 ** rrand(1, 5)).asInteger + rrand(0, 4) }.dup(rrand(5, 7));
		shape = shape.sort.reverse;
		w.drawHook = {
			GUI.pen.width = width;
			center = 30;
			GUI.pen.translate(0, 0);
			z = z + 1;
			
			GUI.pen.beginPath; 
			bounds = Rect.aboutPoint(w.view.bounds.center, 100, 100);
			shape.do { |i, j|
				var bb;
				a = sin(z * 0.12 + (j / 10));
				b = sin(z * 0.12 + 0.5pi + (j / 4));
				c = sin(z * 0.005);
				bounds = bounds.insetBy(i, i);
				bb = bounds.moveBy(13 * a, 13 * b);
				colors.wrapAt(i).set;
				GUI.pen.width = 30.rand;
				fillOrStroke.wrapAt(i).value(bb);
			};
			GUI.pen.stroke;
			
		};
		w.refresh;
			fork {Êloop {Ê0.04.wait; defer {Êw.refresh; } } };		
)
