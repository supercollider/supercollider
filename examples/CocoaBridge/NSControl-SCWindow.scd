
// NSControl on SCWindows example - blackrain
// Linked NSSlider NSTextField
(
w = SCWindow.new("NSControl on SCWindow", Rect(10,400,400,150)).front;
w.onClose_({ [l, c, d, e].do(_.release) });
p = w.dataptr.asNSReturn;

l = SCNSObject("NSSlider", "initWithFrame:", [ Rect(10,50,380,22) ]);
l.initAction.action_({ arg v, val; d.invoke("setFloatValue:", [val], true) });

p.invoke("addSubview:", [ l ], true);

c = l.invoke("cell");
c.invoke("setControlSize:", [ 1 ], true); // small


d = SCNSObject("NSTextField", "initWithFrame:", [ Rect(10,10,100,20) ]);

p.invoke("addSubview:", [ d ], true);

d.initAction.action_({ arg v, val; l.invoke("setFloatValue:", [val], true) });
)

f = SCSlider.new(w, Rect(10,80,380,16));


c.invoke("setControlSize:", [ 0 ], true); // regular
c.invoke("setControlSize:", [ 1 ], true); // small
c.invoke("setControlSize:", [ 2 ], true); // mini

SCNSObject.dumpPool


// NSButton
(
w = SCWindow.new("NSButton", Rect(10,530,280,100)).front;
w.onClose_({ b.release });
p = w.dataptr.asNSReturn;

b = SCNSObject("NSButton", "initWithFrame:", [ Rect(10,5,88,24) ]);
b.invoke("setTitle:", ["OK"], true);
b.invoke("setBezelStyle:", [10], true); // NSSmallSquareBezelStyle
b.initAction("doAction:");
b.nsAction.action_({ arg v, val; [v, val].postln });

p.invoke("addSubview:", [ b ], true);
)

b.dump

SCNSObject.dumpPool

// NSTextView
(
w = SCWindow.new("NSTextView", Rect(10,400,500,280)).front;
w.view.decorator = FlowLayout(w.view.bounds);
w.onClose_({ [v, t].do(_.release) });
p = w.dataptr.asNSReturn;

t = SCNSObject("NSTextView", "initWithFrame:", [ Rect(10,35,480,220) ]);
v = SCNSObject("NSScrollView", "initWithFrame:", [ Rect(10,35,480,220) ]);
v.invoke("setDocumentView:", [t]);
v.invoke("setHasVerticalScroller:", [ true ]);
p.invoke("addSubview:", [ v ]);
)

SCNSObject.dumpPool

