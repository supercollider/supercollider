

KeyCodeResponder {

	classvar global, 
		<>normalModifier=0,<>shiftModifier=131074,<>controlModifier=262145,
		<>optionModifier=524320,<>functionKeyModifier=8388608,<>capsModifier=65536;
	
	var <>dict;
	
	*new { ^super.new.clear }
	*setModifiers { arg n,s,c,o,f,caps;
		normalModifier = n;
		shiftModifier = s;
		controlModifier = c;
		optionModifier = o;
		functionKeyModifier = f;
		capsModifier = caps;
	}
	
	normal { arg ... asses; 
		asses.do({ arg as;
			this.register(as.key,false,false,false,false,as.value)
		})
	}
	
	shift { arg ... asses; 
		asses.do({ arg as;
			this.register(as.key,true,false,false,false,as.value)
		})
	}
	control { arg ... asses; 
		asses.do({ arg as;
			this.register(as.key,false,false,false,true,as.value)
		})
	}
	option { arg ... asses; 
		asses.do({ arg as;
			this.register(as.key,false,false,true,false,as.value)
		})
	}
	
	*register { arg keycode,shift,caps,opt,cntl,function;
		this.global.register(keycode,shift,caps,opt,cntl,function);
	}
	*registerAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\register,set)
		})
	}
	register { arg keycode,shift,caps,opt,cntl,function;
		var require=0,deny=0;
		require = [];
		deny = [];
		if(shift.notNil,{
			if(shift,{
				require = require.add(shiftModifier);
			},{
				deny = deny.add( shiftModifier );
			})
		});
		if(caps.notNil,{
			if(caps,{
				require = require.add(capsModifier);
			},{
				deny = deny.add(capsModifier);
			})
		});
		if(opt.notNil,{
			if(opt,{
				require = require.add(optionModifier);
			},{
				deny = deny.add(optionModifier);
			})
		});
		if(cntl.notNil,{
			if(cntl,{
				require = require.add(controlModifier);
			},{
				deny = deny.add(controlModifier);
			})
		});
		this.pushForKeycode(keycode,require,deny,function);
	}	

	*registerKeycode { arg modifier,keycode,function;
		this.global.registerKeycode(modifier,keycode, function)
	}
	*registerKeycodeAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\registerKeycode,set)
		})
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
	registerKeycodeAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\registerKeycode,set)
		})
	}
	
	clear { dict = IdentityDictionary.new }
	*clear { this.global.clear }
	remove { this.clear }
	*remove { SCView.globalKeyDownAction = nil; }
	*resetKeycode { arg keycode;
		this.at(keycode).reset
	}
	
	*tester {
		this.clear;
		Sheet({ arg l;
			ActionButton.new(l,
			"while focused on this button, press keys and modifiers to post a code template").focus
				.keyDownAction_({ arg v,c,m,u,k;
					//("view:"+v+" char:"+c+" mod:"+m+" unicode:"+u+" keycode:"+k).postln;
					("KeyCodeResponder.registerKeycode(" + m + "," + k + ",{      });// " + c ).postln;
				});
		})
	}

	++ { arg that;
		var new,keys;
		if(that.isNil,{ ^this });
		if(that.class !== this.class,{ 
			die("Can't concatenate KeyCode/Unicode responder classes:", this.class, that.class) 
		});

		// that overides this
		new = KeyCodeResponder.new;
		new.dict = dict.copy;
		that.dict.keysValuesDo({ arg keycode,kdrstack;
			new.put(keycode, kdrstack ++ this.at(keycode))
		});
		^new
	}
	
	
	/** PRIVATE **/
	
	/** the actual responder **/
	value { arg view,char,modifier,unicode,keycode;
		^this.at(keycode).value(char,modifier,unicode,keycode)
	}
	*value { arg view,keycode,modifier,unicode;
		^this.at(unicode).value(unicode,modifier)
	}
	
	*global { 
		^global ?? {
				global = this.new;
				SCView.globalKeyDownAction = global;
				global
		} 
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
		kdr.addMaskTester(requireMask,denyMask,function);
	}
}

// used by both UnicodeResponder and KeyCodeResponder
KeyCodeResponderStack {

	// only needs to parse the modifier
	// but passes ascii, code and modifer to the function
	
	var <>stack;
	
	*new {	^super.new.reset }
	addMaskTester { arg requireMask,denyMask,function;
		this.addKDR( KDRMaskTester(requireMask,denyMask,function) );
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

SimpleKDRUnit { // exact single modifier match only

	 var <>requireMask,<>function;
	 
	*new { arg modifier,function;
		^super.newCopyArgs(modifier,function)
	}
	value { arg char,modifier,unicode,keycode;
		if((modifier & requireMask) == requireMask,{function.value(char,modifier,unicode,keycode)})
	}
	== { arg that;
		^(this.class == that.class) and:
			{ that.requireMask == requireMask }
	}
}


// test multiple modifiers deny and require
KDRMaskTester : SimpleKDRUnit {
	
	var <>denyMask;

	*new { arg requireMask,denyMask,function;
		var r=0,d=0;
		requireMask.do({ arg m; r = r | m });
		denyMask.do({ arg m; d = d | m });
		^super.new(r,function).denyMask_(d)
	}
	value { arg char,modifier,unicode,keycode;
		if((modifier & requireMask) == requireMask // all required bits set
			and: 
		{  (denyMask & modifier) == 0 } // no denied bits present
		,{
			function.value(char,modifier,unicode,keycode);
		})
	}
	== { arg aResponder;
		^(this.class === aResponder.class) and: {
			(aResponder.requireMask == requireMask and: {
				aResponder.denyMask == denyMask
			})
		}
	}
}


// this bit of cleverness now mysteriously fails to work
// KDRUnit : SimpleKDRUnit {
//	
//	var <>denyMask;
//
//	*new { arg requireMask,denyMask,function;
//		^super.new(requireMask,function).denyMask_(denyMask)
//	}
//	value { arg char,modifier,unicode,keycode;
//		// no longer works :(
//		
//		//["char",char,"m",modifier,"u",unicode,keycode,"rmask",requireMask,"test",(modifier & requireMask) == requireMask, "dmask",denyMask,"dmask test", (denyMask & modifier) == 0].debug;
//	
//		if((modifier & requireMask) == requireMask // all required bits set
//			and: 
//		{  (denyMask & modifier) == 0 } // no denied bits present
//		,{
//			function.value(char,modifier,unicode,keycode)
//		})
//	}
//	== { arg aResponder;
//		^(this.class === aResponder.class) and: {
//			(aResponder.requireMask == requireMask and: {
//				aResponder.denyMask == denyMask
//			})
//		}
//	}
//}
