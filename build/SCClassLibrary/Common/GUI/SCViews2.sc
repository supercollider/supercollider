
SCViewAdapter { // SCViewHolder
	
	// SCView classes can't be subclassed.
	// SCViewAdapter makes it possible to wrap more capabilities by holding, not subclassing
	// has a , not is a
	// alternative is for SCView to pass in the name of the c++ view to prInit primitive
	
	var <>view;
	
	action_ { arg f; view.action_(f) }
	font_ { arg f;
		view.font = f;
	}
	keyDownAction_ { arg f;
		view.keyDownAction_(f);
	}
	asView { ^view }
	bounds { ^view.bounds }
	resize_ { arg r; view.resize_(r) }
	refresh { view.refresh }
	background_ { arg b; view.background_(b) }
}


SCTextField : SCViewAdapter {

	var keyString;
	
	*new { arg parent,bounds;
		^super.new.init(parent,bounds)
	}
	init { arg parent,bounds;
		view = SCNumberBox(parent,bounds);
		view.keyDownAction =  { arg k,m,u; this.keyDownAction(k,m,u) };
	}
	keyDownAction { arg view, key, modifiers, unicode;
		// standard keydown
		if ((key == 3.asAscii) || (key == $\r) || (key == $\n), { // enter key
			if (keyString.notNil,{ // no error on repeated enter
				view.valueAction_(keyString);
				keyString = nil;// restart editing
			});
			^this
		});
		if (key == 127.asAscii, { // delete key
			if(keyString.notNil,{
				if(keyString.size > 1,{
					keyString = keyString.copyRange(0,keyString.size - 2);
				},{
					keyString = String.new;
				});
				view.string = keyString.asString;
				view.stringColor = Color.red;
			},{
				keyString = String.new;
				view.string = keyString;
				view.stringColor = Color.red;
			});
			^this
		});
		if (keyString.isNil, { 
			keyString = view.string;
			view.stringColor = Color.red;
		});
		keyString = keyString.add(key);
		view.string = keyString;
	}
	string_ { arg s; view.string = s; }
	string { ^view.string }

}

/*

SCAutoCompleteTextField : SCTextField {
	
	var <possibles,searchIndex=0;
	
	possibles_ { arg list;
		possibles = list.sort;
	}
	
	keyDownAction { arg view, key, modifiers, unicode;
		// standard keydown
		if ((key == 3.asAscii) || (key == $\r) || (key == $\n), { // enter key
			if (keyString.notNil,{ // no error on repeated enter
				view.valueAction_(keyString);
				keyString = nil;// restart editing
			});
			^this
		});
		if (key == 127.asAscii, { // delete key
			if(keyString.notNil,{
				if(keyString.size > 1,{
					keyString = keyString.copyRange(0,keyString.size - 2);
				},{
					keyString = String.new;
				});
				view.string = keyString.asString;
				view.stringColor = Color.red;
			},{
				keyString = String.new;
				view.string = keyString;
				view.stringColor = Color.red;
			});
			^this
		});
		if (keyString.isNil, { 
			keyString = view.string;
			view.stringColor = Color.red;
		});
		if(key == $\t) { // tab
			// step through 
			for(searchIndex,possibles.size - 1,{ arg i;
				// extend 
				candidate = possibles.at(i);
				// what char are we scanning from ?
				if(candidate.at(ci),{
					...
				})
			});
		keyString = keyString.add(key);
		view.string = keyString;
	}

}

*/


