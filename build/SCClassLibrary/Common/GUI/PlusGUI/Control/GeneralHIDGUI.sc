GeneralHIDDeviceGUI{
	var <win, <updater;
	var <device;
	var butlabels, buttons;
	var slidlabels, sliders;
	var rellabels, relsliders;

	var <bkeys,<akeys,<rkeys;

	*new{ |device|
		^super.new.init(device);
	}

	init{ |dev|
		device = dev;
		win = GUI.window.new( device.info.name, Rect( 0, 0, 400, 400 ));
		win.view.decorator = FlowLayout( win.bounds );

		GUI.staticText.new( win, Rect( 0, 0, 390, 20 ) ).string_( device.info.name );

		GUI.staticText.new( win, Rect( 0, 0, 390, 20 ) ).string_( device.info.findArgs );

		win.view.decorator.nextLine;

		// buttons:
		if ( device.slots[1].notNil, {
			bkeys = device.slots[1].keys.asArray.sort;
			butlabels = bkeys.collect{ |key,it|
				GUI.staticText.new( win, Rect( 0, 0, 30, 20 ) ).string_( device.slots[1][key].id.asString ).align_( 0 );
			};
			win.view.decorator.nextLine;
			buttons = bkeys.collect{ |it|
				GUI.button.new( win, Rect( 0, 0, 30, 20 ) ).states_( [ ["0"], ["1"] ] );
			};
		});
		win.view.decorator.nextLine;

		// absolute slots:
		if ( device.slots[3].notNil, {
			akeys = device.slots[3].keys.asArray.sort;
			slidlabels = akeys.collect{ |key,it|
				GUI.staticText.new( win, Rect( 0, 0, 30, 20 ) ).string_( device.slots[3][key].id.asString ).align_( 0 );
			};
			win.view.decorator.nextLine;
			sliders = akeys.collect{ |it|
				GUI.slider.new( win, Rect( 0, 0, 30, 60 ) )
			};
		});
		win.view.decorator.nextLine;

		// relative slots:

		if ( device.slots[2].notNil, {
			rkeys = device.slots[2].keys.asArray.sort;
			rellabels = rkeys.collect{ |key,it|
				GUI.staticText.new( win, Rect( 0, 0, 30, 20 ) ).string_( device.slots[2][key].id.asString ).align_( 0 );
			};
			win.view.decorator.nextLine;
			relsliders = rkeys.collect{ |it|
				GUI.slider.new( win, Rect( 0, 0, 30, 60 ) )
			};
		});

		win.front;

		updater = SkipJack.new( {
			if ( bkeys.notNil, {
				bkeys.do{ |key,k,v| buttons[k].value = device.slots[1][key].value };
			});
			if ( rkeys.notNil, {
				rkeys.do{ |key,k,v| relsliders[k].value = device.slots[2][key].value };
			});
			if ( akeys.notNil, {
				akeys.do{ |key,k,v| sliders[k].value = device.slots[3][key].value };
			});
		}, 0.2, win.isClosed, name: "GeneralHIDGUI" );
	}
}
