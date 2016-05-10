DragView : TextField {
	*new { arg parent, bounds; ^super.new(parent,bounds).initQDragView }

	initQDragView {
		var plt = this.palette;
		plt.base = plt.window;
		this.palette = plt;
		this.setProperty(\readOnly, true);
	}

	// override View.mouseDownEvent to initiate drag without keyboard modifier
	mouseDownEvent { arg x, y, modifiers, buttonNumber, clickCount;
		// Try to get drag obj and start a drag.
		// If successful, block further processing of this event.
		if( this.beginDrag( x, y ) ) { ^true };

		// else continue to handle mouse down event
		modifiers = QKeyModifiers.toCocoa(modifiers);
		^this.mouseDown( x, y, modifiers, buttonNumber, clickCount );
	}

	defaultGetDrag { ^nil }
	defaultCanReceiveDrag { ^false }
	defaultReceiveDrag { }
}

DragSource : DragView {
	defaultGetDrag { ^object }
}

DragSink : DragView {
	defaultCanReceiveDrag { ^true }
	defaultReceiveDrag { this.object = View.currentDrag; action.value(this); }
}

DragBoth : DragSink {
	defaultGetDrag { ^object }
}
