QMetaObject {
	var className;

	*new { |className|
		^super.newCopyArgs(className)
	}

	properties {
		_QMetaObject_Properties
		^this.primitiveFailed
	}

	methods { |plain = true, signals = false, slots = true|
		_QMetaObject_Methods
		^this.primitiveFailed
	}
}

QObject {
	classvar
		heap,
		<closeEvent = 19,
		<showEvent = 17,
		<windowActivateEvent = 24,
		<windowDeactivateEvent = 25,
		<mouseDownEvent = 2,
		<mouseUpEvent = 3,
		<mouseDblClickEvent = 4,
		<mouseMoveEvent = 5,
		<mouseEnterEvent = 10,
		<mouseLeaveEvent = 11,
		<mouseWheelEvent = 31,
		<keyDownEvent = 6,
		<keyUpEvent = 7;

	var qObject, finalizer;
	var virtualSlots;

	*qtClass { ^nil }

	*meta { ^QMetaObject(this.qtClass) }

	*new { |argumentArray|
		var className = this.qtClass;
		if(className.isNil) {
			Error("Qt:" + this.name + "is an abstract class and can not be instantiated.").throw
		};
		^super.new.initQObject(className, argumentArray)
	}

	*newCopyArgs { |...args, kwargs|
		Error("Please replace call to 'newCopyArgs' with a call to 'new'. All QObjects must call the 'new' method.").throw
	}

	*heap { ^heap.copy }

	initQObject{ |className, argumentArray|
		this.prConstruct(className, argumentArray);
		heap = heap.add(this)
	}

	destroy {
		_QObject_Destroy
		^this.primitiveFailed
	}

	isValid {
		_QObject_IsValid
		^this.primitiveFailed
	}

	parent { |class|
		^this.prGetParent(if(class.notNil) { class.name } { nil })
	}

	children { |class|
		^this.prGetChildren(if(class.notNil) { class.name } { nil })
	}

	setParent { |parent|
		_QObject_SetParent
		^this.primitiveFailed
	}

	properties {
		_QObject_GetProperties
		^this.primitiveFailed
	}

	methods { |plain = true, signals = false, slots = true|
		_QObject_GetMethods
		^this.primitiveFailed
	}

	getProperty { |property|
		_QObject_GetProperty
		^this.primitiveFailed
	}

	setProperty { |property, value, direct = true|
		_QObject_SetProperty
		^this.primitiveFailed
	}

	setEventHandler{ |event, method, direct = false, enabled = true|
		_QObject_SetEventHandler
		^this.primitiveFailed
	}

	setEventHandlerEnabled { |event, enabled = true|
		_QObject_SetEventHandlerEnabled
		^this.primitiveFailed
	}

	connectMethod { |signal, handler, direct = false|
		_QObject_ConnectMethod
		^this.primitiveFailed
	}

	disconnectMethod { |signal, method|
		_QObject_DisconnectMethod;
		^this.primitiveFailed
	}

	connectFunction { |signal, object, synchronous = false|
		virtualSlots = virtualSlots.add(object);
		this.prConnectObject(signal, object, synchronous)
	}

	disconnectFunction { |signal, object|
		virtualSlots.remove(object);
		this.prDisconnectObject(signal, object)
	}

	connectSlot { |signal, receiver, slot|
		_QObject_ConnectSlot;
		^this.primitiveFailed
	}

	invokeMethod { |method, arguments, synchronous = true|
		_QObject_InvokeMethod
		^this.primitiveFailed
	}

	////////////////////// private //////////////////////////////////

	prConstruct { |className, argumentArray|
		_QObject_New
		^this.primitiveFailed
	}

	prConnectObject { |signal, object, synchronous = false|
		_QObject_ConnectObject;
		^this.primitiveFailed
	}

	prDisconnectObject { |signal, object|
		_QObject_DisconnectObject;
		^this.primitiveFailed
	}

	prGetChildren { |className|
		_QObject_GetChildren
		^this.primitiveFailed
	}

	prGetParent { |className|
		_QObject_GetParent
		^this.primitiveFailed
	}

	prRelease { heap.remove(this) }

	doFunction { |f ... args| f.performList(\value, this, args) }
}
