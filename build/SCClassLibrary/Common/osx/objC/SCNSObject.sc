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
	var <dataptr, <> className, < nsAction, < nsDelegate;
	
	*new{|classname, initname, args, defer=false|
		^super.new.init(classname.asString, initname, args, defer)
	}
	
	*getClass{|classname| //return only the class id, the object is not allocated yet ...
		^super.new.initClass(classname);
	}
	
	invoke{|method, args, defer=false|
		var result;
		args = args ? []; //todo: fix that in the primitive ...
		result = this.prInvoke(method, args, defer);
		^result.asNSReturn
	}
	
	initClass{|name|
		this.prGetClass(name)
	}
	
	release {
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
		^out
	}
	setDelegate{
		var out;
		out = CocoaAction.newClear;
		this.prSetDelegate(out);
		out.prSetClassName;
		nsDelegate = out;
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
		className = cn;
		this.prAllocInit(cn, in, args, defer);
	}
	
	prAllocInit { arg classname, initname,args;
		_ObjC_AllocInit;
		^this.primitiveFailed;
	}
	
	prDealloc {
		_ObjC_Dealloc;		
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
}

//this is usually noy created directly. call SCNSObject-initAction instead.
CocoaAction : SCNSObjectAbstract{
	var <>action;
	doAction{|it|
		action.value(this, it);
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
	
	allocClassNamed{|name|
		^SCNSObject.newFromRawPointer(this.prAllocClassNamed(name));
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
	
	prAllocClassNamed{|name|
		_ObjcBundleAllocClassNamed
	}
	
}

/* cocoa-bridge by Jan Trutzschler 2005 */

