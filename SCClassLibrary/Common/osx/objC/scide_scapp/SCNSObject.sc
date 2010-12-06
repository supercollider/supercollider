//SCNSClass {
//	var <name;
//
//	*new{|name|
//		^super.newCopyArgs
//	}
//
//	asString{
//		^name
//	}
//
//	//todo implement objc functionality
//
//
//}

SCNSObjectAbstract {
	var <dataptr=nil, <> className, < nsAction=nil, < nsDelegate=nil;

	*dumpPool {
		_ObjC_DumpPool
		^this.primitiveFailed;
	}

	*freePool {
		_ObjC_FreePool
		^this.primitiveFailed;
	}

	*new{|classname, initname, args, defer=false|
		^super.new.init(classname.asString, initname, args, defer)
	}

	*getClass{|classname| //return only the class id, the object is not allocated yet ...
		^super.new.initClass(classname);
	}

	invoke{|method, args, defer=false|
		var result;
		if(dataptr.isNil, {^nil;});
		args = args ? []; //todo: fix that in the primitive ...
		result = this.prInvoke(method, args, defer);
		^result.asNSReturn
	}

	initClass{|name|
		this.prGetClass(name)
	}

	isSubclassOf {
		|nsclassname|
		_ObjC_IsSubclassOfNSClass
		^this.primitiveFailed;
	}

	release {
		if(nsAction.notNil, {nsAction.release; nsAction=nil;});
		if(nsDelegate.notNil, {nsDelegate.release; nsDelegate=nil;});
		this.prDealloc;
		dataptr = nil;
	}

	isAllocated {
		^dataptr.notNil
	}

	isReleased {
		^dataptr.isNil
	}

	initAction{|actionName = "doFloatAction:"|
		var out;
		out = CocoaAction.newClear;
		this.prSetActionForControl(out, actionName);
		out.prSetClassName;
		nsAction = out;
		nsAction.object = this;
		^out
	}
	setDelegate{
		var out;
		out = CocoaAction.newClear;
		this.prSetDelegate(out);
		out.prSetClassName;
		nsDelegate = out;
		nsDelegate.object = this;
		^out
	}

	sendMessage{|msgname, args|
		this.prSendMsg(msgname, args)
	}

	prSendMsg{|msgname, args|
			_ObjC_SendMessage
	}
//private
	*newFromRawPointer{|ptr|
		^super.new.initFromRawPointer(ptr)
	}

	*newClear{
		^super.new
	}

	*newWith{| classname, initname,args|
		^super.new.initWith( classname, initname,args)
	}

	initWith{| cn, initname,args|
		className = cn;
		this.prAllocWith( cn, initname,args);
	}

	initFromRawPointer{|ptr|
		dataptr = ptr;
		className = this.prGetClassName;
	}

	prSetClassName{
		className = this.prGetClassName;
	}

	init{|cn, in, args, defer|
		var result;
	//	className = cn; // set upon return
		if(cn.isKindOf(String) and:{in.isKindOf(String)}, {
			result = this.prAllocInit(cn, in, args, defer);
			^result;
		});
	}

	prAllocInit { arg classname, initname,args;
		_ObjC_AllocInit;
		^this.primitiveFailed;
	}

	prDealloc {
		_ObjC_Dealloc;
		^this.primitiveFailed;
	}

	prInvoke { arg initname,args, defer=true;
		_ObjC_Invoke
		^this.primitiveFailed;
	}

	prGetClassName{|it|
		_ObjC_GetClassName
//		^this.primitiveFailed;
	}

	prAllocWith { arg classname, initname,args;
		_ObjC_AllocSend;
		^this.primitiveFailed;
	}
	prGetClass{arg classname;
		_ObjC_GetClass
		^this.primitiveFailed;
	}

	/*
	asPyrString {
		^this.prAsPyrString;
	}
	*/

	//for NSControl:
	prSetActionForControl{|control|
		_ObjC_SetActionForControl
	}
	prSetDelegate{|control|
		_ObjC_SetDelegate
	}

//	sendMsg{|initname,args|
//		^this.prSendMsg(className, initname, args, 0)
//	}

// experimental
	*panel{|path|
		_LoadUserPanel
	}

	asArray {arg arrayType;
		var requestedLength;
		requestedLength = this.invoke("length").asInteger;
		if(this.isSubclassOf("NSData"), {
			if(arrayType.isKindOf(String), {arrayType = arrayType.asSymbol});
			if(arrayType.isKindOf(Symbol), {
				arrayType = case
					{ arrayType == \string } { String.newClear(requestedLength) }
					{ arrayType == \int8   } { Int8Array.newClear(requestedLength) }
					{ arrayType == \int16  } { Int16Array.newClear(requestedLength >> 1) }
					{ arrayType == \int32  } { Int32Array.newClear(requestedLength >> 2) }
					{ arrayType == \double } { DoubleArray.newClear(requestedLength >> 3) }
					{ arrayType == \float  } { FloatArray.newClear(requestedLength >> 2) };
				^this.prAsArray(arrayType, requestedLength);
			});
			^nil;
		});
	}

	prAsArray {|type, len|
		_ObjC_NSDataToSCArray
		^this.primitiveFailed;
	}

	/*
	prAsPyrString {
		_ObjC_NSStringToPyrString
		^this.primitiveFailed;
	}
	*/

	registerNotification {
		|aNotificationName, aFunc, obj=1|
		if(nsDelegate.isNil, {
			this.setDelegate;
		});
		nsDelegate.prRegisterNotification(aNotificationName, aFunc);
		if (obj.notNil and:{ obj == 1 }) { obj = this };
		this.prRegisterNotification(aNotificationName, obj);
	}

	prRegisterNotification {|aNotificationName, obj|
		_ObjC_RegisterNotification
	}
}

//this is usually noy created directly. call SCNSObject-initAction instead.
CocoaAction : SCNSObjectAbstract{
	var <>action, notificationActions=nil, delegateActions=nil, <>object;

	doAction{|it|
		action.value(this, it);
	}

	doNotificationAction {
		|notif, nsNotification, obj|
		var func;
		func = notificationActions.at(notif.asSymbol);
		if(func.notNil, {
			func.value(notif, nsNotification, obj, this);
		});
	}

	doDelegateAction {
		|method, arguments|
		var result, func;
		func = delegateActions.at(method.asSymbol);
		if(func.notNil, {
			result = func.value(method, arguments);
			^result;
		});
	}

	addMethod {
		|selectorName, returntype, objctypes, aFunc|
		var types;
		if(selectorName.notNil and:{aFunc.isKindOf(Function)}, {
			if(delegateActions.isNil, {delegateActions = IdentityDictionary.new(16)});
			if(returntype.isNil, {returntype = "v"});
			types = returntype ++ "@:" ++ objctypes; // first and second types are always ID and _cmd
			this.praddMethod(selectorName, types);
			delegateActions.add(selectorName.asSymbol -> aFunc);
		});
	}

	prRegisterNotification {
		|aNotName, aFunc|
		if(aNotName.notNil, {
			if(notificationActions.isNil, {notificationActions = IdentityDictionary.new(16);});
			notificationActions.add(aNotName.asSymbol -> aFunc);
		});
	}

	praddMethod {
		|selectorName, objctypesAsString|
		_ObjC_DelegateAddSelector
		^this.primitiveFailed;
	}

	removeMethod {
		_ObjC_DelegateRemoveSelector
		^this.primitiveFailed;
	}
}

SCNSObject : SCNSObjectAbstract{

}

NSBundle : SCNSObject {
	classvar <> all;

	*new{|path|
		^super.newClear.loadBundle(path);
	}

	allocPrincipalClass{
		^SCNSObject.newFromRawPointer(this.prAllocPrincipalClass);
	}

	allocClassNamed{|name, initname, args, defer=false|
		var ptr;
		ptr = this.prAllocClassNamed(name, initname, args, defer);
		if(ptr.isNil){"could not alloc class: %".format(name).warn; ^nil};
		^SCNSObject.newFromRawPointer(ptr);
	}


	//private

	loadBundle{|path|
		this.prLoadBundle(path);
		if(this.isAllocated){
			all = all.add(this);
		}
	}

	prLoadBundle{|path|
		_ObjC_LoadBundle
	}

	prAllocPrincipalClass{
		_ObjcBundleAllocPrincipalClass
	}

	prAllocClassNamed{|name, initname, args, defer|
		_ObjcBundleAllocClassNamed
	}

}

/* cocoa-bridge by Jan Trutzschler 2005 */

NSTypeEncoding {
	*object {^"@"}
	*integer {^"i"}
	*float {^"f"}
	*double {^"d"}
	*boolean {^"i"}
}

+ SCView {
	primitive {^dataptr;}
	*newFromRawPointer {|nsptr|
	}
}
