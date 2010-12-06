/*------
	Simple Tests
	Charles - TheLych at gmail dot com =)
*/

(
a = SCNSObject("NSURL", "initWithString:", ["http://www.audiosynth.com"]);
SCNSObject.dumpPool; // dump the current NS objects retained in SC
)

(
SCNSObject.freePool; // free all those objects -- carefull if their already in use
SCNSObject.dumpPool;
)

(
a = SCNSObject("NSWindow", "initWithContentRect:styleMask:backing:defer:", [Rect(250, 250, 800, 600), 15, 2, 1]);
a.registerNotification("NSWindowWillCloseNotification", {|notificationName, nsNotification|
	[notificationName, nsNotification].postln;
	a.release;
});
a.invoke("makeKeyAndOrderFront:", [a], true);
SCNSObject.dumpPool;
)


/*----------------------
Notification Examples
using Webview
________________________*/

(
var win, root, cocoaUI, cell, webview, levelIndicator;
win = SCNSObject("NSWindow", "initWithContentRect:styleMask:backing:defer:", [Rect(250, 250, 800, 600), 15, 2, 1]);

root = SCNSObject("NSView", "initWithFrame:", [Rect(0, 0, 800, 600)]);
root.invoke("setAutoresizingMask:", [1 + 2 + 8 + 16]);

webview = SCNSObject("WebView", "initWithFrame:frameName:groupName:", [Rect(10, 30, 800-20, 600-40), "mywebview", "mywebviewgroup"]);
webview.invoke("setAutoresizingMask:", [1 + 2 + 8 + 16]);

~webview = webview; // just to retrieve the source after

cell = SCNSObject("NSLevelIndicatorCell", "initWithLevelIndicatorStyle:", [1]);
levelIndicator = SCNSObject("NSLevelIndicator", "initWithFrame:", [Rect(10, 5, 800-20, 10)]);
levelIndicator.invoke("setCell:", [cell]);
levelIndicator.invoke("setMinValue:", [0]);
levelIndicator.invoke("setMaxValue:", [100]);
levelIndicator.invoke("setFloatValue:", [0]);
levelIndicator.invoke("setContinuous:", [true]);
cell.release;

cocoaUI.add(root);
cocoaUI.add(webview);
cocoaUI.add(levelIndicator);

win.invoke("setContentView:", [root]);
root.invoke("addSubview:", [webview]);
root.invoke("addSubview:", [levelIndicator]);

///// Notifications
// Window 
win.registerNotification("NSWindowWillCloseNotification", {
	|notificationName, nsNotificationObjectAsRawPointer|
	"closing window".postln;
	cocoaUI.do {|ui| ui.invoke("removeFromSuperviewWithoutNeedingDisplay")};
	win.release;
	root.release;
	webview.release;
	levelIndicator.release;
	~webview = nil;
});

win.registerNotification("NSWindowDidMoveNotification", {
	|notificationName, nsNotificationObjectAsRawPointer|
	notificationName.postln;
});

win.registerNotification("NSWindowDidMiniaturizeNotification", {
	|notificationName, nsNotificationObjectAsRawPointer|
	notificationName.postln;
});
// Webview Notifications
webview.registerNotification("WebProgressEstimateChangedNotification", {
	|notificationName, nsNotificationObjectAsRawPointer|
	var value;
	value = webview.invoke("estimatedProgress");
	levelIndicator.invoke("setFloatValue:", [value*100]);
	("loading progress: "+ (value*100) + "%").postln;
});

webview.registerNotification("WebProgressFinishedNotification", {
	|notificationName, nsNotificationObjectAsRawPointer|
	var t0, t1;
	levelIndicator.invoke("setFloatValue:", [0]);
	t0 = webview.invoke("mainFrame");
	t1 = t0.invoke("dataSource"); t0.release;
	t0 = t1.invoke("initialRequest"); t1.release;
	t1 = t0.invoke("URL"); t0.release;
	t0 = t1.invoke("absoluteString"); t1.release;
	(t0 ++ " finished Loading").postln;
	win.invoke("setTitle:", [t0]);
});
///// Show Window
win.invoke("makeKeyAndOrderFront:", [win], true);

///// URL Loading
{
	var url;
	url = "http://swiki.hfbk-hamburg.de:8888/MusicTechnology/6";
	webview.invoke("setMainFrameURL:", [url]);
	SCNSObject.dumpPool;
}.defer(0.2);
)



/*----------------------
NSData conversion
using Webview html source
________________________*/
(
/// interpret it AFTER previous example for getting source html file
var mainframe, datasource, nsdata;
mainframe = ~webview.invoke("mainFrame");
datasource = mainframe.invoke("dataSource"); mainframe.release;
nsdata = datasource.invoke("data"); datasource.release;
nsdata.isSubclassOf("NSData").postln; // 
"---- HTML Source ----".postln;
nsdata.asArray(\string).postln;
"---- End of HTML Source ----".postln;
nsdata.release;
)

// dump to test after closing
SCNSObject.dumpPool;




/*----------------------
special Delegates actions with return values
using NSURLConnection
________________________*/
(
var url;

// first URL Request
url = SCNSObject("NSURL", "initWithString:", ["http://www.audiosynth.com"]);
~urlRequest = SCNSObject("NSURLRequest", "requestWithURL:cachePolicy:timeoutInterval:", [url, 0, 60]); url.release;

// redirection to set after delegate call
url = SCNSObject("NSURL", "initWithString:", ["http://www.apple.com"]);
~redirection = SCNSObject("NSURLRequest", "requestWithURL:cachePolicy:timeoutInterval:", [url, 0, 60]); url.release;

// we need here to set a void object to set its delegate before it is allocated really
// because urlConnection does not have a setDelegate: method
~urlConnection = SCNSObject.newClear;
~urlConnection.setDelegate; // create and attach a special delegate
~urlConnection.nsDelegate.addMethod("connectionDidFinishLoading:", nil, "@", {
|method, args| [method, args].postln;
});

//// Custom Delegate Method with return values allowed (automatic conversion for most)
//// Here we have to provide the (name, return type of the delegate method, and the type encoding for the arguments)
//// see http://developer.apple.com/documentation/Cocoa/Conceptual/ObjectiveC/Articles/chapter_13_section_9.html#//apple_ref/doc/uid/TP30001163-CH9-TPXREF165 for explanations
~urlConnection.nsDelegate.addMethod("connection:didReceiveResponse:", nil, "@@", {
	|method, args| [method, args].postln;
});

~urlConnection.nsDelegate.addMethod("connection:willSendRequest:redirectResponse:", "@", "@@@", {
	|method, arguments|
	[method, arguments].postln;
	url = ~redirection.invoke("URL");
	"redirecting to "++url.invoke("absoluteString"); url.release;
	^~redirection; // redirect !
});

// we can init the object now
~urlConnection.init("NSURLConnection", "initWithRequest:delegate:", [~urlRequest, ~urlConnection.nsDelegate]); // now we can alloc the object and attach its delegate
)

(
~urlConnection.release;
~urlRequest.release;
~redirection.release;
)



/*
* NSToolbar
*/

(
var win, toolbar, delegate, toolbarItems, items, toolbarItem=nil, itemAction /*, myCustomToolbarItemIcon = Document.current.path.dirname ++ "/../icons/TrBtn_pause_on.tiff"*/;

win = SCNSObject("NSWindow", "initWithContentRect:styleMask:backing:defer:", [Rect(250, 250, 800, 600), 1 + 2 + 4 + 8, 2, 1]);

"-> Creating Toolbar".postln;
toolbar = SCNSObject.newClear;
itemAction = {|a, b|
	[a, b].postln;
};

toolbarItems = SCNSObject("NSMutableArray", "initWithCapacity:", [16]);
toolbarItems.invoke("addObject:", ["NSToolbarSeparatorItem"]);
toolbarItems.invoke("addObject:", ["NSToolbarShowFontsItem"]);
toolbarItems.invoke("addObject:", ["NSToolbarShowColorsItem"]);
toolbarItems.invoke("addObject:", ["NSToolbarCustomizeToolbarItem"]);
/*toolbarItems.invoke("addObject:", ["MyCustomToolbarItem"]);*/ 

delegate = toolbar.setDelegate;
toolbar.nsDelegate.addMethod("toolbarAllowedItemIdentifiers:", "@", "@", {
	|method, arguments|
	"toolbarAllowedItemIdentifiers called".postln;
	^toolbarItems;
});

toolbar.nsDelegate.addMethod("toolbarSelectableItemIdentifiers:", "@", "@", {
	|method, arguments|
	"toolbarSelectableItemIdentifiers called".postln;
	^toolbarItems;
});

toolbar.nsDelegate.addMethod("toolbarDefaultItemIdentifiers:", "@", "@", {
	|method, arguments|
	"toolbarDefaultItemIdentifiers called".postln;
	^toolbarItems;
});

toolbar.nsDelegate.addMethod("toolbar:itemForItemIdentifier:willBeInsertedIntoToolbar:", "@", "@@i", {
	|method, arguments|
	var image;
	arguments[1].postln;
	/*
	if(toolbarItem.isNil, {
		toolbarItem = SCNSObject("NSToolbarItem", "initWithItemIdentifier:", [arguments[1]]);
		image = SCNSObject("NSImage", "initWithContentsOfFile:", [myCustomToolbarItemIcon]);
		toolbarItem.invoke("setImage:", [image]);
		toolbarItem.invoke("setLabel:", ["Custom"]);
		toolbarItem.initAction("doAction:");
		toolbarItem.nsAction.action_({"----> action".postln});
		"toolbar:itemForItemIdentifier:willBeInsertedIntoToolbar:".postln;
		image.release;
	})
	^toolbarItem;
	*/
	^nil;
});

toolbar.init("NSToolbar", "initWithIdentifier:", ["myToolbar"]);
toolbar.invoke("setAllowsUserCustomization:", [true]);
toolbar.invoke("setDisplayMode:", [0]); //default
toolbar.invoke("setSizeMode:", [0]); //default

win.invoke("setToolbar:", [toolbar], true);
toolbar.invoke("setDelegate:", [toolbar.nsDelegate]);

win.registerNotification("NSWindowWillCloseNotification", {
	"closing window".postln;
	win.release;
	toolbarItems.release;
	if(toolbarItem.notNil, {toolbarItem.release});
	toolbar.release;
});

win.invoke("makeKeyAndOrderFront:", [win], true);
)


/*
	TabView - very simple example
*/
(
var win, nswin, root, tabview, tabviewitems, numTabs = 5, temp, temp1;

win = SCWindow("Hello", Rect(100, 200, 700, 400), textured:true);
win.view.decorator = FlowLayout.new(win.view.bounds);

10.do{|i| EZSlider.new(win, 600 @ 20, "Hi"+i); win.view.decorator.nextLine};

root = SCNSObject.newFromRawPointer(win.dataptr); // SCGraphView

tabview = SCNSObject("NSTabView", "initWithFrame:", [Rect(0, 0, 700, 400)]);
tabview.invoke("setAutoresizingMask:", [1 + 2 + 8 + 16]);
tabviewitems = Array.newClear(numTabs);
numTabs.do {|i|
	var obj = SCNSObject("NSTabViewItem", "initWithIdentifier:", ["Tab"++i.asString]);
	obj.invoke("setLabel:", ["Tab"+(i+1)]);
	tabview.invoke("addTabViewItem:", [obj]);
	tabviewitems[i] = obj;
};

root.invoke("retain");
nswin = root.invoke("window");

{
"swapping views".postln;
nswin.invoke("setContentView:", [tabview]);
nswin.invoke("displayIfNeeded");
tabviewitems[0].invoke("setView:", [root]);
//tabviewitems[0].invoke("setNeedsDisplay:", [true]);
}.defer(0.01);

win.onClose_({
	"closing window".postln;
	tabview.release;
	nswin.release;
	root.release;
	tabviewitems.do {|obj| obj.release;}
});

tabviewitems[1].invoke("view").className.postln;

win.front;
)

/*
	Class methods examples
	SCUserView drawing with NSBezierPath very very simple just for test
*/
(
var win, uview, obj, bp;
win = SCWindow("Hello", Rect(400, 200, 400, 300));
uview = SCUserView(win, win.view.bounds);

uview.drawFunc_({|v|
	Color.red.setFill;
	bp = SCNSObject("NSBezierPath", "fillRect:", [v.bounds]);
	bp.free;
});

win.front;
)

SCNSObject.dumpPool;
SCNSObject.freePool;