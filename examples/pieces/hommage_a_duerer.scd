

// Vera Molnar: Hommage à Duerer (1948-92)

(
w = Window.new("Vera Molnar: Hommage a Duerer (1948-92)", Rect(20, 20, 720, 720), false).front;
w.view.background_(Color.white);
x = Array.fill2D(4, 4, _ @ _).flat.scramble * 20;
w.drawFunc = {
	Pen.strokeColor = Color.black;
	Pen.translate(10, 10);
	9.do {
		9.do {
			Pen.moveTo(x.first);
			x.do { |point, i|
				Pen.lineTo(point);
			};
			x = x.scramble;
			Pen.translate(80, 0);
			Pen.stroke;
		};
	Pen.translate(80 * -9, 80);
	};
};
w.refresh;
)
