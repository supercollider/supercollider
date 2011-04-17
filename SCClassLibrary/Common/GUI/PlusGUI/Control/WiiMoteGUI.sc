/* class to display wiimote data as a gui */

WiiMoteGUI {
	classvar <>xposScreen=0, <>yposScreen=20;
	classvar <counter = 0;
	var <>w, <>wiimote, <watcher;

	// subwindows:
	var <rm, <nc, <cl;

	*new { |wiimote, w|
		^super.new.w_(w).wiimote_(wiimote).init;
	}

	init {
		var f, xsize, ysize;
		var ncview, rmview, clview;
		counter = counter + 1;

		//		xsize = 64 + ((2*44 + 2)*2) + 2;
		xsize = 210;
		ysize = 105+130+10;
		//		ysize = (10+1) * 34 + 2;


		//		labelColor = Color.white.alpha_(0.4);
		w = w ?? {
			w = GUI.window.new("WiiMote Control", Rect(xposScreen, yposScreen, xsize + 10, ysize)).front;
			//w.view.background_(Color.black);
			w.view.decorator = FlowLayout(Rect(4, 4, w.bounds.width, w.bounds.height), 2@2, 2@2);
			w;
		};

		// unclutter the windows on the screen:
		yposScreen = yposScreen + 160;
		if ( yposScreen > 700,
			{ yposScreen = 20; xposScreen = xposScreen + xsize + 10;
				if ( xposScreen > 900,
					{ xposScreen = 0; });
			});

		GUI.staticText.new(w, Rect(0, 0, xsize - 2, 20)).string_("WiiMote" + wiimote.id + wiimote.address )
			.align_(0);
		//.background_(labelColor);

		rmview = GUI.compositeView.new( w, Rect( 5, 30, 205, 130 ));
		rm = WiiRemoteGUI.new( rmview, wiimote, 0 ); //30 );

		ncview = GUI.compositeView.new( w, Rect( 5, 160, 205, 105 ));
		nc = WiiNunchukGUI.new( ncview, wiimote, 0 ); // 160 );

		watcher = SkipJack.new( { this.updateVals }, 0.1, { w.isClosed }, (\wiimote_gui_ ++ counter));
		watcher.start;

	}

	updateVals {
		{
			rm.updateVals;
			if ( wiimote.ext_type == 1, {
				nc.updateVals;
				});
			//			cl.updateVals;
		}.defer;
	}

	hide{
		if ( GUI.scheme.id == \swing,
			{
				w.visible_( false );
			},{
				w.close;
			});
		watcher.stop;
	}

	show{
		if ( GUI.scheme.id == \swing,
			{
				w.visible_( true );
			});
		watcher.start;
	}

}

WiiRemoteGUI{
	var <>wiimote;
	var <>w;
	var <buttons, <sliders, <led, <rumble;
	var <onColor, <offColor;

	*new{ |w,wiimote,offset|
		^super.new.wiimote_(wiimote).w_(w).init( offset );
	}

	init{ |offset=0|
		var xpos,ypos;
		xpos = 5; ypos = offset+5;

		onColor = Color.yellow;
		offColor = Color.white;

		buttons = Array.fill( 11, 0 );

		[ "A", "B", "1", "2", "-", "home", "+" ].do{ |it,i|
			var size;
			size = 20;
			if ( it == "home", { size = 45 } );
			buttons[i] = GUI.staticText.new( w, Rect( xpos, ypos, size, 20 ) ).string_( it ).background_(onColor).align_( 0 );
			xpos = xpos + size + 5;
		};

		xpos = 5;
		ypos = ypos + 25;

		buttons[9] = GUI.staticText.new( w, Rect( xpos, ypos+25, 20, 20 ) ).string_( "<-" ).background_(onColor).align_( 0 );
		xpos = xpos + 25;
		buttons[7] = GUI.staticText.new( w, Rect( xpos, ypos, 20, 20 ) ).string_( "^" ).background_(onColor).align_( 0 );
		buttons[8] = GUI.staticText.new( w, Rect( xpos, ypos+50, 20, 20 ) ).string_( "v" ).background_(onColor).align_( 0 );
		xpos = xpos + 25;
		buttons[10] = GUI.staticText.new( w, Rect( xpos, ypos+25, 20, 20 ) ).string_( "->" ).background_(onColor).align_( 0 );

		xpos = xpos + 25;

		sliders = wiimote.remote_motion.collect{ |it,i|
			xpos = xpos + 25;
			GUI.slider.new( w, Rect( xpos-25, ypos, 20, 70 ) );
		};

		sliders = sliders.add( GUI.slider.new( w, Rect( xpos, ypos, 20, 70 ) ) );

		ypos = ypos + 75;
		xpos = 5;

		led = wiimote.remote_led.collect{ |it,i|
			xpos = xpos + 25;
			GUI.button.new( w, Rect( xpos-25, ypos, 20, 20 ) )
			.states_( [ [ "X", Color.black, Color.yellow ],["O", Color.yellow, Color.black ] ] )
			.action_( { |but| wiimote.setLEDState( i, but.value ) } );
		};

		rumble = GUI.button.new( w, Rect( xpos, ypos, 70, 20 ) )
		.states_( [ [ "rumble", Color.black, Color.yellow ],["RUMBLING", Color.yellow, Color.black ] ] )
		.action_( { |but| wiimote.rumble( but.value ) } );
	}

	updateVals{
		led.do{ |it,i| it.value = wiimote.remote_led[i] };
		//rumble.value = wiimote.rumble;
		wiimote.remote_motion.do{ |it,i| sliders[i].value = it };
		//sliders[3].value = wiimote.remote_motion[3]/3;
		sliders[4].value = wiimote.battery;
		wiimote.remote_buttons.do{ |it,i|
			if ( it == 1 ,
				{
					buttons[i].background = onColor;
				},{
					buttons[i].background = offColor;
				});
		};
	}
}

WiiNunchukGUI{
	var <>wiimote;
	var <>w;
	var <buttons, <sliders, <slider2d;
	var <onColor, <offColor;

	*new{ |w,wiimote,offset|
		^super.new.wiimote_(wiimote).w_(w).init( offset );
	}

	init{ |offset=0|
		var xpos,ypos;
		xpos = 5; ypos = offset+5;

		onColor = Color.yellow;
		offColor = Color.white;

		buttons = Array.fill( 2, 0 );

		[ "Z", "C" ].do{ |it,i|
			buttons[i] = GUI.staticText.new( w, Rect( xpos, ypos, 20, 20 ) ).string_( it ).background_(onColor).align_( 0 );
			ypos = ypos + 25;
		};

		xpos = xpos + 25;
		ypos = ypos - 50;

		sliders = wiimote.nunchuk_motion.collect{ |it,i|
			xpos = xpos + 25;
			GUI.slider.new( w, Rect( xpos-25, ypos, 20, 70 ) );
		};

		slider2d = GUI.slider2D.new( w, Rect( xpos, ypos, 70, 70 ) );

	}

	updateVals{
		//rumble.value = wiimote.rumble;
		wiimote.nunchuk_motion.do{ |it,i| sliders[i].value = it };
		wiimote.nunchuk_buttons.do{ |it,i|
			if ( it == 1 ,
				{
					buttons[i].background = onColor;
				},{
					buttons[i].background = offColor;
				});
		};
		slider2d.setXY( wiimote.nunchuk_stick[0], wiimote.nunchuk_stick[1] );

	}
}

/*WiiClassicGUI{

	}*/
