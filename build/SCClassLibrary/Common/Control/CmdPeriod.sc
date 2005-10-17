// things to clear when hitting cmd-.

CmdPeriod {
	classvar <>objects;
	
	*init {
		objects = IdentitySet.new;
	}
	
	*add { arg object;
		if(objects.isNil) { this.init }; // lazy init
		objects = objects.add(object)
	}
	*doOnce { arg object;
		var f;
		this.add(f = { this.remove(f); object.cmdPeriod  })
	}
	
	*remove { arg object;
		objects.remove(object)
	}
	
	*clear {
		objects.copy.do({ arg item; item.cmdPeriod;  });
	}


}

// things to do after startup file executed

StartUp {
	
	classvar <>functions;
	
	*add { arg func;
		functions = functions.add(func)
	}
	
	*remove { arg func;
		functions.remove(func)
	}
	
	*run {
		functions.do(_.value)
	}
	
}
