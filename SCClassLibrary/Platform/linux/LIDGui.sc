LIDGui{
	var <win, <updater;
	var <device;
	var buttons;
	var sliders;
	var relsliders;

	var <bkeys,<akeys,<rkeys;

	*new{ |device|
		^super.new.init(device);
	}

	init{ |dev|
		device = dev;
		win = Window.new( device.info.name, Rect( 0, 0, 400, 400 ));
		win.view.decorator = FlowLayout( win.bounds );

		EZText.new( win, 390@20 , "name", initVal: device.info.name );

		EZText.new( win, 390@20, "findArgs", initVal: device.info.findArgs );

		win.view.decorator.nextLine;

		// buttons:
		if ( device.slots[1].notNil, {
			bkeys = device.slots[1].keys.asArray.sort;
			buttons = bkeys.collect{ |key,it|
				EZNumber.new( win, 30@40, device.slots[1][key].key.asString, [0,1,\linear,1].asSpec, labelWidth: 30, layout: 'line2', gap:0@0, margin: 0@0 );
			};
		});
		win.view.decorator.nextLine;

		// absolute slots:
		if ( device.slots[3].notNil, {
			akeys = device.slots[3].keys.asArray.sort;
			sliders = akeys.collect{ |key,it|
				EZSlider.new( win, 30@80, device.slots[3][key].key.asString, labelWidth: 30, layout: 'vert',gap:0@0,margin:0@0 );
			};
		});
		win.view.decorator.nextLine;

		// relative slots:

		if ( device.slots[2].notNil, {
			rkeys = device.slots[2].keys.asArray.sort;
			relsliders = rkeys.collect{ |key,it|
				EZSlider.new( win, 30@80, device.slots[2][key].key.asString, [-10,10].asSpec, labelWidth: 30, layout: 'vert',gap:0@0,margin:0@0 );
			};
		});

		win.front;

		updater = SkipJack.new( {
			if ( bkeys.notNil, {
				bkeys.do{ |key,k,v| buttons[k].value = device.slots[1][key].value };
			});
			if ( rkeys.notNil, {
				rkeys.do{ |key,k,v| relsliders[k].value = device.slots[2][key].delta };
			});
			if ( akeys.notNil, {
				akeys.do{ |key,k,v| sliders[k].value = device.slots[3][key].value };
			});
		}, 0.1, win.isClosed, name: "LIDGui" );
	}
}

+ LID {

	makeGui{
		^LIDGui.new( this );
	}

}
