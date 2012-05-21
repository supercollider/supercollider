QMetaObject {
  var className;

  *new { arg className;
    ^super.newCopyArgs(className);
  }

  properties {
    _QMetaObject_Properties
    ^this.primitiveFailed
  }

  methods { arg plain = true, signals = false, slots = true;
    _QMetaObject_Methods
    ^this.primitiveFailed
  }
}

QObject {
  classvar
    heap,
    < closeEvent = 19,
    < showEvent = 17,
    < windowActivateEvent = 24,
    < windowDeactivateEvent = 25,
    < mouseDownEvent = 2,
    < mouseUpEvent = 3,
    < mouseDblClickEvent = 4,
    < mouseMoveEvent = 5,
    < mouseEnterEvent = 10,
    < mouseLeaveEvent = 11,
    < mouseWheelEvent = 31,
    < keyDownEvent = 6,
    < keyUpEvent = 7;

  var qObject, finalizer;
  var virtualSlots;

  *qtClass { ^nil }

  *meta { ^QMetaObject(this.qtClass) }

  *new { arg argumentArray;
    var className = this.qtClass;
    if( className.isNil ) {
      Error("Qt:" + this.name + "is an abstract class and can not be instantiated.").throw;
    };
    ^super.new.initQObject( className, argumentArray );
  }

  *heap { ^heap.copy }

  initQObject{ arg className, argumentArray;
    this.prConstruct( className, argumentArray );
    heap = heap.add( this );
  }

  destroy {
    _QObject_Destroy
    ^this.primitiveFailed
  }

  isValid {
    _QObject_IsValid
    ^this.primitiveFailed
  }

  parent { arg class;
    ^this.prGetParent( if( class.notNil ){class.name}{nil} );
  }

  children { arg class;
    ^this.prGetChildren( if( class.notNil ){class.name}{nil} );
  }

  setParent { arg parent;
    _QObject_SetParent
    ^this.primitiveFailed
  }

  properties {
    _QObject_GetProperties
    ^this.primitiveFailed
  }

  methods { arg plain = true, signals = false, slots = true;
    _QObject_GetMethods
    ^this.primitiveFailed
  }

  getProperty { arg property;
    _QObject_GetProperty
    ^this.primitiveFailed
  }

  setProperty { arg property, value, direct=true;
    _QObject_SetProperty
    ^this.primitiveFailed
  }

  setEventHandler{ arg event, method, direct=false, enabled=true;
    _QObject_SetEventHandler
    ^this.primitiveFailed
  }

  setEventHandlerEnabled { arg event, enabled=true;
    _QObject_SetEventHandlerEnabled
    ^this.primitiveFailed
  }

  connectMethod { arg signal, handler, direct=false;
    _QObject_ConnectMethod
    ^this.primitiveFailed
  }

  disconnectMethod { arg signal, method;
    _QObject_DisconnectMethod;
    ^this.primitiveFailed
  }

  connectFunction { arg signal, object, synchronous = false;
    virtualSlots = virtualSlots.add( object );
    this.prConnectObject( signal, object, synchronous );
  }

  disconnectFunction { arg signal, object;
    virtualSlots.remove( object );
    this.prDisconnectObject( signal, object );
  }

  connectSlot { arg signal, receiver, slot;
    _QObject_ConnectSlot;
    ^this.primitiveFailed
  }

  invokeMethod { arg method, arguments, synchronous = true;
    _QObject_InvokeMethod
    ^this.primitiveFailed
  }

  ////////////////////// private //////////////////////////////////

  prConstruct { arg className, argumentArray;
    _QObject_New
    ^this.primitiveFailed;
  }

  prConnectObject { arg signal, object, synchronous = false;
    _QObject_ConnectObject;
    ^this.primitiveFailed
  }

  prDisconnectObject { arg signal, object;
    _QObject_DisconnectObject;
    ^this.primitiveFailed
  }

  prGetChildren { arg className;
    _QObject_GetChildren
    ^this.primitiveFailed
  }

  prGetParent { arg className;
    _QObject_GetParent
    ^this.primitiveFailed
  }

  prRelease { heap.remove(this); }

  doFunction { arg f ... args; f.performList(\value, this, args); }
}
