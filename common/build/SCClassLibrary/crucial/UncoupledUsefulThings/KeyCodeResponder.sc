

KeyCodeResponder {

       classvar global;

       const <normalModifier       = 0;
       const <capsModifier         = 0x00010000;
       const <shiftModifier        = 0x00020000;
       const <controlModifier      = 0x00040000;
       const <optionModifier       = 0x00080000;
       const <functionKeyModifier  = 0x00800000;
       const <commandModifier = 0x00100000;

/* 	classvar <>normalModifier=0,<>shiftModifier=0x00020000,<>controlModifier=0x00040000,
               <>optionModifier=0x00080000,<>functionKeyModifier=0x00800000,<>capsModifier=0x00010000;
*/
	var <>dict;

	// keycode -> { }, keycode -> { }, ...

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

	// true, false or nil
	register { arg keycode,shift,caps,opt,cntl,function,description;
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
		deny = deny.add(commandModifier);
		this.pushForKeycode(keycode,require,deny,function,description);
	}

	// true, false or nil
	*register { arg keycode,shift,caps,opt,cntl,function;
		this.global.register(keycode,shift,caps,opt,cntl,function);
	}
	// [ keycode,shift,caps,opt,cntl,function],[ keycode,shift,caps,opt,cntl,function], ...
	*registerAll { arg ... sets;
		sets.do({ arg set;
			this.performList(\register,set)
		})
	}

	clear { dict = IdentityDictionary.new }
	*clear { this.global.clear }
	// remove me when you close your window !
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
					var words="",boos="";
					//"k = KeyCodeResponder.new;".postln;
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
						k.postln;
					});

					("k.register(  " + k + " , " + boos + "{").postln;
					"".postln;
					"});".postln;
				});
		})
	}

	// concatenate responders
	++ { arg that;
		var new,keys;
		if(that.isNil,{ ^this });
		if(that.class !== this.class,{
			this.die("Can't concatenate KeyCode and Unicode responder classes:", this.class, that.class)
		});

		// that overides this
		new = KeyCodeResponder.new;
		new.dict = dict.copy;
		that.dict.keysValuesDo({ arg keycode,kdrstack;
			new.put(keycode, kdrstack ++ this.at(keycode))
		});
		^new
	}

	*new { ^super.new.clear }

	/** PRIVATE **/
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



	/** the actual responder **/
	value { arg view,char,modifier,unicode,keycode;
		^this.at(keycode).value(view, char,modifier,unicode,keycode);
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

	*pushForKeycode { arg keycode,requireMask,denyMask,function,description;
		this.global.pushForKeycode(keycode,requireMask,denyMask,function,description)
	}
	pushForKeycode { arg keycode,requireMask,denyMask,function,description;
		var kdr;
		kdr = this.at(keycode);
		if(kdr.isNil,{
			kdr = KeyCodeResponderStack.new;
			this.put(keycode,kdr);
		});
		kdr.addMaskTester(requireMask,denyMask,function,description);
	}
	*maskToString { arg m;
		var words;
		words = "";
		[\shift->KeyCodeResponder.shiftModifier, \caps->KeyCodeResponder.capsModifier,
		\option->KeyCodeResponder.optionModifier, \control->KeyCodeResponder.controlModifier, \command->KeyCodeResponder.commandModifier]
		.do({ |modass|
			if((m & modass.value) == modass.value,{
				words = words + modass.key;
				//boos = boos + "true,";
			},{
				//boos = boos + "false,";
			});
		});
		^words
	}
	report {
		dict.keysValuesDo({ |k,v|
			Post.nl;
			//Post << k.asAscii;
			v.report(k);
			//Post << v;
		});
	}
	guiClass { ^KeyCodeResponderGui }
}

// used by both UnicodeResponder and KeyCodeResponder
KeyCodeResponderStack {

	// only needs to parse the modifier
	// but passes ascii, code and modifer to the function

	var <>stack;

	*new {	^super.new.reset }
	addMaskTester { arg requireMask,denyMask,function,description;
		this.addKDR( KDRMaskTester(requireMask,denyMask,function,description) );
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
	value { arg view, char,modifier,unicode,keycode;
		var result;
		stack.do({ arg responder;
			result = responder.value(view,char,modifier,unicode,keycode);
		});
		^result
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
	guiClass { ^KeyCodeResponderStackGui }
	report { arg key;
		stack.do({ |kdr|
			Post << Char.nl;
			if(key.asAscii.isPrint,{ Post << key.asAscii },{ Post << key; });
			kdr.report;
		})
	}
}

SimpleKDRUnit {
	// matches if the modifier combo (or single) is present,
	// but does NOT FAIL if others are also present

	 var <>requireMask,<>function,<>description;

	*new { arg modifier,function,description;
		^super.newCopyArgs(modifier,function,description)
	}
	value { arg view,char,modifier,unicode,keycode;
		if((modifier & requireMask) == requireMask,{function.value(char,modifier,unicode,keycode)})
	}
	== { arg that;
		^this.compareObject(that,['requireMask'])
	}
	guiClass { ^SimpleKDRUnitGui }
	report {
		Post << KeyCodeResponder.maskToString(requireMask);
		if(description.notNil,{ Post << " : " << description; });
	}

}


// test multiple modifiers deny and require
KDRMaskTester : SimpleKDRUnit {

	var <>denyMask;

	*new { arg requireMask,denyMask,function,description;
		var r=0,d=0;
		requireMask.do({ arg m; r = r | m });
		denyMask.do({ arg m; d = d | m });
		^super.new(r,function,description).denyMask_(d)
	}
	value { arg view, char,modifier,unicode,keycode;
//		[modifier & requireMask, requireMask].debug("require");
//		[denyMask & modifier].debug("deny");
		if((modifier & requireMask) == requireMask // all required bits set
			and:
		{  (denyMask & modifier) == 0 } // no denied bits present
		,{
			^function.value(view, char,modifier,unicode,keycode);
		},{^nil})
	}
	== { arg aResponder;
		^(this.class === aResponder.class) and: {
			(aResponder.requireMask == requireMask and: {
				aResponder.denyMask == denyMask
			})
		}
	}
}
