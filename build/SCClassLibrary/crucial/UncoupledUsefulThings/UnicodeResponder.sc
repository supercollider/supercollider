
// should share a superclass with KeyCodeResonder

UnicodeResponder {

	classvar global;
	
	var <>dict;
	
	*new { ^super.new.clear }
	
	value { arg view,char,modifier,unicode,keycode;
		^this.at(unicode).value(unicode,modifier)
	}

	registerUnicode { arg modifier,unicode,function;
		var kdr;
		kdr = dict.at(unicode);
		if(kdr.isNil,{
			kdr = KeyCodeResponderStack.new;
			dict.put(unicode,kdr);
		});
		kdr.addSimple(modifier,function);
	}
	normal { arg ... assns; 
		assns.do({ arg as;
			this.registerUnicode(0,as.key,as.value)
		})
	}
	shift { arg ... assns; 
		assns.do({ arg as;
			this.registerUnicode(131072,as.key,as.value)
		})
	}
	control { arg ... assns; 
		assns.do({ arg as;
			this.registerUnicode(262144,as.key,as.value)
		})
	}
	option { arg ... assns; 
		assns.do({ arg as;
			this.registerUnicode(524288,as.key,as.value)
		})
	}
	registerUnicodeAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\registerUnicode,set)
		})
	}
	
	
	/** GLOBAL **/	
	*registerUnicode { arg modifier,ascii,function;
		this.global.registerUnicode(modifier,ascii, function)
	}
	*registerUnicodeAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\registerUnicode,set)
		})
	}	
	*value { arg view,char,modifiers,unicode,keycode;
		^this.at(unicode).value(char,modifiers,unicode,keycode)
	}
	*global { 
		^global ?? {
				global = this.new;
				SCView.globalKeyDownAction = global;
				global
		} 
	}

	*registerAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\register,set)
		})
	}

	*register { arg unicode,shift,caps,opt,cntl,function;
		this.global.register(unicode,shift,caps,opt,cntl,function);
	}
	register { arg unicode,shift,caps,opt,cntl,function;
		var require=0,deny=0;
		if(shift.notNil,{
			if(shift,{
				require = require + KeyCodeResponder.shiftModifier;
			},{
				deny = deny + KeyCodeResponder.shiftModifier;
			})
		});
		if(caps.notNil,{
			if(caps,{
				require = require + KeyCodeResponder.capsModifier;
			},{
				deny = deny + KeyCodeResponder.capsModifier;
			})
		});
		if(opt.notNil,{
			if(opt,{
				require = require + KeyCodeResponder.optionModifier;
			},{
				deny = deny + KeyCodeResponder.optionModifier;
			})
		});
		if(cntl.notNil,{
			if(cntl,{
				require = require + KeyCodeResponder.controlModifier;
			},{
				deny = deny + KeyCodeResponder.controlModifier;
			})
		});
		this.pushForUnicode(unicode,require,deny,function);
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
					("UnicodeResponder.registerUnicode(" + m + "," + u + ",{      });// " + c ).postln;
				});
		})
	}

	*at { arg  address;  ^this.global.at(address) }
	*put { arg address,val;  this.global.put(address,val) }
	at { arg key; ^dict.at(key) }
	put { arg key,value; dict.put(key,value) }

	*pushForUnicode { arg unicode,requireMask,denyMask,function;
		this.global.pushForUnicode(unicode,requireMask,denyMask,function)
	}
	pushForUnicode { arg unicode,requireMask,denyMask,function;
		var kdr;
		kdr = this.at(unicode);
		if(kdr.isNil,{
			kdr = KeyCodeResponderStack.new;
			this.put(unicode,kdr);
		});
		kdr.add(requireMask,denyMask,function);
	}
	*resetUnicode { arg unicode;
		this.at(unicode).reset
	}
	++ { arg that;
		var new,keys;
		if(that.isNil,{ ^this });
		if(that.class !== this.class,{ die("Can't mix resonder classes:", this.class, that.class) });
		// that overides this
		new = this.class.new;
		new.dict = dict.copy;
		that.dict.keysValuesDo({ arg unicode,kdrstack;
			new.put(unicode, kdrstack ++ this.at(unicode))
		});
		^new
	}
}


