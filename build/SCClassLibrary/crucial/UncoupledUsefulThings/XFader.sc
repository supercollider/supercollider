
XFader : UGen  {  // UNIPOLAR 		// slightly cheaper than LinXFade2 ( no clip )		// and the name is easier to remember			*ar { arg l, r, pan;
		^(l + (pan * (r - l)))	}	*kr { arg l, r, pan;		^(l + (pan * (r - l)))
	}}XFader4 : UGen {	// quad stereo cross fade	*ar { arg lf, rf, lb, rb, xpan, ypan; // unipolar		var front, back;
		front = lf + (xpan * (rf - lf));
		back  = lb + (xpan * (rb - lb));
		^back + (ypan * (front - back))
	}	*kr { arg lf, rf, lb, rb, xpan, ypan;		var front, back;
		front = lf + (xpan * (rf - lf));
		back  = lb + (xpan * (rb - lb));
		^back + (ypan * (front - back))
	}}XFaderN : UGen { //shouldnt be a ugen, it never should expand	*ar { arg inputs, bipolar,width=2.0;		var whiches;
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