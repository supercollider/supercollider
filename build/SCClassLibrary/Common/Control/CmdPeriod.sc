//things to clear when hitting cmd-.

CmdPeriod {
	classvar <>objects;
	
	*init {
		objects = IdentitySet.new;
	}
	
	*add { arg object;
		if(objects.isNil) { this.init }; // lazy init
		objects = objects.add(object)
	}
	
	
	*remove { arg object;
		objects.remove(object)
	}
	
	*clear {
		objects.copy.do({ arg item; item.cmdPeriod;  });
	}


}