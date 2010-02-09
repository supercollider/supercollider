
// return booleans 
(
o = SCNSObject("NSWindow", "initWithContentRect:styleMask:backing:defer:", [Rect(100,540,400,200), 15, 2, 1]);
o.invoke("makeKeyAndOrderFront:", [nil], true);
)

c = 0; // window should close = false

o.setDelegate;
o.nsDelegate.addMethod("windowShouldClose:", "i", "@", { arg method, args; [method, args].postln;  ^c });

// try to close the window  then set c to 1
c = 1; // window should close = true


o.release;

SCNSObject.dumpPool;
