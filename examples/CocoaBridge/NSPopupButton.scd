
// NSPopupButton tests - blackrain
(
w = SCWindow.new("NSPopupButton", Rect(10,400,400,150));
w.onClose_({ [l, f, c].do(_.release) });
p = w.dataptr.asNSReturn;
l = SCNSObject("NSPopUpButton", "initWithFrame:", [ Rect(10,25,200,22) ]);
c = l.invoke("cell");
p.invoke("addSubview:", [ l ], true);
w.front;
)

c.invoke("setControlSize:", [1], true);
c.invoke("setControlTint:", [6], true); // graphite tint

(
f = SCNSObject("NSFont", "fontWithName:size:", ["Lucida Grande", 11]);
f.className.postln;
c.invoke("setFont:", [f], true);
)

// add items
(
	l.invoke("addItemWithTitle:", ["Zero"], true);
	l.invoke("addItemWithTitle:", ["One"], true);
	l.invoke("addItemWithTitle:", ["Two"], true);
	l.invoke("addItemWithTitle:", ["Three"], true);
	l.invoke("addItemWithTitle:", ["Four"], true);
	l.invoke("addItemWithTitle:", ["Five"], true);
)

// set an action
(
l.initAction("doAction:");
l.nsAction.action_({ arg v, val;
	[v, val, l.invoke("indexOfSelectedItem", [], false), l.invoke("titleOfSelectedItem", [], false)].postln;
});
)

i = l.invoke("selectedItem", [], false)
i.invoke("title", [], false);
i.release;

l.invoke("titleOfSelectedItem", [], false);

l.invoke("indexOfSelectedItem", [], false);

l.invoke("selectItemWithTitle:", ["Five"], false);

l.invoke("selectItemWithTitle:", ["whatever"], false);
l.invoke("indexOfSelectedItem", [], false);

l.invoke("selectItemWithTitle:", ["One"], false);

// as a pulldown list
l.invoke("setTitle:", ["Title"], true);
l.invoke("setPullsDown:", [true], true);
l.invoke("setPullsDown:", [false], true);

SCNSObject.dumpPool;
