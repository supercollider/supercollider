GeneralHIDDeviceGUI{
	var <win, <updater;
	var <device;
	var butlabels, buttons;
	var slidlabels, sliders;
	var rellabels, relsliders;
	
	*new{ |device|
		^super.new.init(device);
	}
	
	init{ |dev|
		device = dev;
		win = GUI.window.new( "GeneralHIDDevice", Rect( 0, 0, 400, 400 ));
		win.view.decorator = FlowLayout( win.bounds );
		
		// buttons:
		butlabels = device.slots[1].collect{ |it|
			GUI.staticText.new( win, Rect( 0, 0, 30, 20 ) ).string_( it.id.asString ).align_( 0 );
		};
		win.view.decorator.nextLine;
		buttons = device.slots[1].collect{ |it|
			GUI.button.new( win, Rect( 0, 0, 30, 20 ) ).states_( [ ["0"], ["1"] ] );
		};
		win.view.decorator.nextLine;

		// absolute slots:
		slidlabels = device.slots[3].collect{ |it|
			GUI.staticText.new( win, Rect( 0, 0, 30, 20 ) ).string_( it.id.asString ).align_( 0 );
		};
		win.view.decorator.nextLine;
		sliders = device.slots[3].collect{ |it|
			GUI.slider.new( win, Rect( 0, 0, 30, 60 ) )
		};
		win.view.decorator.nextLine;

		// relative slots:
		rellabels = device.slots[2].collect{ |it|
			GUI.staticText.new( win, Rect( 0, 0, 30, 20 ) ).string_( it.id.asString ).align_( 0 );
		};
		win.view.decorator.nextLine;
		relsliders = device.slots[2].collect{ |it|
			GUI.slider.new( win, Rect( 0, 0, 30, 60 ) )
		};

		win.front;

		updater = SkipJack.new( {
			device.slots[1].keysValuesDo{ |k,v| buttons[k].value = v.value };
			device.slots[2].keysValuesDo{ |k,v| relsliders[k].value = v.value };
			device.slots[3].keysValuesDo{ |k,v| sliders[k].value = v.value };
		}, 0.2, win.isClosed, name: "GeneralHIDGUI" );
	}
}
