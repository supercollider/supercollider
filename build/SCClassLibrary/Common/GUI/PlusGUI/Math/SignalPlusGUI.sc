+ Signal {
	plot { arg name, bounds;
		//this.asciiPlot;
		super.plot(name, bounds);
		
	}
}

+ ArrayedCollection{
	
	plot { arg name, bounds, discrete;
		//this.asciiPlot;
		var plotter, val, minval, maxval, txt, window, thumbsize;
		bounds = bounds ?  Rect(200 , 140, 705, 410);
		discrete = discrete ? false;
		if(discrete){
			thumbsize = (bounds.width-8) / this.size;
		}{
			thumbsize = 1;
		};
		name = name ? "plot";
		minval = this.minItem;
		maxval = this.maxItem;
		val = Array.newClear(this.size);
		this.do({|v, i|
			val.put(i, v.linlin(minval, maxval, 0.0, 1.0)); 
		});
		
		window = SCWindow(name, bounds);
		window.view.decorator =  FlowLayout(window.view.bounds);
		txt = SCStaticText(window, bounds.width @18)
			.string_("index: 0, value: " ++ this[0].asString);
		window.view.decorator.nextLine;
		plotter = SCMultiSliderView(window, Rect(0, 0, bounds.width-8, bounds.height - 60))
			.readOnly_(true)
			.drawLines_(discrete.not)
			.drawRects_(discrete)
			.thumbSize_(thumbsize) 
			.valueThumbSize_(1)
			.colors_(Color.black, Color.blue(1.0,1.0))
			.action_({|v| 
				txt.string_("index: " ++ v.index.asString ++ 
					", value: " ++ v.currentvalue.linlin(0.0, 1.0, minval, maxval).asString) })
			.value_(val)
			.resize_(5)
			.elasticMode_(1);
		window.front;
		
	}
}

+ Wavetable {
	plot { arg name, bounds;
	}
}