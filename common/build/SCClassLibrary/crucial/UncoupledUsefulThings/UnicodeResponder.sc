
UnicodeResponder {

	classvar global;
	var <>dict;

	*new { ^super.new.clear }

	// use this as your view's keyDownAction in place of a function
	// view.keyDownAction = unicodeResponder
	value { arg view,char,modifier,unicode,keycode;
		^this.at(unicode).value(view, unicode,modifier)
	}

	// ur.normal( unicode -> { } [, unicode -> { } ... ]  )
	normal { arg ... assns;
		assns.do({ arg as;
			this.register(as.key,false,false,false,false,as.value)
		})
	}
	shift { arg ... assns;
		assns.do({ arg as;
			this.register(as.key,true,false,false,false,as.value)
		})
	}
	control { arg ... assns;
		assns.do({ arg as;
			this.register(as.key,false,false,true,false,as.value)
		})
	}
	option { arg ... assns;
		assns.do({ arg as;
			this.register(as.key,false,false,false,true,as.value)
		})
	}
	// for combinations: true/false/nil
	register { arg unicode,shift,caps,opt,cntl,function,description;
		var require=0,deny=0;
		require = [];
		deny = [];
		if(shift.notNil,{
			if(shift,{
				require = require.add(KeyCodeResponder.shiftModifier);
			},{
				deny = deny.add( KeyCodeResponder.shiftModifier );
			})
		});
		if(caps.notNil,{
			if(caps,{
				require = require.add(KeyCodeResponder.capsModifier);
			},{
				deny = deny.add(KeyCodeResponder.capsModifier);
			})
		});
		if(opt.notNil,{
			if(opt,{
				require = require.add(KeyCodeResponder.optionModifier);
			},{
				deny = deny.add(KeyCodeResponder.optionModifier);
			})
		});
		if(cntl.notNil,{
			if(cntl,{
				require = require.add(KeyCodeResponder.controlModifier);
			},{
				deny = deny.add(KeyCodeResponder.controlModifier);
			})
		});
		deny = deny.add(KeyCodeResponder.commandModifier);

		this.pushForUnicode(unicode.asUnicode,require,deny,function,description);
	}

	// you can concatenate responders
	++ { arg that;
		var new,keys;
		if(that.isNil,{ ^this });
		if(that.class !== this.class,{ die("Can't mix responder classes:", this.class, that.class) });
		// that overides this
		new = this.class.new;
		new.dict = dict.copy;
		that.dict.keysValuesDo({ arg unicode,kdrstack;
			new.put(unicode, kdrstack ++ this.at(unicode))
		});
		^new
	}


	/** GLOBAL
	  * installs itself in SCView.globalKeyDownAction
	  */
	*register { arg unicode,shift,caps,opt,cntl,function;
		this.global.register(unicode.asUnicode,shift,caps,opt,cntl,function);
	}
	*clear { this.global.clear }
	*remove { SCView.globalKeyDownAction = nil; }

	// less useful
	// matches only if the modifier is present
	*registerUnicode { arg modifier,unicode,function;
		this.global.registerUnicode(modifier,unicode.asUnicode, function)
	}
	*registerUnicodeAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\registerUnicode,set)
		})
	}
	*registerAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\register,set)
		})
	}
	registerUnicode { arg modifier,unicode,function;
		var kdr;
		unicode = unicode.asUnicode;
		kdr = dict.at(unicode);
		if(kdr.isNil,{
			kdr = KeyCodeResponderStack.new;
			dict.put(unicode,kdr);
		});
		kdr.addSimple(modifier,function);
	}



	// private
	registerUnicodeAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\registerUnicode,set)
		})
	}

	clear { dict = IdentityDictionary.new }
	remove { this.clear }

	*tester {
		this.clear;
		Sheet({ arg l;
			ActionButton.new(l,
				"while focused on this button, press keys and modifiers to post a code template").focus
				.keyDownAction_({ arg v,c,m,u,k;
					var words="",boos="";
					//"k = UnicodeResponder.new;".postln;
					"// ".post;
					[\shift->KeyCodeResponder.shiftModifier, \caps->KeyCodeResponder.capsModifier,
					\option->KeyCodeResponder.optionModifier, \control->KeyCodeResponder.controlModifier].do({ |modass|
						if((m & modass.value) == modass.value,{
							words = words + modass.key;
							boos = boos + "true,";
						},{
							boos = boos + "false,";
						});
					});

					words.post; " ".post;
					if(c.isPrint,{
						c.postln;
					},{
						u.postln;
					});

					("k.register(  " + u + " , " + boos + "{").postln;
					"".postln;
					"});".postln;
				});
		})
	}

	*at { arg  address;  ^this.global.at(address) }
	*put { arg address,val;  this.global.put(address,val) }
	at { arg key; ^dict.at(key) }
	put { arg key,value; dict.put(key,value) }

	*pushForUnicode { arg unicode,requireMask,denyMask,function,description;
		this.global.pushForUnicode(unicode,requireMask,denyMask,function,description)
	}
	pushForUnicode { arg unicode,requireMask,denyMask,function,description;
		var kdr;
		unicode = unicode.asUnicode;
		kdr = this.at(unicode);
		if(kdr.isNil,{
			kdr = KeyCodeResponderStack.new;
			this.put(unicode,kdr);
		});
		kdr.addMaskTester(requireMask,denyMask,function,description);
	}
	*resetUnicode { arg unicode;
		this.at(unicode.asUnicode).reset
	}
	*value { arg view,char,modifiers,unicode,keycode;
		^this.at(unicode).value(view, char,modifiers,unicode,keycode)
	}
	*global {
		^global ?? {
			global = this.new;
			SCView.globalKeyDownAction = global;
			global
		}
	}
	guiClass { ^KeyCodeResponderGui }
	report {
		dict.keysValuesDo({ |k,v|
			Post.nl;
			//Post << k.asAscii;
			v.report(k);
			//Post << v;
		});
	}
}
