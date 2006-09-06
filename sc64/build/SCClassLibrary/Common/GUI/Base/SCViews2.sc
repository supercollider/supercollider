

SCTextField : SCNumberBox {

	*viewClass { ^SCNumberBox }
	
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



EZSlider 
{
	var <>labelView, <>sliderView, <>numberView, <>controlSpec, <>action, <value;
	var <>round = 0.001;
	
	*new { arg window, dimensions, label, controlSpec, action, initVal, 
			initAction=false, labelWidth=80, numberWidth = 80;
		^super.new.init(window, dimensions, label, controlSpec, action, initVal, 
			initAction, labelWidth, numberWidth);
	}
	init { arg window, dimensions, label, argControlSpec, argAction, initVal, 
			initAction, labelWidth, numberWidth;
		var	decorator = window.asView.tryPerform(\decorator),
			gap = decorator.tryPerform(\gap);
		
		gap.notNil.if({
			(dimensions = dimensions.copy).x_(dimensions.x - (2*gap.x));
		});

		labelView = SCStaticText(window, labelWidth @ dimensions.y);
		labelView.string = label;
		labelView.align = \right;
		
		controlSpec = argControlSpec.asSpec;
		initVal = initVal ? controlSpec.default;
		action = argAction;
		
		sliderView = SCSlider(window, (dimensions.x - labelWidth - numberWidth) @ dimensions.y);
		sliderView.action = {
			value = controlSpec.map(sliderView.value);
			numberView.value = value.round(round);
			action.value(this);
		};
		if (controlSpec.step != 0) {
			sliderView.step = (controlSpec.step / (controlSpec.maxval - controlSpec.minval));
		};

		numberView = SCNumberBox(window, numberWidth @ dimensions.y);
		numberView.action = {
			numberView.value = value = controlSpec.constrain(numberView.value);
			sliderView.value = controlSpec.unmap(value);
			action.value(this);
		};
		
		if (initAction) {
			this.value = initVal;
		}{
			value = initVal;
			sliderView.value = controlSpec.unmap(value);
			numberView.value = value.round(round);
		};
	}
	value_ { arg value; numberView.valueAction = value }
	set { arg label, spec, argAction, initVal, initAction=false;
		labelView.string = label;
		controlSpec = spec.asSpec;
		action = argAction;
		initVal = initVal ? controlSpec.default;
		if (initAction) {
			this.value = initVal;
		}{
			value = initVal;
			sliderView.value = controlSpec.unmap(value);
			numberView.value = value.round(round);
		};
	}
	
	visible_ { |bool|
		[labelView, sliderView, numberView].do(_.visible_(bool))
	}
}



EZNumber
{
	var <>labelView, <>numberView, <>controlSpec, <>action, <value;
	var <>round = 0.001;
	
	*new { arg window, dimensions, label, controlSpec, action, initVal, 
			initAction=false, labelWidth=80, numberWidth = 80;
		^super.new.init(window, dimensions, label, controlSpec, action, initVal, 
			initAction, labelWidth, numberWidth);
	}
	init { arg window, dimensions, label, argControlSpec, argAction, initVal, 
			initAction, labelWidth, numberWidth;

		labelView = SCStaticText(window, labelWidth @ dimensions.y);
		labelView.string = label;
		labelView.align = \right;
		
		controlSpec = argControlSpec.asSpec;
		initVal = initVal ? controlSpec.default;
		action = argAction;
		
		numberView = SCNumberBox(window, numberWidth @ dimensions.y);
		numberView.action = {
			numberView.value = value = controlSpec.constrain(numberView.value);
			action.value(this);
		};
		
		if (initAction) {
			this.value = initVal;
		}{
			value = initVal;
			numberView.value = value.round(round);
		};
	}
	value_ { arg value; numberView.valueAction = value }
	set { arg label, spec, argAction, initVal, initAction=false;
		labelView.string = label;
		controlSpec = spec.asSpec;
		action = argAction;
		initVal = initVal ? controlSpec.default;
		if (initAction) {
			this.value = initVal;
		}{
			value = initVal;
			numberView.value = value.round(round);
		};
	}
	
	visible_ { |bool|
		[labelView, numberView].do(_.visible_(bool))
	}
}
