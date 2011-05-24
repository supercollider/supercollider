QDragView : QTextField
{
  var <>setBoth = true;
  var <object;

  *new { arg parent, bounds; ^super.new(parent,bounds).initQDragView }

  initQDragView {
    var plt = this.palette;
    plt.baseColor = plt.windowColor;
    this.palette = plt;
    this.setProperty(\readOnly, true);
  }

  object_  { arg obj;
    if( setBoth ) { this.string = obj.asString };
    object = obj;
  }

  // override QView.mouseDownEvent to initiate drag without keyboard modifier
  mouseDownEvent { arg x, y, modifiers, buttonNumber, clickCount;
    // Try to get drag obj and start a drag.
    // If successful, block further processing of this event.
    if( this.beginDrag( x, y ) ) { ^false };

    // else continue to handle mouse down event
    modifiers = QKeyModifiers.toCocoa(modifiers);
    ^this.mouseDown( x, y, modifiers, buttonNumber, clickCount );
  }

  defaultGetDrag { ^nil }
  defaultCanReceiveDrag { ^false }
  defaultReceiveDrag { }
}

QDragSource : QDragView
{
  defaultGetDrag { ^object }
}

QDragSink : QDragView
{
  defaultCanReceiveDrag { ^true }
  defaultReceiveDrag { this.object = QView.currentDrag; action.value(this); }
}

QDragBoth : QDragSink
{
  defaultGetDrag { ^object }
}
