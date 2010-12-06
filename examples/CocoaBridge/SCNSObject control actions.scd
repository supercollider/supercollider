
// blackrain
// NSButton
w = SCNSObject("NSWindow", "initWithContentRect:styleMask:backing:defer:", [ Rect(100,600,500,200), 10, 2, 1]);

w.invoke("makeKeyAndOrderFront:", [nil], true);
w.invoke("setTitle:", [ "Hello, World!" ], true);


b = SCNSObject("NSButton", "initWithFrame:", [ Rect(10,10,75,22) ]);
b.invoke("setBezelStyle:", [10], true);

v = w.invoke("contentView");
v.invoke("addSubview:", [ b ], true);


w.setDelegate;
(
	// - (void)buttonAction:(id)sender;
	w.nsDelegate.addMethod("buttonAction:", "v", "@", { arg method, args;
		var btn = args.at(0).asNSReturn; // args.at(0) is the button
		
		btn.invoke("state").postln;
		
	});
)

b.invoke("setTarget:", [ w.nsDelegate ], false);
b.invoke("setAction:", [ "buttonAction:" ], false); // SEL are recognized now

//

w.release;

SCNSObject.dumpPool;

SCNSObject.freePool;

// NSSlider
w = SCNSObject("NSWindow", "initWithContentRect:styleMask:backing:defer:", [ Rect(100,650,500,40), 10, 2, 1]);

w.invoke("makeKeyAndOrderFront:", [nil], true);
w.invoke("setTitle:", [ "Hello, World!" ], true);


l = SCNSObject("NSSlider", "initWithFrame:", [ Rect(10,10,480,22) ]);

v = w.invoke("contentView");
v.invoke("addSubview:", [ l ], true);


w.setDelegate;
(
	w.nsDelegate.addMethod("sliderAction:", "v", "@", { arg method, args;
		var slider = args.at(0).asNSReturn;
		
		slider.invoke("doubleValue").postln;
		
	});
)

l.invoke("setTarget:", [ w.nsDelegate ], false);
l.invoke("setAction:", [ "sliderAction:" ], false);



SCNSObject.dumpPool;

SCNSObject.freePool;
