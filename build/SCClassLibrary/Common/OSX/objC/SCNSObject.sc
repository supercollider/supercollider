SCNSObjectAbstract {
	var <dataptr, <> className, < nsAction, < nsDelegate;
	
	*new{|classname, initname, args, defer=false|
		^super.new.init(classname, initname, args, defer)
	}
	
	
	invoke{|method, args, defer=false|
		var result;
		args = args ? []; //todo: fix that in the primitive ...
		result = this.prInvoke(method, args, defer);
		^result.asNSReturn
	}
	
	release {
		this.prDealloc;
		dataptr = nil;
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
		
	initWith{| classname, initname,args|
		this.prAllocWith( classname, initname,args)
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

/* cocoa-bridge by Jan Trutzschler 2005 */

