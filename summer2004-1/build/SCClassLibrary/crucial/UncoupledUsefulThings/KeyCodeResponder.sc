
/*
	todo: 
		add command, function and help modifier keys

*/

KeyCodeResponder {

	classvar global,<map;
	
	var <>dict;
	
	*new { ^super.new.clear }
	
	value {arg view,char,modifier,unicode,keycode;
		^this.at(keycode).value(keycode,modifier)
	}

	registerKeycode { arg modifier,keycode,function;
		var kdr;
		kdr = dict.at(keycode);
		if(kdr.isNil,{
			kdr = KeyCodeResponderStack.new;
			dict.put(keycode,kdr);
		});
		kdr.addSimple(modifier,function);
	}
	normal {arg ... assns; 
		assns.do({arg as;
			this.registerKeycode(0,as.key,as.value)
		})
	}
	shift {arg ... assns; 
		assns.do({arg as;
			this.registerKeycode(131072,as.key,as.value)
		})
	}
	control {arg ... assns; 
		assns.do({arg as;
			this.registerKeycode(262144,as.key,as.value)
		})
	}
	option {arg ... assns; 
		assns.do({arg as;
			this.registerKeycode(524288,as.key,as.value)
		})
	}
	registerKeycodeAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\registerKeycode,set)
		})
	}
	
	
	/** GLOBAL **/	
	*registerKeycode { arg modifier,keycode,function;
		this.global.registerKeycode(modifier,keycode, function)
	}
	*registerKeycodeAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\registerKeycode,set)
		})
	}	
	*value {arg view,keycode,modifier,unicode;
		^this.at(unicode).value(unicode,modifier)
	}
	*global { 
		^global ?? {
				global = this.new;
				SCView.globalKeyDownAction = global;
				global
		} 
	}

	*registerAscii { arg modifier,ascii,function;
		this.pushForKeycode(map.at(ascii.asAscii),modifier,modifier bitXor: 31, function)
	}
	*registerChar { arg modifier=0,char,function;
		this.pushForKeycode(map.at(char),modifier,modifier bitXor: 31, function)
	}
	*registerAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\register,set)
		})
	}

	*register { arg keycode,shift,caps,opt,cntl,function;
		this.global.register(keycode,shift,caps,opt,cntl,function);
	}
	register { arg keycode,shift,caps,opt,cntl,function;
		var require=0,deny=0;
		if(shift.notNil,{
			if(shift,{
				require = require + 131072;
			},{
				deny = deny + 131072;
			})
		});
		if(caps.notNil,{
			if(caps,{
				require = require + 65536;
			},{
				deny = deny + 65536;
			})
		});
		if(opt.notNil,{
			if(opt,{
				require = require + 524288;
			},{
				deny = deny + 524288;
			})
		});
		if(cntl.notNil,{
			if(cntl,{
				require = require + 262144;
			},{
				deny = deny + 262144;
			})
		});
		this.pushForKeycode(keycode,require,deny,function);
	}

	clear { dict = IdentityDictionary.new }
	*clear { this.global.clear }
	remove { this.clear }
	*remove { SCView.globalKeyDownAction = nil; }
	
	*tester {
		this.clear;
		Sheet({ arg l;
			ActionButton.new(l,
			"while focused on this button, press keys and modifiers to post a code template").focus
				.keyDownAction_({ arg v,c,m,u,k;
					("KeyCodeResponder.registerKeycode(" + m + "," + k + ",{      });// " + c ).postln;
				});
		})
	}

	*at { arg  address;  ^this.global.at(address) }
	*put { arg address,val;  this.global.put(address,val) }
	at { arg key; ^dict.at(key) }
	put { arg key,value; dict.put(key,value) }

	*pushForKeycode { arg keycode,requireMask,denyMask,function;
		this.global.pushForKeycode(keycode,requireMask,denyMask,function)
	}
	pushForKeycode { arg keycode,requireMask,denyMask,function;
		var kdr;
		kdr = this.at(keycode);
		if(kdr.isNil,{
			kdr = KeyCodeResponderStack.new;
			this.put(keycode,kdr);
		});
		kdr.add(requireMask,denyMask,function);
	}
	*resetKeycode { arg keycode;
		this.at(keycode).reset
	}
	++ { arg that;
		var new,keys;
		if(that.isNil,{ ^this });
		// that overides this
		new = KeyCodeResponder.new;
		new.dict = dict.copy;
		that.dict.keysValuesDo({ arg keycode,kdrstack;
			new.put(keycode, kdrstack ++ this.at(keycode))
		});
		^new
	}
}

KeyCodeResponderStack { 
	// only needs to parse the modifier
	// but passes ascii, code and modifer to the function
	
	var <>stack;
	*new {	^super.new.reset }
	add { arg requireMask,denyMask,function;
		this.addKDR( KDRUnit(requireMask,denyMask,function) );
	}
	addSimple { arg modifier,function;
		this.addKDR( SimpleKDRUnit(modifier,function) );
	}
	addKDR { arg newGuy;
		var oldIndex;
		oldIndex = stack.indexOfEqual(newGuy);
		if(oldIndex.notNil,{
			stack.put(oldIndex,newGuy) // replaces old
		},{
			stack = stack.add(newGuy);
		});
	}	
	reset { stack = [] }
	value { arg char,modifier,unicode,keycode;
		stack.do({ arg responder;
			responder.value(char,modifier,unicode,keycode)
		})
	}
	++ { arg that;
		var new;
		if(that.isNil,{ ^this });
		new = this.class.new.stack_(stack);
		that.stack.do({ arg kdr;
			new.addKDR(kdr)
		});
		^new
	}
}

// maybe faster to build a dict for modifiers too

SimpleKDRUnit { // exact modifier match only

	 var <>requireMask,<>function;
	 
	*new { arg modifier,function;
		^super.newCopyArgs(modifier,function)
	}
	value { arg char,modifier,unicode,keycode;
		if(modifier == requireMask,{function.value(char,modifier,unicode,keycode)})
	}
	== { arg that;
		^that.requireMask == requireMask and: {this.class === that.class }
	}
}

KDRUnit : SimpleKDRUnit {
	
	var <>denyMask;

	*new { arg requireMask,denyMask,function;
		^super.new(requireMask,function).denyMask_(denyMask)
	}
	value { arg char,modifier,unicode,keycode;
		if((modifier & requireMask) == requireMask // all required bits set
			and: 
		{  (denyMask & modifier) == 0 } // no denied bits present
		,{
			function.value(char,modifier,unicode,keycode)
		})
	}
	== { arg aResponder;
		^(aResponder.requireMask == requireMask and: {aResponder.denyMask == denyMask})
	}
}

