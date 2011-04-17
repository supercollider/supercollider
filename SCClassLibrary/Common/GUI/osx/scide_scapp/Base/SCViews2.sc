SCTextFieldOld : SCNumberBoxOld {

	*viewClass { ^SCNumberBoxOld }

	defaultKeyDownAction { arg key, modifiers, unicode;
		if(unicode == 0,{ ^this });
		// standard keydown
		if ((key == 3.asAscii) || (key == $\r) || (key == $\n), { // enter key
			if (keyString.notNil,{ // no error on repeated enter
				this.valueAction_(string);
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
				this.string = keyString;
				this.stringColor = typingColor;
			},{
				keyString = String.new;
				this.string = keyString;
				this.stringColor = typingColor;
			});
			^this
		});
		if (keyString.isNil, {
			keyString = this.string;
			this.stringColor = typingColor;
		});
		keyString = keyString.add(key);
		this.string = keyString;
	}
	string_ { arg s; super.string = s.as(String); }

	defaultGetDrag {
		^this.string
	}
	defaultCanReceiveDrag {
		^currentDrag.respondsTo(\asString)
	}
	defaultReceiveDrag {
		this.valueAction = currentDrag;
	}
}



/*

SCAutoCompleteTextField : SCTextField {

	var <possibles,charIndex=0,searchIndex=0;

	possibles_ { arg list;
		possibles = list.sort;
	}

	keyDownAction { arg view, key, modifiers, unicode;
		var keyChar;
		// standard keydown
		if ((key == 3.asAscii) || (key == $\r) || (key == $\n), { // enter key
			if (keyString.notNil,{ // no error on repeated enter
				view.valueAction_(keyString);
				keyString = nil;// restart editing
				charIndex = searchIndex = 0;
			});
			^this
		});
		if (key == 127.asAscii, { // delete key
			if(keyString.notNil,{
				if(keyString.size > 1,{
					keyString = keyString.copyRange(0,charIndex = keyString.size - 2);
					// research on next tab
					searchIndex = 0;
				},{
					keyString = String.new;
					charIndex = searchIndex = 0;
				});
				view.string = keyString.asString;
				view.stringColor = Color.red;
			},{
				keyString = String.new;
				view.string = keyString;
				view.stringColor = Color.red;
				charIndex = searchIndex = 0;
			});
			^this
		});
		if (keyString.isNil, {
			keyString = view.string;
			view.stringColor = Color.red;
			searchIndex = charIndex = 0;
		});
		if(key == $\t) { // tab
			// step through
			keyChar = keyString.at(charIndex);
			for(searchIndex,possibles.size - 1,{ arg i;
				var candidate;
				candidate = possibles.at(i);
				while({ candidate.at(charIndex) == keyChar }, {
					bestMatch = candidate;
					charIndex = charIndex + 1;
				});
				//should spot gone past it...
			});
		keyString = keyString.add(key);
		view.string = keyString;
	}

}

*/


SCTabletView : SCView {

//	var <>mouseDownAction,<>mouseUpAction;

	mouseDown { arg x,y,pressure,tiltx,tilty,deviceID, buttonNumber,clickCount,absoluteZ,rotation;
		mouseDownAction.value(this,x,y,pressure,tiltx,tilty,deviceID, buttonNumber,clickCount,absoluteZ,rotation);
	}
	mouseUp { arg x,y,pressure,tiltx,tilty,deviceID, buttonNumber,clickCount,absoluteZ,rotation;
		mouseUpAction.value(this,x,y,pressure,tiltx,tilty,deviceID, buttonNumber,clickCount,absoluteZ,rotation);
	}
	doAction { arg x,y,pressure,tiltx,tilty,deviceID, buttonNumber,clickCount,absoluteZ,rotation;
		action.value(this,x,y,pressure,tiltx,tilty,deviceID, buttonNumber,clickCount,absoluteZ,rotation);
	}
}
