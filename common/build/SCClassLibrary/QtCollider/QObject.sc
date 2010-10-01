QObject {
  classvar
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

  *new { arg className, argumentArray;
    ^super.new.prInitQObject( className, argumentArray );
  }

  getProperty{ arg property, preAllocatedReturn;
    _QObject_GetProperty
    ^this.primitiveFailed
  }

  setProperty { arg property, value, direct=true;
    _QObject_SetProperty
    ^this.primitiveFailed
  }

  prInitQObject{ arg className, argumentArray;
    _QObject_New
    ^this.primitiveFailed;
  }

  prDestroy {
    _QObject_Destroy
    ^this.primitiveFailed
  }

  registerEventHandler{ arg event, method, direct=false;
    _QObject_SetEventHandler
    ^this.primitiveFailed
  }

  invokeMethod { arg method, arguments;
    _QObject_InvokeMethod
    ^this.primitiveFailed
  }
}
