

SCTextField : SCNumberBox {

	*viewClass { ^SCNumberBox }
	
	defaultKeyDownAction { arg key, modifiers, unicode;
		if(unicode == 0,{ ^this });
		// standard keydown
		if ((key == 3.asAscii) || (key == $\r) || (key == $\n), { // enter key
			if (keyString.notNil,{ // no error on repeated enter
				this.valueAction_(keyString);
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
				this.string = keyString.asString;
				this.stringColor = Color.red;
			},{
				keyString = String.new;
				this.string = keyString;
				this.stringColor = Color.red;
			});
			^this
		});
		if (keyString.isNil, { 
			keyString = this.string;
			this.stringColor = Color.red;
		});
		keyString = keyString.add(key);
		this.string = keyString;
	}
	string_ { arg s; super.string = s.as(String); }

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


