
// a simple NSTableView example - blackrain
(
	w = SCWindow.new("NSTableView", Rect(200,500,400,220));
	w.onClose_({ t.release; v.release; h.release; k.release; l.release; c.release; });
	p = w.dataptr.asNSReturn;

	t = SCNSObject("NSTableView", "initWithFrame:", [ Rect(10,10,380,200) ]);
	t.invoke("setUsesAlternatingRowBackgroundColors:", [ true ]);

	v = SCNSObject("NSScrollView", "initWithFrame:", [ Rect(10,10,380,200) ]);

	v.invoke("setHasVerticalScroller:", [ true ]);
	v.invoke("setHasHorizontalScroller:", [ true ]);
	
	k = v.invoke("verticalScroller");
	k.invoke("setControlSize:",[1]);

	l = v.invoke("horizontalScroller");
	l.invoke("setControlSize:",[1]);
	
	v.invoke("setAutoresizingMask:", [18]);
	v.invoke("setDocumentView:", [t]);

	p.invoke("addSubview:", [v], true);

	c = SCNSObject("NSTableColumn", "initWithIdentifier:", [ "column_1" ]);
	c.invoke("setEditable:", [ true ]);
	c.invoke("setWidth:", [ 200 ]);
	c.invoke("setResizingMask:", [ 2 ]);
	
	h = c.invoke("headerCell");
	h.invoke("setStringValue:", ["Items"]);
	h.invoke("setAlignment:", [2]);

	t.invoke("addTableColumn:", [c], true);

	t.invoke("setFocusRingType:", [ 1 ]); //NSFocusRingTypeNone
	w.front;
)


(
i = ["Zero", "One", "Two", "Three", "Four", "Five", "Six"];
t.initAction("doAction:");
t.setDelegate;

t.nsDelegate.addMethod("tableView:objectValueForTableColumn:row:", "@", "@@i", { arg method, args;
	[method, args].postln;
	
	^i[ args[2] ]
});

t.nsDelegate.addMethod("numberOfRowsInTableView:", 'i', "@", { arg method, args;
	[method, args].postln;
	
	^i.size
});
)

t.invoke("setDataSource:", [ t.nsDelegate ], true);

t.invoke("reloadData", defer:true);

SCNSObject.dumpPool;
