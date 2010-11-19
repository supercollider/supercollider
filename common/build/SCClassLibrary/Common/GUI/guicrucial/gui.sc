
+ Object {

	// create a gui using the guiClass passing in args
	// the first arg is the parent: a window or view
	// and the second arg is the bounds: a Rect, Point (width x height), or nil
	gui { arg  ... args;
		^this.guiClass.new(this).performList(\gui,args);
	}

	debug { arg caller;
		// by using this rather than just postln
		// you can command-shift-y on debug and find every one you left
		if(caller.notNil,{
			Post << caller << ": " << this << Char.nl;
		},{
			Post << this << Char.nl;
		});
	}
	die { arg ... culprits;
		"\n\nFATAL ERROR:\n".postln;
		culprits.do({ arg c; if(c.isString,{c.postln},{c.dump}) });
		this.dump;
		Error("FATAL ERROR").throw;
	}
	checkKind { arg shouldBeKindOf;
		if(this.isKindOf(shouldBeKindOf).not,{
			Error("Type check failed:" + this + "should be kind of:" + shouldBeKindOf).throw;
		})
	}
	guiClass { ^ObjectGui }

}

+ Collection {
	debug { arg caller;
		// by using this rather than just postln
		// you can command-shift-y on debug and find every one you left
		if(caller.notNil,{
			if(this.size < 10,{
				Post << caller << ": " << this << Char.nl;
			},{
				Post << caller << ": " <<* this << Char.nl;
			});
		},{
			Post <<* this << Char.nl;
		});
	}
}

// these all get simple guis : just a label with the name
+ String {
	guiClass { ^StringGui }
}
+ Symbol {
	guiClass { ^StringGui }
}
// though it would be nice to have it indicated what class it is by color etc
+ SimpleNumber {
	guiClass { ^StringGui }
}
+ Boolean {
	guiClass { ^StringGui }
}
+ Nil {
	guiClass { ^StringGui }
}


+ RawArray { // string, signal
	debug { arg caller;
		// by using this rather than just postln
		// you can command-shift-y on debug and find every one you left
		if(caller.notNil,{
			Post << caller << ": " << this << Char.nl;
		},{
			Post << this << Char.nl;
		});
	}
}


/* layout support */
+ Nil {

	asFlowView { arg bounds;
		^FlowView(nil,bounds)
	}
}


+ FlowView {
	asFlowView {}
	asPageLayout {} // should be compatible
}

