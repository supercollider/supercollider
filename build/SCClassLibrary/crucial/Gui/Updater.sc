
Updater { 

	var <>model,<>updateFunc;
	
	*new { arg model,updateFunc;
		^super.new.model_(model).updateFunc_(updateFunc).init
	}
	init {
		model.addDependant(this);
	}
	
	update { arg ... args;
		updateFunc.valueArray(args)
	}
	remove {
		model.removeDependant(this);
	}
	removeOnClose { arg window;
		NotificationCenter.registerOneShot(window,\didClose,this,{
			this.remove;
		})
	}
}

NotificationCenter {

	// TODO pull it out of Library for more speed
		
	*notify { arg object,message , args;
		Library.at(this,object,message).do({ arg function;
			function.valueArray(args)
		})
	}
	
	// 			who ?   theyDidWhat ?  whoCares ?  waddayawanmetodoaboutit ?
	*register { arg object,message,listener,action;
		var nr;
		nr = NotificationRegistration(object,message,listener);
		Library.put(this,object,message,listener,action);
		^nr;
	}

	*unregister { arg object,message,listener;
		var lastKey,lastDict;
		lastDict = Library.at(this,object,message);
		if(lastDict.notNil,{
			lastDict.removeAt(listener);
		});
	}
	*registerOneShot {  arg object,message,listener,action;
		Library.put(this,object,message,listener,
			{
				action.value; 
				this.unregister(object,message,listener)
			})
	}
	*clear {
		Library.global.removeAt(this)
	}
}

NotificationRegistration { // flowLayout.autorelease(notiReg)
	
	var <>object,<>message,<>listener;
	
	*new { arg o,m,l;
		^super.new.object_(o).message_(m).listener_(l)
	}
	remove {
		NotificationCenter.unregister(object,message,listener)
	}
}

