
+ Object {
	state { ^nil }
	state_ {}
}	

+ Patch {
	state { 
		var s;
		s = Environment.new;
		args.do({ arg ag,i;
			s.put(instr.argNameAt(i), ag.state) // or nil is fine
		});
		^s
	}
	state_ { arg state;
		args.do({ arg ag,i;
			ag.state_( state.at( instr.argNameAt(i) ) )
		});
	}
}

+ Editor {
	state { ^value }
	state_ { arg val; this.value_(val).changed(this); }
}

+ HasSubject {
	state { ^subject.state }
	state_ { arg state; subject.state_(state) }
}
