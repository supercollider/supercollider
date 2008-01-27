

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

		labelView = GUI.staticText.new(window, labelWidth @ dimensions.y);
		labelView.string = label;
		labelView.align = \right;
		
		controlSpec = argControlSpec.asSpec;
		initVal = initVal ? controlSpec.default;
		action = argAction;
		
		sliderView = GUI.slider.new(window, (dimensions.x - labelWidth - numberWidth) @ dimensions.y);
		sliderView.action = {
			this.valueAction_(controlSpec.map(sliderView.value));
		};
		if (controlSpec.step != 0) {
			sliderView.step = (controlSpec.step / (controlSpec.maxval - controlSpec.minval));
		};

		sliderView.receiveDragHandler = { arg slider;
			slider.valueAction = controlSpec.unmap(GUI.view.currentDrag);
		};
		
		sliderView.beginDragAction = { arg slider;
			controlSpec.map(slider.value)
		};

		numberView = GUI.numberBox.new(window, numberWidth @ dimensions.y);
		numberView.action = { this.valueAction_(numberView.value) };
		
		if (initAction) {
			this.valueAction_(initVal);
		}{
			this.value_(initVal);
		};
	}
	
	value_ { arg val; 
		value = controlSpec.constrain(val);
		numberView.value = value.round(round);
		sliderView.value = controlSpec.unmap(value);
	}
	valueAction_ { arg val; 
		this.value_(val);
		this.doAction;
	}
	doAction { action.value(this) }

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
	
	visible { ^sliderView.visible }
	visible_ { |bool| [labelView, sliderView, numberView].do(_.visible_(bool)) }
	
	enabled {  ^sliderView.enabled } 
	enabled_ { |bool| [sliderView, numberView].do(_.enabled_(bool)) }
	
	remove { [labelView, sliderView, numberView].do(_.remove) }
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

		labelView = GUI.staticText.new(window, labelWidth @ dimensions.y);
		labelView.string = label;
		labelView.align = \right;
		
		controlSpec = argControlSpec.asSpec;
		initVal = initVal ? controlSpec.default;
		action = argAction;
		
		numberView = GUI.numberBox.new(window, numberWidth @ dimensions.y);
		numberView.action = {
			this.valueAction_(numberView.value);
		};
		
		if (initAction) {
			this.valueAction = initVal;
		}{
			this.value = initVal;
		};
	}
	value_ { arg val; 
		value = controlSpec.constrain(val);
		numberView.value = value.round(round);
	}
	valueAction_ { arg val; 
		this.value_(val);
		this.doAction;
	}
	doAction { action.value(this) }

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
	
	visible { ^numberView.visible }
	visible_ { |bool| [labelView, numberView].do(_.visible_(bool)) }
	
	enabled {  ^numberView.enabled } 
	enabled_ { |bool| numberView.enabled_(bool) }
	
	remove { [labelView, numberView].do(_.remove) }
}
