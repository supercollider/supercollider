//things to clear when hitting cmd-.

CmdPeriod {
	classvar <>objects;
	
	*add { arg object;
		objects = objects.add(object)
	}
	
	*remove { arg object;
		objects.remove(object)
	}
	
	*clear {
		
		objects.copy.do({ arg item; item.cmdPeriod;  });
	}


}