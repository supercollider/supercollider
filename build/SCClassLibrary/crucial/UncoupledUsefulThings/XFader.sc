
XFader  {  // UNIPOLAR		// XFade2 is now cheaper (in c)
					*ar { arg l, r, pan;
		^(l + (pan * (r - l)))	}	*kr { arg l, r, pan;		^(l + (pan * (r - l)))
	}}XFader4  {	// quad stereo cross fade	*ar { arg lf, rf, lb, rb, xpan, ypan; // unipolar		var front, back;
		front = lf + (xpan * (rf - lf));
		back  = lb + (xpan * (rb - lb));
		^back + (ypan * (front - back))
	}	*kr { arg lf, rf, lb, rb, xpan, ypan;		var front, back;
		front = lf + (xpan * (rf - lf));
		back  = lb + (xpan * (rb - lb));
		^back + (ypan * (front - back))
	}}XFaderN  {
	*ar { arg inputs, bipolar,width=2.0;		var whiches;
		inputs = inputs.dereference;
		whiches = PanAz.ar(inputs.size,1.0,bipolar,width);

		^Mix.ar(
			inputs.collect({ arg sound,i;
				sound * whiches.at(i)
			})
		)
	}	*kr { arg inputs, bipolar,width=2.0;		var whiches;
		inputs = inputs.dereference;
		whiches = PanAz.ar(inputs.size,1.0,bipolar,width);

		^Mix.ar(
			inputs.collect({ arg sound,i;
				sound * whiches.at(i)
			})
		)
	}}