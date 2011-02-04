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
    < mouseOverEvent = 10,
    < keyDownEvent = 6,
    < keyUpEvent = 7;

  var qObject, finalizer;
  var virtualSlots;

  *new { arg className, argumentArray;
    ^super.new.initQObject( className, argumentArray );
  }

  *heap { ^heap.copy }

  initQObject{ arg className, argumentArray;
    this.prConstruct( className, argumentArray );
    heap = heap.add( this );
    this.connectFunction( 'destroyed()', { heap.remove(this) }, false );
  }

  destroy {
    _QObject_Destroy
    ^this.primitiveFailed
  }

  isValid {
    _QObject_IsValid
    ^this.primitiveFailed
  }

  parent {
    _QObject_GetParent
    ^this.primitiveFailed
  }

  children { arg class;
    ^this.prGetChildren( if( class.notNil ){class.name}{nil} );
  }

  setParent { arg parent;
    _QObject_SetParent
    ^this.primitiveFailed
  }

  getProperty{ arg property, preAllocatedReturn;
    _QObject_GetProperty
    ^this.primitiveFailed
  }

  setProperty { arg property, value, direct=true;
    _QObject_SetProperty
    ^this.primitiveFailed
  }

  registerEventHandler{ arg event, method, direct=false;
    _QObject_SetEventHandler
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

  connectFunction { arg signal, function, synchronous = false;
    virtualSlots = virtualSlots.add( function );
    this.prConnectFunction( signal, function, synchronous );
  }

  disconnectFunction { arg signal, function;
    virtualSlots.remove( function );
    this.prDisconnectFunction( signal, function );
  }

  connectSlot { arg signal, receiver, slot;
    _QObject_ConnectSlot;
    ^this.primitiveFailed
  }

  invokeMethod { arg method, arguments, synchronous = false;
    _QObject_InvokeMethod
    ^this.primitiveFailed
  }

  ////////////////////// private //////////////////////////////////

  prConstruct { arg className, argumentArray;
    _QObject_New
    ^this.primitiveFailed;
  }

  prConnectFunction { arg signal, function, synchronous = false;
    _QObject_ConnectFunction;
    ^this.primitiveFailed
  }

  prDisconnectFunction { arg signal, function;
    _QObject_DisconnectFunction;
    ^this.primitiveFailed
  }

  prGetChildren { arg className;
    _QObject_GetChildren
    ^this.primitiveFailed
  }

  doFunction { arg f ... args; f.valueArray( this, args ); }

}
