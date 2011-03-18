AbstractResponder {
	var <removeOnCmdPeriod = true, <added = false;
	
	removeOnCmdPeriod_ {|bool| 
		removeOnCmdPeriod = bool;
		if(bool && added, { CmdPeriod.add(this) }, { CmdPeriod.remove(this) });
	}
	
	cmdPeriod { this.remove }
	
}