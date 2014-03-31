//Sam Pluta - 2014
//A poor simulation of LaMonte Young's outstanding Dream House installation, found on Church St. in New York
//Go check out the real thing!
//If you want to understand the math, read the actual title of the piece
//This will not be good in headphones - use speakers and walk around the room


SynthDef("YoungPartial",{arg freq, amp, gate=1;
	var out, env;

	env = EnvGen.kr(Env.asr(0,1,0), gate);

	out = SinOsc.ar(freq, 0, amp)*env;

	Out.ar(0, Pan2.ar(out, Rand(-1,1)));
}).load(s);


(
	var scManta, partials, freqs, sins, textStrings, notesOn, partialsOn, lastSin, differenceList, differenceText, win, octaves, spec;

	partials=IdentitySet.new;		//create a new set

	//add the 9:7:4 base
	partials.add(4.dup);partials.add(7.dup);partials.add(9.dup);

	//add the main band of partials
	(224..288).do{arg i;
		if((i.isPrime),{
			partials.add(i.dup);
		});
	};

	partials.add(["29*(3**2)", 261]); partials.add(["31*(3**2)", 279]);

	//reinforce the base frequencies within the 9:7 major third of the primary band
	partials.add(["2**8", 2**8]);partials.add(["7*(2**5)",7*(2**5)]);partials.add(["9*(2**5)",9*(2**5)]);

	partials.add(31.dup);partials.add(29.dup);partials.add(61.dup);partials.add(59.dup);partials.add(113.dup);

	//add 119
	partials.add(["17*7",119]);

	//add the upper band of partials
	//these are put in different octaves so that they are symetrical with the lower band
	partials.add(["71*(2**3)",71*(2**3)]);partials.add(["17*(2**5)", 17*(2**5)]);
	partials.add(["67*(2**4)",67*(2**4)]);partials.add(["137*(2**3)",137*(2**3)]);partials.add(["131*(2**4)",131*(2**4)]);partials.add(["139*(2**4)",139*(2**4)]);

	partials=partials.asArray.sort({arg a, b; a[1]<b[1]});

partials.postln;


	freqs = partials.flop[1]*7.5;
	//freqs.postln;

	sins = List.new;
	notesOn = List.new;

	partialsOn = partials.size;

	octaves = List.newClear(partials.size);

	spec = ControlSpec(1, 1.28, \exponential);

	partials.do{arg item,i;
		10.do{arg i2;
			if(item[1]/(7/2*(2**i2))>=1,{
				octaves.put(i, [i2, spec.unmap(item[1]/(7/2*(2**i2)))]);
			})
		}
	};

	textStrings = List.new;
	win = Window("Dreamhouse", Window.screenBounds);
	freqs.size.do{arg i;
	textStrings.add(StaticText(win, Rect(Window.screenBounds.width/10*octaves[i][0], Window.screenBounds.height-100*(1-octaves[i][1]), Window.screenBounds.width/10, 20)).font_(Font("Monaco", 9)));
		textStrings[i].string = partials[i].asString+"  "+freqs[i].asString;
	};

	differenceText = StaticText(win, Rect(Window.screenBounds.width*3/4, Window.screenBounds.height*5/8, Window.screenBounds.width/5, Window.screenBounds.height/8));

	win.front;

	win.onClose_(
		{
			sins.do{arg item; item.set(\gate, 0)};
		};
	);

	freqs.do{arg item, i;
		sins.add(Synth("YoungPartial",[\freq, item, \amp, 1/freqs.size]));
		notesOn.add(true);

	};

)
