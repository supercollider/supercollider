+ SCWindow{
	dataptr{
		^dataptr
	}
	asNSObject {
		^dataptr.asNSReturn;
	}

	asNSWindow {
		var view, window;
		view = this.asNSObject;
		window = view.invoke("window");
		view.release;
		^window; // you own it - call release one you do not need it anymore
	}

	visible {
		var ok, nsObj;

		if(isClosed, {^false});

		nsObj = this.asNSWindow;
		ok = nsObj.invoke("isVisible");
		nsObj.release;

		if(ok.isKindOf(Char), {
			^(ok.ascii > 0); // 3.3 - BOOL is returned as Char
		}, {
			^(ok > 0); // post 3.3 - BOOL is returned as Integer
		});

		^false;
	}

	visible_ { |yorn|
		var nsObj;

		if(isClosed, {^this});

		nsObj = this.asNSWindow;
		if(yorn, {
			this.front;
		}, {
			nsObj.invoke("orderOut:", [nil], true);
		});
		nsObj.release;
	}

	unminimize {
		this.front;
	}
}
