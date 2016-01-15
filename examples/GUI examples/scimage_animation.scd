
(
	// SCImage animation example - from a code of Tom Tlalim, adapted for SCImage by blackrain

	var w, h = 800, v = 600, seed = Date.seed, phase=0, phasef = 0.00125, zoom = 1, zoomf = 1.01, trans=0.1, run = true, i=0, amess, ind=0;
	var size = 100, mess, ordered;
	var image,

	export=false;		// <--- toggle to export frames

	w = Window("animation", Rect(40, 40, h, v));

	w.view.background_(Color.white);
	u = UserView.new(w, Rect(0, 0, h, v));
	w.front;

	// image preparation
	image =  SCImage.new(h@v);
	image.lockFocus;
	Color.black.set;
	Pen.fillRect(Rect(0,0,h,v));
	image.unlockFocus;

	mess = [{1.0.rand2}!200, {1.0.rand2}!200, {1.0.rand2}!200];
	size = mess[0].size;

	u.drawFunc = {

		image.drawAtPoint(0@0);

	};

	{
		while { run } {

			phase = phase+phasef;
			zoom = zoom*zoomf;
			amess = mess;

			//{
				image.lockFocus;

					Pen.use {
						Pen.color = Color.black.alpha_(0.1);
						Pen.fillRect(Rect(0,0,h, v));
						Pen.strokeColor = Color.grey(1,0.7);
						(mess[0].size * (zoom*0.1).min(1)).do {
							var x, y, z;
							#x, y, z = [amess[0][i], amess[1][i],  amess[2][i]];
							Pen.addRect(
								Rect(
								(x*0.5+1)*400,
								(z.neg*0.5+0.75)*400,
								(y.neg)*100 + (z*0.5 +1)*(zoom/5),
								(y.neg)*100  + (z*0.5+1)*(zoom/5)
								)
							);
							i=(i+1)%((mess[0].size));
						};

						Pen.stroke;

						i = (mess[0].size/(zoom));

					}; // End of Pen.use

				image.unlockFocus;

				if(export, {
					image.write(Document.current.path.dirname ++ "/SavedImage_"++ind++".jpeg");
				});

			//}.bench; // bench to tune it

			u.refresh;

			0.05.wait;

			ind=ind+1;
		}

	}.fork(AppClock);

	w.onClose_({

		run = false;
		{
			image.free;
		}.defer(0.3); // enough to release safely

	});
)
