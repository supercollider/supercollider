
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
			//this.insp("removing");
			this.remove;
		})
	}

}

/**

	var tog;
	layout=PageLayout.guify(layout);
	
	tog=ToggleButton(layout,name ? "process",{ this.on },{ this.off });
	
	layout.removeOnClose(
		Updater(this,{
			tog.passiveToggle(state)
		})
	)	
	

**/
